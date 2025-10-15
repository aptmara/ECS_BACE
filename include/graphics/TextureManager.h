#pragma once
#include "graphics/GfxDevice.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <wincodec.h>
#include <DirectXMath.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <fstream>
#include <cstdio>

#pragma comment(lib, "windowscodecs.lib")

// ========================================================
// TextureManager - �e�N�X�`���Ǘ��V�X�e��
// ========================================================
class TextureManager {
public:
    // �e�N�X�`���n���h��
    using TextureHandle = uint32_t;
    static constexpr TextureHandle INVALID_TEXTURE = 0;

    // ������
    bool Init(GfxDevice& gfx) {
        gfx_ = &gfx;
        
        // �f�t�H���g�̔��e�N�X�`�����쐬
        uint32_t whitePixel = 0xFFFFFFFF;
        defaultWhiteTexture_ = CreateTextureFromMemory(
            reinterpret_cast<const uint8_t*>(&whitePixel),
            1, 1, 4
        );
        
        return defaultWhiteTexture_ != INVALID_TEXTURE;
    }

    // �t�@�C������e�N�X�`����ǂݍ��݁iBMP�APNG�AJPG�Ȃǁj
    TextureHandle LoadFromFile(const char* filepath) {
        // WIC���g�p���ĉ摜��ǂݍ���
        Microsoft::WRL::ComPtr<IWICImagingFactory> wicFactory;
        HRESULT hr = CoCreateInstance(
            CLSID_WICImagingFactory,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&wicFactory)
        );
        
        if (FAILED(hr)) {
            char msg[512];
            sprintf_s(msg, "Failed to create WIC factory for: %s", filepath);
            MessageBoxA(nullptr, msg, "Texture Load Error", MB_OK | MB_ICONERROR);
            return INVALID_TEXTURE;
        }

        // ���C�h������ɕϊ�
        wchar_t wpath[MAX_PATH];
        MultiByteToWideChar(CP_ACP, 0, filepath, -1, wpath, MAX_PATH);

        // �f�R�[�_�[���쐬
        Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder;
        hr = wicFactory->CreateDecoderFromFilename(
            wpath,
            nullptr,
            GENERIC_READ,
            WICDecodeMetadataCacheOnDemand,
            &decoder
        );

        if (FAILED(hr)) {
            char msg[512];
            sprintf_s(msg, "Failed to load image file: %s", filepath);
            MessageBoxA(nullptr, msg, "Texture Load Error", MB_OK | MB_ICONERROR);
            return INVALID_TEXTURE;
        }

        // �t���[�����擾
        Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> frame;
        hr = decoder->GetFrame(0, &frame);
        if (FAILED(hr)) return INVALID_TEXTURE;

        // RGBA32�ɕϊ�
        Microsoft::WRL::ComPtr<IWICFormatConverter> converter;
        hr = wicFactory->CreateFormatConverter(&converter);
        if (FAILED(hr)) return INVALID_TEXTURE;

        hr = converter->Initialize(
            frame.Get(),
            GUID_WICPixelFormat32bppRGBA,
            WICBitmapDitherTypeNone,
            nullptr,
            0.0,
            WICBitmapPaletteTypeCustom
        );
        if (FAILED(hr)) return INVALID_TEXTURE;

        // �T�C�Y���擾
        UINT width, height;
        hr = converter->GetSize(&width, &height);
        if (FAILED(hr)) return INVALID_TEXTURE;

        // �s�N�Z���f�[�^���擾
        std::vector<uint8_t> pixels(width * height * 4);
        hr = converter->CopyPixels(
            nullptr,
            width * 4,
            static_cast<UINT>(pixels.size()),
            pixels.data()
        );
        if (FAILED(hr)) return INVALID_TEXTURE;

        return CreateTextureFromMemory(pixels.data(), width, height, 4);
    }

    // ����������e�N�X�`�����쐬
    TextureHandle CreateTextureFromMemory(const uint8_t* data, uint32_t width, uint32_t height, uint32_t channels) {
        D3D11_TEXTURE2D_DESC texDesc{};
        texDesc.Width = width;
        texDesc.Height = height;
        texDesc.MipLevels = 1;
        texDesc.ArraySize = 1;
        texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        texDesc.SampleDesc.Count = 1;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

        D3D11_SUBRESOURCE_DATA initData{};
        initData.pSysMem = data;
        initData.SysMemPitch = width * channels;

        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
        HRESULT hr = gfx_->Dev()->CreateTexture2D(&texDesc, &initData, &texture);
        if (FAILED(hr)) {
            MessageBoxA(nullptr, "Failed to create texture2D", "Texture Error", MB_OK | MB_ICONERROR);
            return INVALID_TEXTURE;
        }

        // �V�F�[�_�[���\�[�X�r���[���쐬
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
        srvDesc.Format = texDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = 1;

        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
        hr = gfx_->Dev()->CreateShaderResourceView(texture.Get(), &srvDesc, &srv);
        if (FAILED(hr)) {
            MessageBoxA(nullptr, "Failed to create SRV", "Texture Error", MB_OK | MB_ICONERROR);
            return INVALID_TEXTURE;
        }

        // �e�N�X�`����o�^
        TextureHandle handle = nextHandle_++;
        TextureData texData;
        texData.texture = texture;
        texData.srv = srv;
        texData.width = width;
        texData.height = height;
        textures_[handle] = texData;

        return handle;
    }

    // �e�N�X�`���̎擾
    ID3D11ShaderResourceView* GetSRV(TextureHandle handle) const {
        if (handle == INVALID_TEXTURE) return nullptr;
        auto it = textures_.find(handle);
        if (it == textures_.end()) return nullptr;
        return it->second.srv.Get();
    }

    // �f�t�H���g�e�N�X�`��
    TextureHandle GetDefaultWhite() const { return defaultWhiteTexture_; }

    // �e�N�X�`���̉��
    void Release(TextureHandle handle) {
        textures_.erase(handle);
    }

    ~TextureManager() {
        textures_.clear();
    }

private:
    struct TextureData {
        Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
        uint32_t width;
        uint32_t height;
    };

    GfxDevice* gfx_ = nullptr;
    std::unordered_map<TextureHandle, TextureData> textures_;
    TextureHandle nextHandle_ = 1;
    TextureHandle defaultWhiteTexture_ = INVALID_TEXTURE;
};
