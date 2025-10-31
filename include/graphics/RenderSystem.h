#pragma once
#include "graphics/GfxDevice.h"
#include "graphics/Camera.h"
#include "ecs/World.h"
#include "components/Transform.h"
#include "components/MeshRenderer.h"
#include "components/ModelComponent.h"
#include "components/Light.h"
#include "graphics/TextureManager.h"
#include "app/DebugLog.h"
#include "app/ServiceLocator.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <cstring>
#include <cstdio>
#include <string>
#include <vector>

#pragma comment(lib, "d3dcompiler.lib")

struct RenderSystem {
    Microsoft::WRL::ComPtr<ID3D11VertexShader> vs_;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> ps_;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> layout_;
    Microsoft::WRL::ComPtr<ID3D11Buffer> vsCb_; // VS per-object constants
    Microsoft::WRL::ComPtr<ID3D11Buffer> psCb_; // PS per-object constants
    Microsoft::WRL::ComPtr<ID3D11Buffer> psLightCb_; // PS per-frame light constants
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterState_;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState_;

    struct VSConstants {
        DirectX::XMMATRIX World;
        DirectX::XMMATRIX WVP;
        DirectX::XMFLOAT4 uvTransform;
    };

    struct PSConstants {
        DirectX::XMFLOAT4 color;
        float useTexture;
        float useNormalMap;
        float specularPower;
        float padding;
    };

    struct PSLightConstants {
        DirectionalLight light;
        DirectX::XMFLOAT3 ambientColor{ 0.2f, 0.2f, 0.2f };
        float padding2;
        DirectX::XMFLOAT3 eyePos;
        float padding3;
    };

    ~RenderSystem() { Shutdown(); }

    void Shutdown() {
        vs_.Reset();
        ps_.Reset();
        layout_.Reset();
        vsCb_.Reset();
        psCb_.Reset();
        psLightCb_.Reset();
        rasterState_.Reset();
        samplerState_.Reset();
    }

    bool Init() {
        auto& gfx = ServiceLocator::Get<GfxDevice>();

        const char* VS = R"(
            cbuffer PerObject : register(b0) {
                float4x4 gWorld;
                float4x4 gWVP;
                float4 gUVTransform;
            };

            struct VSIn {
                float3 pos : POSITION;
                float2 tex : TEXCOORD;
                float3 nrm : NORMAL;
                float3 tan : TANGENT;
                float3 bitan : BITANGENT;
            };

            struct VSOut {
                float4 pos : SV_POSITION;
                float2 tex : TEXCOORD;
                float3 nrm : NORMAL;
                float3 tan : TANGENT;
                float3 bitan : BITANGENT;
                float3 worldPos : WORLDPOS;
            };

            VSOut main(VSIn i) {
                VSOut o;
                o.pos = mul(float4(i.pos, 1.0f), gWVP);
                o.worldPos = mul(float4(i.pos, 1.0f), gWorld).xyz;
                o.nrm = mul(i.nrm, (float3x3)gWorld);
                o.tan = mul(i.tan, (float3x3)gWorld);
                o.bitan = mul(i.bitan, (float3x3)gWorld);
                o.tex = i.tex * gUVTransform.zw + gUVTransform.xy;
                return o;
            }
        )";

        const char* PS = R"(
            struct DirectionalLight {
                float3 direction;
                float padding;
                float4 color;
            };

            cbuffer PerObject : register(b0) {
                float4 gColor;
                float gUseTexture;
                float gUseNormalMap;
                float gSpecularPower;
                float padding_obj;
            };

            cbuffer PerFrame : register(b1) {
                DirectionalLight gLight;
                float3 gAmbientColor;
                float padding_frame;
                float3 gEyePos;
                float padding_frame2;
            };

            Texture2D gTexture : register(t0);
            Texture2D gNormalMap : register(t1);
            SamplerState gSampler : register(s0);

            struct VSOut {
                float4 pos : SV_POSITION;
                float2 tex : TEXCOORD;
                float3 nrm : NORMAL;
                float3 tan : TANGENT;
                float3 bitan : BITANGENT;
                float3 worldPos : WORLDPOS;
            };

