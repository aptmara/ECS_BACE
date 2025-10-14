#pragma once
#include "GfxDevice.h"
#include "Camera.h"
#include "World.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <cstring>

#pragma comment(lib, "d3dcompiler.lib")

// ========================================================
// RenderSystem - レンダリングシステム
// ========================================================
struct RenderSystem {
    // パイプラインオブジェクト
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vs_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> ps_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> layout_;
    Microsoft::WRL::ComPtr<ID3D11Buffer> cb_; // 定数バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> vb_; // 頂点バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> ib_; // インデックスバッファ
    UINT indexCount_ = 0;

    // 頂点シェーダー定数バッファ
    struct VSConstants {
        DirectX::XMMATRIX WVP;
    };

    // 初期化
    bool Init(GfxDevice& gfx) {
        // 1) シェーダーをコンパイル
        const char* VS = R"(
            cbuffer CB : register(b0) { float4x4 gWVP; };
            struct VSIn { float3 pos : POSITION; float3 col : COLOR; };
            struct VSOut { float4 pos : SV_POSITION; float3 col : COLOR; };
            VSOut main(VSIn i){
                VSOut o;
                o.pos = mul(float4(i.pos,1), gWVP);
                o.col = i.col;
                return o;
            }
        )";
        const char* PS = R"(
            struct VSOut { float4 pos : SV_POSITION; float3 col : COLOR; };
            float4 main(VSOut i) : SV_Target { return float4(i.col,1); }
        )";

        Microsoft::WRL::ComPtr<ID3DBlob> vsb, psb, err;
        if (FAILED(D3DCompile(VS, strlen(VS), nullptr, nullptr, nullptr, "main", "vs_5_0", 0, 0, vsb.GetAddressOf(), err.GetAddressOf())))
            return false;
        if (FAILED(D3DCompile(PS, strlen(PS), nullptr, nullptr, nullptr, "main", "ps_5_0", 0, 0, psb.GetAddressOf(), err.ReleaseAndGetAddressOf())))
            return false;

        if (FAILED(gfx.Dev()->CreateVertexShader(vsb->GetBufferPointer(), vsb->GetBufferSize(), nullptr, vs_.GetAddressOf())))
            return false;
        if (FAILED(gfx.Dev()->CreatePixelShader(psb->GetBufferPointer(), psb->GetBufferSize(), nullptr, ps_.GetAddressOf())))
            return false;

        // 2) 入力レイアウト
        D3D11_INPUT_ELEMENT_DESC il[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };
        if (FAILED(gfx.Dev()->CreateInputLayout(il, 2, vsb->GetBufferPointer(), vsb->GetBufferSize(), layout_.GetAddressOf())))
            return false;

        // 3) 定数バッファ
        D3D11_BUFFER_DESC cbd{};
        cbd.ByteWidth = sizeof(VSConstants);
        cbd.Usage = D3D11_USAGE_DEFAULT;
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        if (FAILED(gfx.Dev()->CreateBuffer(&cbd, nullptr, cb_.GetAddressOf())))
            return false;

        // 4) ジオメトリ（単位キューブ）
        struct V { DirectX::XMFLOAT3 pos; DirectX::XMFLOAT3 col; };
        const float c = 0.5f;
        V verts[] = {
            {{-c,-c,-c},{1,0,0}}, {{-c,+c,-c},{1,0,0}}, {{+c,+c,-c},{1,0,0}}, {{+c,-c,-c},{1,0,0}}, // 背面: 赤
            {{-c,-c,+c},{0,1,0}}, {{-c,+c,+c},{0,1,0}}, {{+c,+c,+c},{0,1,0}}, {{+c,-c,+c},{0,1,0}}, // 前面: 緑
        };
        uint16_t idx[] = {
            // 背面
            0,1,2, 0,2,3,
            // 前面
            4,6,5, 4,7,6,
            // 左
            4,5,1, 4,1,0,
            // 右
            3,2,6, 3,6,7,
            // 上
            1,5,6, 1,6,2,
            // 下
            4,0,3, 4,3,7
        };
        indexCount_ = (UINT)(sizeof(idx) / sizeof(idx[0]));

        D3D11_BUFFER_DESC vbd{};
        vbd.ByteWidth = (UINT)sizeof(verts);
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.Usage = D3D11_USAGE_IMMUTABLE;
        D3D11_SUBRESOURCE_DATA vinit{ verts, 0, 0 };
        if (FAILED(gfx.Dev()->CreateBuffer(&vbd, &vinit, vb_.GetAddressOf())))
            return false;

        D3D11_BUFFER_DESC ibd{};
        ibd.ByteWidth = (UINT)sizeof(idx);
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.Usage = D3D11_USAGE_IMMUTABLE;
        D3D11_SUBRESOURCE_DATA iinit{ idx, 0, 0 };
        if (FAILED(gfx.Dev()->CreateBuffer(&ibd, &iinit, ib_.GetAddressOf())))
            return false;

        return true;
    }

    // レンダリング実行
    void Render(GfxDevice& gfx, World& w, const Camera& cam) {
        // パイプラインバインド
        gfx.Ctx()->IASetInputLayout(layout_.Get());
        gfx.Ctx()->VSSetShader(vs_.Get(), nullptr, 0);
        gfx.Ctx()->PSSetShader(ps_.Get(), nullptr, 0);
        gfx.Ctx()->VSSetConstantBuffers(0, 1, cb_.GetAddressOf());

        UINT stride = sizeof(DirectX::XMFLOAT3) * 2;
        UINT offset = 0;
        gfx.Ctx()->IASetVertexBuffers(0, 1, vb_.GetAddressOf(), &stride, &offset);
        gfx.Ctx()->IASetIndexBuffer(ib_.Get(), DXGI_FORMAT_R16_UINT, 0);
        gfx.Ctx()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // 全MeshRendererを描画
        w.ForEach<MeshRenderer>([&](Entity e, MeshRenderer& mr) {
            auto* t = w.TryGet<Transform>(e);
            if (!t) return;

            // ワールド行列の構築
            DirectX::XMMATRIX S = DirectX::XMMatrixScaling(t->scale.x, t->scale.y, t->scale.z);
            DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(
                DirectX::XMConvertToRadians(t->rotation.x),
                DirectX::XMConvertToRadians(t->rotation.y),
                DirectX::XMConvertToRadians(t->rotation.z));
            DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(t->position.x, t->position.y, t->position.z);
            DirectX::XMMATRIX W = S * R * T;

            VSConstants cbuf;
            cbuf.WVP = DirectX::XMMatrixTranspose(W * cam.View * cam.Proj);
            gfx.Ctx()->UpdateSubresource(cb_.Get(), 0, nullptr, &cbuf, 0, 0);

            gfx.Ctx()->DrawIndexed(indexCount_, 0, 0);
        });
    }
};
