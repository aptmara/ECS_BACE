/**
 * @file RenderSystem.h
 * @brief テクスチャ対応レンダリングシステム
 * @author 山内陽
 * @date 2025
 * @version 5.0
 * 
 * @details
 * ECSワールド内のMeshRendererコンポーネントを持つエンティティを
 * 自動的に描画するレンダリングシステムです。
 */
#pragma once
#include "graphics/GfxDevice.h"
#include "graphics/Camera.h"
#include "ecs/World.h"
#include "components/Transform.h"
#include "components/MeshRenderer.h"
#include "graphics/TextureManager.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <cstring>
#include <cstdio>

#pragma comment(lib, "d3dcompiler.lib")

/**
 * @struct RenderSystem
 * @brief テクスチャ対応レンダリングシステム
 * 
 * @details
 * ECSワールド内のすべての描画可能なエンティティ(Transform + MeshRenderer)を
 * 自動的に描画します。単色描画とテクスチャ描画の両方に対応しています。
 * 
 * ### レンダリングパイプライン:
 * 1. Transform から World 行列を計算
 * 2. Camera から View・Projection 行列を取得
 * 3. MeshRenderer の色・テクスチャ設定を適用
 * 4. キューブメッシュを描画
 * 
 * @par 使用例
 * @code
 * RenderSystem renderer;
 * renderer.Init(gfx, texManager);
 * 
 * // 毎フレーム
 * gfx.BeginFrame();
 * renderer.Render(gfx, world, camera);
 * gfx.EndFrame();
 * @endcode
 * 
 * @note Transform と MeshRenderer の両方を持つエンティティのみ描画されます
 * 
 * @author 山内陽
 */
struct RenderSystem {
    // パイプラインオブジェクト
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vs_;      ///< 頂点シェーダー
    Microsoft::WRL::ComPtr<ID3D11PixelShader> ps_;       ///< ピクセルシェーダー
    Microsoft::WRL::ComPtr<ID3D11InputLayout> layout_;   ///< 入力レイアウト
    Microsoft::WRL::ComPtr<ID3D11Buffer> cb_;            ///< 定数バッファ(VS用)
    Microsoft::WRL::ComPtr<ID3D11Buffer> vb_;            ///< 頂点バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> ib_;            ///< インデックスバッファ
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterState_;  ///< ラスタライザステート
    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState_;    ///< サンプラーステート
    UINT indexCount_ = 0;  ///< インデックス数

    TextureManager* texManager_ = nullptr;  ///< テクスチャマネージャーへのポインタ

    /**
     * @struct VSConstants
     * @brief 頂点シェーダー定数バッファ
     */
    struct VSConstants {
        DirectX::XMMATRIX WVP;  ///< World * View * Projection 行列
        DirectX::XMFLOAT4 uvTransform;  ///< xy=offset, zw=scale
    };

    /**
     * @struct PSConstants
     * @brief ピクセルシェーダー定数バッファ
     */
    struct PSConstants {
        DirectX::XMFLOAT4 color;  ///< 基本色
        float useTexture;  ///< 0=カラー, 1=テクスチャ
        float padding[3];  ///< パディング(16バイトアライメント)
    };

    Microsoft::WRL::ComPtr<ID3D11Buffer> psCb_;  ///< PSの定数バッファ

    /**
     * @brief デストラクタ
     * 
     * @details
     * すべてのDirectX11リソースを自動的に解放します。
     */
    ~RenderSystem() {
        vs_.Reset();
        ps_.Reset();
        layout_.Reset();
        cb_.Reset();
        psCb_.Reset();
        vb_.Reset();
        ib_.Reset();
        rasterState_.Reset();
        samplerState_.Reset();
    }

    /**
     * @brief 初期化
     * @param[in] gfx グラフィックスデバイス
     * @param[in] texMgr テクスチャマネージャー
     * @return bool 初期化が成功した場合は true
     * 
     * @details
     * シェーダーのコンパイル、パイプラインステートの作成、
     * キューブメッシュの作成を行います。
     */
    bool Init(GfxDevice& gfx, TextureManager& texMgr) {
        texManager_ = &texMgr;

        // テクスチャ対応シェーダー
        const char* VS = R"(
            cbuffer CB : register(b0) { 
                float4x4 gWVP; 
                float4 gUVTransform; // xy=offset, zw=scale
            };
            struct VSIn { 
                float3 pos : POSITION; 
                float2 tex : TEXCOORD; 
            };
            struct VSOut { 
                float4 pos : SV_POSITION; 
                float2 tex : TEXCOORD; 
            };
            VSOut main(VSIn i){
                VSOut o;
                o.pos = mul(float4(i.pos,1), gWVP);
                o.tex = i.tex * gUVTransform.zw + gUVTransform.xy;
                return o;
            }
        )";
        
