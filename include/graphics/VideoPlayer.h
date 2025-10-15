#pragma once
#include "graphics/GfxDevice.h"
#include "components/Component.h"
#include "ecs/Entity.h"
#include "ecs/World.h"
#include <d3d11.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <string>
#include <cstdio>

#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

// ========================================================
// VideoPlayer - ����Đ��V�X�e��
// ========================================================
class VideoPlayer {
public:
    bool Init() {
        // Media Foundation������
        HRESULT hr = MFStartup(MF_VERSION);
        if (FAILED(hr)) {
            MessageBoxA(nullptr, "Failed to initialize Media Foundation", "Video Error", MB_OK | MB_ICONERROR);
            return false;
        }
        return true;
    }

    ~VideoPlayer() {
        if (reader_) reader_.Reset();
        MFShutdown();
    }

    // ����t�@�C�����J��
    bool Open(GfxDevice& gfx, const char* filepath) {
        gfx_ = &gfx;
        
        // ���C�h������ɕϊ�
        wchar_t wpath[MAX_PATH];
        MultiByteToWideChar(CP_ACP, 0, filepath, -1, wpath, MAX_PATH);

        // �\�[�X���[�_�[���쐬
        Microsoft::WRL::ComPtr<IMFAttributes> attributes;
        HRESULT hr = MFCreateAttributes(&attributes, 1);
        if (FAILED(hr)) return false;

        hr = attributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);
        if (FAILED(hr)) return false;

        hr = MFCreateSourceReaderFromURL(wpath, attributes.Get(), &reader_);
        if (FAILED(hr)) {
            char msg[512];
            sprintf_s(msg, "Failed to open video file: %s", filepath);
            MessageBoxA(nullptr, msg, "Video Error", MB_OK | MB_ICONERROR);
            return false;
        }

        // �r�f�I�X�g���[����I��
        hr = reader_->SetStreamSelection((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, TRUE);
        if (FAILED(hr)) return false;

        // �o�̓��f�B�A�^�C�v��ݒ�iRGB32�j
        Microsoft::WRL::ComPtr<IMFMediaType> mediaType;
        hr = MFCreateMediaType(&mediaType);
        if (FAILED(hr)) return false;

        hr = mediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
        if (FAILED(hr)) return false;

        hr = mediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);
        if (FAILED(hr)) return false;

        hr = reader_->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, mediaType.Get());
        if (FAILED(hr)) return false;

        // ����T�C�Y�ƒ������擾
        Microsoft::WRL::ComPtr<IMFMediaType> currentType;
        hr = reader_->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, &currentType);
        if (FAILED(hr)) return false;

        UINT32 w, h;
        hr = MFGetAttributeSize(currentType.Get(), MF_MT_FRAME_SIZE, &w, &h);
        if (FAILED(hr)) return false;

        width_ = w;
        height_ = h;

        // ���I�e�N�X�`�����쐬
        if (!createVideoTexture()) return false;

        isOpen_ = true;
        return true;
    }

    // �t���[�����X�V
    bool Update(float dt) {
        if (!isOpen_ || !isPlaying_) return false;

        currentTime_ += dt;

        // �t���[����ǂݍ���
        DWORD streamFlags = 0;
        LONGLONG timestamp = 0;
        Microsoft::WRL::ComPtr<IMFSample> sample;

        HRESULT hr = reader_->ReadSample(
            (DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
            0,
            nullptr,
            &streamFlags,
            &timestamp,
            &sample
        );

        if (FAILED(hr)) return false;

        // �X�g���[���̏I�[
        if (streamFlags & MF_SOURCE_READERF_ENDOFSTREAM) {
            if (loop_) {
                // ���[�v�Đ�
                PROPVARIANT var{};
                var.vt = VT_I8;
                var.hVal.QuadPart = 0;
                reader_->SetCurrentPosition(GUID_NULL, var);
                PropVariantClear(&var);
                return true;
            } else {
                isPlaying_ = false;
                return false;
            }
        }

        if (!sample) return true;

        // �T���v������o�b�t�@���擾
        Microsoft::WRL::ComPtr<IMFMediaBuffer> buffer;
        hr = sample->ConvertToContiguousBuffer(&buffer);
        if (FAILED(hr)) return false;

        BYTE* data = nullptr;
        DWORD length = 0;
        hr = buffer->Lock(&data, nullptr, &length);
        if (FAILED(hr)) return false;

        // �e�N�X�`�����X�V
        D3D11_MAPPED_SUBRESOURCE mapped;
        hr = gfx_->Ctx()->Map(videoTexture_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        if (SUCCEEDED(hr)) {
            // �f�[�^���R�s�[
            uint8_t* dest = static_cast<uint8_t*>(mapped.pData);
            uint8_t* src = data;
            
            for (UINT y = 0; y < height_; ++y) {
                memcpy(dest, src, width_ * 4);
                dest += mapped.RowPitch;
                src += width_ * 4;
            }
            
            gfx_->Ctx()->Unmap(videoTexture_.Get(), 0);
        }

        buffer->Unlock();
        return true;
    }

    // �Đ��E��~
    void Play() { isPlaying_ = true; }
    void Stop() { isPlaying_ = false; }
    void SetLoop(bool loop) { loop_ = loop; }

    // �e�N�X�`�����擾
    ID3D11ShaderResourceView* GetSRV() const { return videoSRV_.Get(); }

    bool IsPlaying() const { return isPlaying_; }
    UINT GetWidth() const { return width_; }
    UINT GetHeight() const { return height_; }

private:
    bool createVideoTexture() {
        D3D11_TEXTURE2D_DESC texDesc{};
        texDesc.Width = width_;
        texDesc.Height = height_;
        texDesc.MipLevels = 1;
        texDesc.ArraySize = 1;
        texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        texDesc.SampleDesc.Count = 1;
        texDesc.Usage = D3D11_USAGE_DYNAMIC;
        texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        HRESULT hr = gfx_->Dev()->CreateTexture2D(&texDesc, nullptr, &videoTexture_);
        if (FAILED(hr)) {
            MessageBoxA(nullptr, "Failed to create video texture", "Video Error", MB_OK | MB_ICONERROR);
            return false;
        }

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = texDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;

        hr = gfx_->Dev()->CreateShaderResourceView(videoTexture_.Get(), &srvDesc, &videoSRV_);
        if (FAILED(hr)) {
            MessageBoxA(nullptr, "Failed to create video SRV", "Video Error", MB_OK | MB_ICONERROR);
            return false;
        }

        return true;
    }

    GfxDevice* gfx_ = nullptr;
    Microsoft::WRL::ComPtr<IMFSourceReader> reader_;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> videoTexture_;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> videoSRV_;

    UINT width_ = 0;
    UINT height_ = 0;
    bool isOpen_ = false;
    bool isPlaying_ = false;
    bool loop_ = false;
    float currentTime_ = 0.0f;
};

// ========================================================
// VideoPlayback - ����Đ��R���|�[�l���g
// ========================================================
struct VideoPlayback : Behaviour {
    VideoPlayer* player = nullptr; // VideoPlayer�ւ̃|�C���^
    bool autoPlay = true;

    void OnStart(World& w, Entity self) override {
        if (autoPlay && player) {
            player->Play();
        }
    }

    void OnUpdate(World& w, Entity self, float dt) override {
        if (player) {
            player->Update(dt);
        }
    }
};