            float4 main(VSOut i) : SV_Target {
                float3 normal = normalize(i.nrm);
                if (gUseNormalMap > 0.5) {
                    float3x3 TBN = float3x3(normalize(i.tan), normalize(i.bitan), normalize(i.nrm));
                    float3 tangentNormal = gNormalMap.Sample(gSampler, i.tex).xyz * 2.0 - 1.0;
                    normal = normalize(mul(tangentNormal, TBN));
                }

                float light_factor = max(0.0f, dot(normal, -gLight.direction));

                float4 final_color = gColor;
                if (gUseTexture > 0.5) {
                    final_color *= gTexture.Sample(gSampler, i.tex);
                }

                float3 toEye = normalize(gEyePos - i.worldPos);
                float3 reflection = reflect(gLight.direction, normal);
                float spec_factor = pow(max(0.0f, dot(toEye, reflection)), gSpecularPower);

                float3 diffuse = final_color.rgb * gLight.color.rgb * light_factor;
                float3 ambient = final_color.rgb * gAmbientColor;
                float3 specular = gLight.color.rgb * spec_factor;

                return float4(diffuse + ambient + specular, final_color.a);
            }
        )";

        Microsoft::WRL::ComPtr<ID3DBlob> vsb, psb, err;
        UINT compileFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(_DEBUG)
        compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        auto logShaderError = [&](const char* stage, HRESULT hr) {
            char hrText[9]{};
            std::snprintf(hrText, sizeof(hrText), "%08X", static_cast<unsigned int>(hr));
            std::string message = "[RenderSystem] ";
            message += stage;
            message += " shader compile failed (hr=0x";
            message += hrText;
            message += ").";
            if (err) {
                message += " ";
                message.append(static_cast<const char*>(err->GetBufferPointer()), err->GetBufferSize());
            }
            DEBUGLOG_ERROR(message);
        };

        HRESULT hr = D3DCompile(VS, strlen(VS), nullptr, nullptr, nullptr, "main", "vs_5_0", compileFlags, 0, vsb.GetAddressOf(), err.GetAddressOf());
        if (FAILED(hr)) {
            logShaderError("vertex", hr);
            return false;
        }
        err.Reset();

        hr = D3DCompile(PS, strlen(PS), nullptr, nullptr, nullptr, "main", "ps_5_0", compileFlags, 0, psb.GetAddressOf(), err.GetAddressOf());
        if (FAILED(hr)) {
            logShaderError("pixel", hr);
            return false;
        }
        err.Reset();

        hr = gfx.Dev()->CreateVertexShader(vsb->GetBufferPointer(), vsb->GetBufferSize(), nullptr, vs_.GetAddressOf());
        if (FAILED(hr)) {
            DEBUGLOG_ERROR("[RenderSystem] Failed to create vertex shader.");
            return false;
        }

        hr = gfx.Dev()->CreatePixelShader(psb->GetBufferPointer(), psb->GetBufferSize(), nullptr, ps_.GetAddressOf());
        if (FAILED(hr)) {
            DEBUGLOG_ERROR("[RenderSystem] Failed to create pixel shader.");
            return false;
        }

        D3D11_INPUT_ELEMENT_DESC il[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };
        gfx.Dev()->CreateInputLayout(il, 5, vsb->GetBufferPointer(), vsb->GetBufferSize(), layout_.GetAddressOf());

        D3D11_BUFFER_DESC cbd{};
        cbd.Usage = D3D11_USAGE_DEFAULT;
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbd.ByteWidth = sizeof(VSConstants);
        gfx.Dev()->CreateBuffer(&cbd, nullptr, vsCb_.GetAddressOf());
        cbd.ByteWidth = sizeof(PSConstants);
        gfx.Dev()->CreateBuffer(&cbd, nullptr, psCb_.GetAddressOf());
        cbd.ByteWidth = sizeof(PSLightConstants);
        gfx.Dev()->CreateBuffer(&cbd, nullptr, psLightCb_.GetAddressOf());

        D3D11_SAMPLER_DESC sampDesc{};
        sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.MaxAnisotropy = 16;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        gfx.Dev()->CreateSamplerState(&sampDesc, &samplerState_);

        D3D11_RASTERIZER_DESC rsd{};
        rsd.FillMode = D3D11_FILL_SOLID;
        rsd.CullMode = D3D11_CULL_BACK;
        rsd.FrontCounterClockwise = FALSE;
        rsd.DepthClipEnable = TRUE;
        gfx.Dev()->CreateRasterizerState(&rsd, rasterState_.GetAddressOf());

        return true;
    }

    void Render(World& w, const Camera& cam) {
        auto& gfx = ServiceLocator::Get<GfxDevice>();
        auto& texMgr = ServiceLocator::Get<TextureManager>();

        gfx.Ctx()->IASetInputLayout(layout_.Get());
        gfx.Ctx()->VSSetShader(vs_.Get(), nullptr, 0);
        gfx.Ctx()->PSSetShader(ps_.Get(), nullptr, 0);
        gfx.Ctx()->VSSetConstantBuffers(0, 1, vsCb_.GetAddressOf());
        gfx.Ctx()->PSSetConstantBuffers(0, 1, psCb_.GetAddressOf());
        gfx.Ctx()->PSSetConstantBuffers(1, 1, psLightCb_.GetAddressOf());
        gfx.Ctx()->PSSetSamplers(0, 1, samplerState_.GetAddressOf());
        gfx.Ctx()->RSSetState(rasterState_.Get());
        gfx.Ctx()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        
        PSLightConstants lightCbuf;
        lightCbuf.eyePos = cam.position;
        w.ForEach<DirectionalLight>([&](Entity e, DirectionalLight& l) {
            lightCbuf.light = l;
        });
        gfx.Ctx()->UpdateSubresource(psLightCb_.Get(), 0, nullptr, &lightCbuf, 0, 0);

        w.ForEach<ModelComponent>([&](Entity e, ModelComponent& mc) {
            auto* t = w.TryGet<Transform>(e);
            if (!t) return;
            if (!mc.vertexBuffer || !mc.indexBuffer) return;

            UINT stride = sizeof(DirectX::XMFLOAT3) * 4 + sizeof(DirectX::XMFLOAT2);
            UINT offset = 0;
            gfx.Ctx()->IASetVertexBuffers(0, 1, mc.vertexBuffer.GetAddressOf(), &stride, &offset);
            gfx.Ctx()->IASetIndexBuffer(mc.indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

            DirectX::XMMATRIX S = DirectX::XMMatrixScaling(t->scale.x, t->scale.y, t->scale.z);
            DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(
                DirectX::XMConvertToRadians(t->rotation.x),
                DirectX::XMConvertToRadians(t->rotation.y),
                DirectX::XMConvertToRadians(t->rotation.z));
            DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(t->position.x, t->position.y, t->position.z);
            DirectX::XMMATRIX W = S * R * T;

            VSConstants vsCbuf;
            vsCbuf.World = DirectX::XMMatrixTranspose(W);
            vsCbuf.WVP = DirectX::XMMatrixTranspose(W * cam.View * cam.Proj);
            vsCbuf.uvTransform = DirectX::XMFLOAT4{ mc.uvOffset.x, mc.uvOffset.y, mc.uvScale.x, mc.uvScale.y };
            gfx.Ctx()->UpdateSubresource(vsCb_.Get(), 0, nullptr, &vsCbuf, 0, 0);

            PSConstants psCbuf;
            psCbuf.color = DirectX::XMFLOAT4{ mc.color.x, mc.color.y, mc.color.z, 1.0f };
            psCbuf.useTexture = (mc.texture != TextureManager::INVALID_TEXTURE) ? 1.0f : 0.0f;
            psCbuf.useNormalMap = (mc.normalTexture != TextureManager::INVALID_TEXTURE) ? 1.0f : 0.0f;
            psCbuf.specularPower = 32.0f; // Hardcoded for now
            gfx.Ctx()->UpdateSubresource(psCb_.Get(), 0, nullptr, &psCbuf, 0, 0);

            ID3D11ShaderResourceView* srvs[2] = { nullptr, nullptr };
            if (mc.texture != TextureManager::INVALID_TEXTURE) {
                srvs[0] = texMgr.GetSRV(mc.texture);
            }
            if (mc.normalTexture != TextureManager::INVALID_TEXTURE) {
                srvs[1] = texMgr.GetSRV(mc.normalTexture);
            }
            gfx.Ctx()->PSSetShaderResources(0, 2, srvs);

            gfx.Ctx()->DrawIndexed(mc.indexCount, 0, 0);
        });
    }
};
