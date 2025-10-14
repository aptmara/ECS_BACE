#pragma once
#include "GfxDevice.h"
#include "Camera.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <vector>
#include <cstring>
#include <cstdio>

#pragma comment(lib, "d3dcompiler.lib")

// ========================================================
// DebugDraw - デバッグ用の線描画システム
// ========================================================
class DebugDraw {
public:
    struct Line {
        DirectX::XMFLOAT3 start;
        DirectX::XMFLOAT3 end;
        DirectX::XMFLOAT3 color;
    };

    // 初期化
    bool Init(GfxDevice& gfx) {
        // シェーダーのコンパイル
        const char* VS = R"(
            cbuffer CB : register(b0) { float4x4 gVP; };
            struct VSIn { float3 pos : POSITION; float3 col : COLOR; };
            struct VSOut { float4 pos : SV_POSITION; float3 col : COLOR; };
            VSOut main(VSIn i){
                VSOut o;
                o.pos = mul(float4(i.pos, 1), gVP);
                o.col = i.col;
                return o;
            }
        )";
        
        const char* PS = R"(
            struct VSOut { float4 pos : SV_POSITION; float3 col : COLOR; };
            float4 main(VSOut i) : SV_Target { return float4(i.col, 1); }
        )";

        Microsoft::WRL::ComPtr<ID3DBlob> vsb, psb, err;
        HRESULT hr = D3DCompile(VS, strlen(VS), nullptr, nullptr, nullptr, "main", "vs_5_0", 0, 0, vsb.GetAddressOf(), err.GetAddressOf());
        if (FAILED(hr)) {
            if (err) {
                char msg[512];
                sprintf_s(msg, "DebugDraw VS compile error:\n%s", (char*)err->GetBufferPointer());
                MessageBoxA(nullptr, msg, "Shader Error", MB_OK | MB_ICONERROR);
            }
            return false;
        }
        
        hr = D3DCompile(PS, strlen(PS), nullptr, nullptr, nullptr, "main", "ps_5_0", 0, 0, psb.GetAddressOf(), err.ReleaseAndGetAddressOf());
        if (FAILED(hr)) {
            if (err) {
                char msg[512];
                sprintf_s(msg, "DebugDraw PS compile error:\n%s", (char*)err->GetBufferPointer());
                MessageBoxA(nullptr, msg, "Shader Error", MB_OK | MB_ICONERROR);
            }
            return false;
        }

        if (FAILED(gfx.Dev()->CreateVertexShader(vsb->GetBufferPointer(), vsb->GetBufferSize(), nullptr, vs_.GetAddressOf()))) {
            MessageBoxA(nullptr, "Failed to create debug vertex shader", "Shader Error", MB_OK | MB_ICONERROR);
            return false;
        }
        
        if (FAILED(gfx.Dev()->CreatePixelShader(psb->GetBufferPointer(), psb->GetBufferSize(), nullptr, ps_.GetAddressOf()))) {
            MessageBoxA(nullptr, "Failed to create debug pixel shader", "Shader Error", MB_OK | MB_ICONERROR);
            return false;
        }

        // 入力レイアウト
        D3D11_INPUT_ELEMENT_DESC il[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };
        if (FAILED(gfx.Dev()->CreateInputLayout(il, 2, vsb->GetBufferPointer(), vsb->GetBufferSize(), layout_.GetAddressOf()))) {
            MessageBoxA(nullptr, "Failed to create debug input layout", "Shader Error", MB_OK | MB_ICONERROR);
            return false;
        }

        // 定数バッファ
        D3D11_BUFFER_DESC cbd{};
        cbd.ByteWidth = sizeof(DirectX::XMMATRIX);
        cbd.Usage = D3D11_USAGE_DEFAULT;
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        if (FAILED(gfx.Dev()->CreateBuffer(&cbd, nullptr, cb_.GetAddressOf()))) {
            MessageBoxA(nullptr, "Failed to create debug constant buffer", "Buffer Error", MB_OK | MB_ICONERROR);
            return false;
        }

        // 動的頂点バッファ（最大10000線分）
        maxLines_ = 10000;
        D3D11_BUFFER_DESC vbd{};
        vbd.ByteWidth = (UINT)(maxLines_ * 2 * sizeof(Vertex)); // 1線分 = 2頂点
        vbd.Usage = D3D11_USAGE_DYNAMIC;
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        if (FAILED(gfx.Dev()->CreateBuffer(&vbd, nullptr, vb_.GetAddressOf()))) {
            MessageBoxA(nullptr, "Failed to create debug vertex buffer", "Buffer Error", MB_OK | MB_ICONERROR);
            return false;
        }

        return true;
    }

    // 線を追加
    void AddLine(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const DirectX::XMFLOAT3& color) {
        lines_.push_back({ start, end, color });
    }

    // グリッドを描画
    void DrawGrid(float size = 10.0f, int divisions = 10, const DirectX::XMFLOAT3& color = {0.5f, 0.5f, 0.5f}) {
        float step = size / divisions;
        float halfSize = size * 0.5f;

        // X-Z平面のグリッド
        for (int i = 0; i <= divisions; ++i) {
            float pos = -halfSize + i * step;
            
            // Z軸に平行な線（X方向に並ぶ）
            AddLine(
                DirectX::XMFLOAT3{-halfSize, 0, pos},
                DirectX::XMFLOAT3{ halfSize, 0, pos},
                color
            );
            
            // X軸に平行な線（Z方向に並ぶ）
            AddLine(
                DirectX::XMFLOAT3{pos, 0, -halfSize},
                DirectX::XMFLOAT3{pos, 0,  halfSize},
                color
            );
        }
    }

    // 座標軸を描画
    void DrawAxes(float length = 5.0f) {
        // X軸（赤）
        AddLine(
            DirectX::XMFLOAT3{0, 0, 0},
            DirectX::XMFLOAT3{length, 0, 0},
            DirectX::XMFLOAT3{1, 0, 0}
        );
        
        // Y軸（緑）
        AddLine(
            DirectX::XMFLOAT3{0, 0, 0},
            DirectX::XMFLOAT3{0, length, 0},
            DirectX::XMFLOAT3{0, 1, 0}
        );
        
        // Z軸（青）
        AddLine(
            DirectX::XMFLOAT3{0, 0, 0},
            DirectX::XMFLOAT3{0, 0, length},
            DirectX::XMFLOAT3{0, 0, 1}
        );
    }

    // すべての線を描画
    void Render(GfxDevice& gfx, const Camera& cam) {
        if (lines_.empty()) return;

        // 頂点データを準備
        std::vector<Vertex> vertices;
        vertices.reserve(lines_.size() * 2);
        
        for (const auto& line : lines_) {
            vertices.push_back({ line.start, line.color });
            vertices.push_back({ line.end, line.color });
        }

        // 頂点バッファを更新
        D3D11_MAPPED_SUBRESOURCE mapped;
        if (SUCCEEDED(gfx.Ctx()->Map(vb_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
            memcpy(mapped.pData, vertices.data(), vertices.size() * sizeof(Vertex));
            gfx.Ctx()->Unmap(vb_.Get(), 0);
        }

        // パイプライン設定
        gfx.Ctx()->IASetInputLayout(layout_.Get());
        gfx.Ctx()->VSSetShader(vs_.Get(), nullptr, 0);
        gfx.Ctx()->PSSetShader(ps_.Get(), nullptr, 0);
        gfx.Ctx()->VSSetConstantBuffers(0, 1, cb_.GetAddressOf());

        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        gfx.Ctx()->IASetVertexBuffers(0, 1, vb_.GetAddressOf(), &stride, &offset);
        gfx.Ctx()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

        // 定数バッファ更新（ワールド行列は単位行列）
        DirectX::XMMATRIX VP = DirectX::XMMatrixTranspose(cam.View * cam.Proj);
        gfx.Ctx()->UpdateSubresource(cb_.Get(), 0, nullptr, &VP, 0, 0);

        // 描画
        gfx.Ctx()->Draw((UINT)vertices.size(), 0);
    }

    // フレーム終了時にクリア
    void Clear() {
        lines_.clear();
    }

    ~DebugDraw() {
        vs_.Reset();
        ps_.Reset();
        layout_.Reset();
        cb_.Reset();
        vb_.Reset();
    }

private:
    struct Vertex {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT3 col;
    };

    Microsoft::WRL::ComPtr<ID3D11VertexShader> vs_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> ps_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> layout_;
    Microsoft::WRL::ComPtr<ID3D11Buffer> cb_;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vb_;
    
    std::vector<Line> lines_;
    size_t maxLines_;
};
