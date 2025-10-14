#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <cstdint>
#include <cstdio>

// ========================================================
// GfxDevice - DirectX11デバイス管理クラス
// ========================================================
class GfxDevice {
public:
    // 初期化
    bool Init(HWND hwnd, uint32_t w, uint32_t h) {
        width_ = w;
        height_ = h;

        DXGI_SWAP_CHAIN_DESC sd{};
        sd.BufferCount = 2;
        sd.BufferDesc.Width = w;
        sd.BufferDesc.Height = h;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hwnd;
        sd.SampleDesc.Count = 1;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        UINT flags = 0;
#if defined(_DEBUG)
        flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
        D3D_FEATURE_LEVEL fl;
        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags,
            nullptr, 0, D3D11_SDK_VERSION,
            &sd,
            swap_.ReleaseAndGetAddressOf(),
            device_.ReleaseAndGetAddressOf(),
            &fl,
            context_.ReleaseAndGetAddressOf());
        
        if (FAILED(hr)) {
            // エラーの詳細をログ出力
            char errorMsg[256];
            sprintf_s(errorMsg, 
                "Failed to create D3D11 device.\nHRESULT: 0x%08X\n"
                "Please check:\n"
                "- DirectX 11 is installed\n"
                "- Graphics drivers are up to date", 
                hr);
            MessageBoxA(nullptr, errorMsg, "DirectX Error", MB_OK | MB_ICONERROR);
            return false;
        }

        return createBackbufferResources();
    }

    // フレーム開始（画面クリア）
    void BeginFrame(float r = 0.1f, float g = 0.1f, float b = 0.12f, float a = 1.0f) {
        float c[4] = { r, g, b, a };
        context_->OMSetRenderTargets(1, rtv_.GetAddressOf(), dsv_.Get());
        context_->ClearRenderTargetView(rtv_.Get(), c);
        context_->ClearDepthStencilView(dsv_.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

        D3D11_VIEWPORT vp{};
        vp.Width = static_cast<FLOAT>(width_);
        vp.Height = static_cast<FLOAT>(height_);
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        context_->RSSetViewports(1, &vp);
    }

    // フレーム終了（画面表示）
    void EndFrame() {
        swap_->Present(1, 0);
    }

    // デバイスアクセス
    ID3D11Device* Dev() const { return device_.Get(); }
    ID3D11DeviceContext* Ctx() const { return context_.Get(); }

    // サイズ取得
    uint32_t Width() const { return width_; }
    uint32_t Height() const { return height_; }
    
    // デストラクタでリソースを明示的に解放
    ~GfxDevice() {
        // ComPtrは自動で解放されるが、念のため明示的にリセット
        dsv_.Reset();
        rtv_.Reset();
        swap_.Reset();
        context_.Reset();
        device_.Reset();
    }

private:
    // バックバッファリソースの作成
    bool createBackbufferResources() {
        Microsoft::WRL::ComPtr<ID3D11Texture2D> back;
        HRESULT hr = swap_->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)back.GetAddressOf());
        if (FAILED(hr)) {
            MessageBoxA(nullptr, "Failed to get back buffer", "DirectX Error", MB_OK | MB_ICONERROR);
            return false;
        }
        
        hr = device_->CreateRenderTargetView(back.Get(), nullptr, rtv_.ReleaseAndGetAddressOf());
        if (FAILED(hr)) {
            MessageBoxA(nullptr, "Failed to create render target view", "DirectX Error", MB_OK | MB_ICONERROR);
            return false;
        }

        // 深度ステンシルバッファ
        D3D11_TEXTURE2D_DESC td{};
        td.Width = width_;
        td.Height = height_;
        td.MipLevels = 1;
        td.ArraySize = 1;
        td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        td.SampleDesc.Count = 1;
        td.Usage = D3D11_USAGE_DEFAULT;
        td.BindFlags = D3D11_BIND_DEPTH_STENCIL;

        Microsoft::WRL::ComPtr<ID3D11Texture2D> depth;
        hr = device_->CreateTexture2D(&td, nullptr, depth.GetAddressOf());
        if (FAILED(hr)) {
            MessageBoxA(nullptr, "Failed to create depth stencil texture", "DirectX Error", MB_OK | MB_ICONERROR);
            return false;
        }
        
        hr = device_->CreateDepthStencilView(depth.Get(), nullptr, dsv_.ReleaseAndGetAddressOf());
        if (FAILED(hr)) {
            MessageBoxA(nullptr, "Failed to create depth stencil view", "DirectX Error", MB_OK | MB_ICONERROR);
            return false;
        }

        return true;
    }

    // メンバ変数
    uint32_t width_ = 0, height_ = 0;
    Microsoft::WRL::ComPtr<ID3D11Device> device_;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context_;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swap_;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv_;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> dsv_;
};