        const char* PS = R"(
            cbuffer CB : register(b0) {
                float4 gColor;
                float gUseTexture;
                float3 padding;
            };
            Texture2D gTexture : register(t0);
            SamplerState gSampler : register(s0);
            struct VSOut { 
                float4 pos : SV_POSITION; 
                float2 tex : TEXCOORD; 
            };
            float4 main(VSOut i) : SV_Target { 
                if (gUseTexture > 0.5) {
                    return gTexture.Sample(gSampler, i.tex) * gColor;
                }
                return gColor;
            }
        )";

        Microsoft::WRL::ComPtr<ID3DBlob> vsb, psb, err;
        HRESULT hr = D3DCompile(VS, strlen(VS), nullptr, nullptr, nullptr, "main", "vs_5_0", 0, 0, vsb.GetAddressOf(), err.GetAddressOf());
        if (FAILED(hr)) {
            if (err) {
                char msg[512];
                sprintf_s(msg, "Vertex Shader compile error:\n%s", (char*)err->GetBufferPointer());
                MessageBoxA(nullptr, msg, "Shader Error", MB_OK | MB_ICONERROR);
            }
            return false;
        }
        
        hr = D3DCompile(PS, strlen(PS), nullptr, nullptr, nullptr, "main", "ps_5_0", 0, 0, psb.GetAddressOf(), err.ReleaseAndGetAddressOf());
        if (FAILED(hr)) {
            if (err) {
                char msg[512];
                sprintf_s(msg, "Pixel Shader compile error:\n%s", (char*)err->GetBufferPointer());
                MessageBoxA(nullptr, msg, "Shader Error", MB_OK | MB_ICONERROR);
            }
            return false;
        }

        if (FAILED(gfx.Dev()->CreateVertexShader(vsb->GetBufferPointer(), vsb->GetBufferSize(), nullptr, vs_.GetAddressOf()))) {
            return false;
        }
        
        if (FAILED(gfx.Dev()->CreatePixelShader(psb->GetBufferPointer(), psb->GetBufferSize(), nullptr, ps_.GetAddressOf()))) {
            return false;
        }

        // 入力レイアウト(Position + TexCoord)
        D3D11_INPUT_ELEMENT_DESC il[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };
        if (FAILED(gfx.Dev()->CreateInputLayout(il, 2, vsb->GetBufferPointer(), vsb->GetBufferSize(), layout_.GetAddressOf()))) {
            return false;
        }

        // VS定数バッファ
        D3D11_BUFFER_DESC cbd{};
        cbd.ByteWidth = sizeof(VSConstants);
        cbd.Usage = D3D11_USAGE_DEFAULT;
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        if (FAILED(gfx.Dev()->CreateBuffer(&cbd, nullptr, cb_.GetAddressOf()))) {
            return false;
        }

        // PS定数バッファ
        cbd.ByteWidth = sizeof(PSConstants);
        if (FAILED(gfx.Dev()->CreateBuffer(&cbd, nullptr, psCb_.GetAddressOf()))) {
            return false;
        }

        // サンプラーステート
        D3D11_SAMPLER_DESC sampDesc{};
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.MaxAnisotropy = 1;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
        if (FAILED(gfx.Dev()->CreateSamplerState(&sampDesc, &samplerState_))) {
            return false;
        }

        // ラスタライザステート
        D3D11_RASTERIZER_DESC rsd{};
        rsd.FillMode = D3D11_FILL_SOLID;
        rsd.CullMode = D3D11_CULL_BACK;
        rsd.FrontCounterClockwise = FALSE;
        rsd.DepthClipEnable = TRUE;
        if (FAILED(gfx.Dev()->CreateRasterizerState(&rsd, rasterState_.GetAddressOf()))) {
            return false;
        }

        // ジオメトリ(Y座標軸付きキューブ)
        struct V { DirectX::XMFLOAT3 pos; DirectX::XMFLOAT2 tex; };
        const float c = 0.5f;
        V verts[] = {
            // 背面
            {{-c,-c,-c}, {0,1}}, {{-c,+c,-c}, {0,0}}, {{+c,+c,-c}, {1,0}}, {{+c,-c,-c}, {1,1}},
            // 前面
            {{-c,-c,+c}, {1,1}}, {{-c,+c,+c}, {1,0}}, {{+c,+c,+c}, {0,0}}, {{+c,-c,+c}, {0,1}},
        };
        uint16_t idx[] = {
            0,1,2, 0,2,3,  // 背面
            4,6,5, 4,7,6,  // 前面
            4,5,1, 4,1,0,  // 左
            3,2,6, 3,6,7,  // 右
            1,5,6, 1,6,2,  // 上
            4,0,3, 4,3,7   // 下
        };
        indexCount_ = (UINT)(sizeof(idx) / sizeof(idx[0]));

        D3D11_BUFFER_DESC vbd{};
        vbd.ByteWidth = (UINT)sizeof(verts);
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.Usage = D3D11_USAGE_IMMUTABLE;
        D3D11_SUBRESOURCE_DATA vinit{ verts, 0, 0 };
        if (FAILED(gfx.Dev()->CreateBuffer(&vbd, &vinit, vb_.GetAddressOf()))) {
            return false;
        }

        D3D11_BUFFER_DESC ibd{};
        ibd.ByteWidth = (UINT)sizeof(idx);
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.Usage = D3D11_USAGE_IMMUTABLE;
        D3D11_SUBRESOURCE_DATA iinit{ idx, 0, 0 };
        if (FAILED(gfx.Dev()->CreateBuffer(&ibd, &iinit, ib_.GetAddressOf()))) {
            return false;
        }

        return true;
    }

    /**
     * @brief レンダリング実行
     * @param[in] gfx グラフィックスデバイス
     * @param[in] w ECSワールド
     * @param[in] cam カメラ
     * 
     * @details
     * ワールド内のすべてのMeshRendererを持つエンティティを描画します。
     * 
     * ### 描画の流れ:
     * 1. パイプラインステートを設定
     * 2. 各エンティティに対して:
     *    - World行列を計算
     *    - WVP行列を定数バッファに設定
     *    - 色とテクスチャを設定
     *    - キューブを描画
     */
    void Render(GfxDevice& gfx, World& w, const Camera& cam) {
        gfx.Ctx()->IASetInputLayout(layout_.Get());
        gfx.Ctx()->VSSetShader(vs_.Get(), nullptr, 0);
        gfx.Ctx()->PSSetShader(ps_.Get(), nullptr, 0);
        gfx.Ctx()->VSSetConstantBuffers(0, 1, cb_.GetAddressOf());
        gfx.Ctx()->PSSetConstantBuffers(0, 1, psCb_.GetAddressOf());
        gfx.Ctx()->PSSetSamplers(0, 1, samplerState_.GetAddressOf());
        gfx.Ctx()->RSSetState(rasterState_.Get());

        UINT stride = sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT2);
        UINT offset = 0;
        gfx.Ctx()->IASetVertexBuffers(0, 1, vb_.GetAddressOf(), &stride, &offset);
        gfx.Ctx()->IASetIndexBuffer(ib_.Get(), DXGI_FORMAT_R16_UINT, 0);
        gfx.Ctx()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        w.ForEach<MeshRenderer>([&](Entity e, MeshRenderer& mr) {
            auto* t = w.TryGet<Transform>(e);
            if (!t) return;

            // ワールド行列
            DirectX::XMMATRIX S = DirectX::XMMatrixScaling(t->scale.x, t->scale.y, t->scale.z);
            DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(
                DirectX::XMConvertToRadians(t->rotation.x),
                DirectX::XMConvertToRadians(t->rotation.y),
                DirectX::XMConvertToRadians(t->rotation.z));
            DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(t->position.x, t->position.y, t->position.z);
            DirectX::XMMATRIX W = S * R * T;

            // VS定数バッファ
            VSConstants vsCbuf;
            vsCbuf.WVP = DirectX::XMMatrixTranspose(W * cam.View * cam.Proj);
            vsCbuf.uvTransform = DirectX::XMFLOAT4{ mr.uvOffset.x, mr.uvOffset.y, mr.uvScale.x, mr.uvScale.y };
            gfx.Ctx()->UpdateSubresource(cb_.Get(), 0, nullptr, &vsCbuf, 0, 0);

            // PS定数バッファ
            PSConstants psCbuf;
            psCbuf.color = DirectX::XMFLOAT4{ mr.color.x, mr.color.y, mr.color.z, 1.0f };
            psCbuf.useTexture = (mr.texture != TextureManager::INVALID_TEXTURE) ? 1.0f : 0.0f;
            gfx.Ctx()->UpdateSubresource(psCb_.Get(), 0, nullptr, &psCbuf, 0, 0);

            // テクスチャ設定
            if (mr.texture != TextureManager::INVALID_TEXTURE && texManager_) {
                ID3D11ShaderResourceView* srv = texManager_->GetSRV(mr.texture);
                if (srv) {
                    gfx.Ctx()->PSSetShaderResources(0, 1, &srv);
                }
            }

            gfx.Ctx()->DrawIndexed(indexCount_, 0, 0);
        });
    }
};
