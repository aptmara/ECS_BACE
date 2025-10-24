/**
 * @file RenderSystem.h
 * @brief テクスチャ・複数形状対応レンダリングシステム
 * @author 山内陽
 * @date 2025
 * @version 6.0
 *
 * @details
 * ECSワールド内のMeshRendererコンポーネントを持つエンティティを
 * 自動的に描画するレンダリングシステムです。
 * 複数の基本形状(立方体、球体、円柱、円錐、平面、カプセル)をサポートします。
 */
#pragma once
#include "graphics/GfxDevice.h"
#include "graphics/Camera.h"
#include "ecs/World.h"
#include "components/Transform.h"
#include "components/MeshRenderer.h"
#include "graphics/TextureManager.h"
#include "app/DebugLog.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <cstring>
#include <cstdio>
#include <string>
#include <vector>  // std::vectorのために追加

#pragma comment(lib, "d3dcompiler.lib")

/**
 * @struct RenderSystem
 * @brief テクスチャ・複数形状対応レンダリングシステム
 *
 * @details
 * ECSワールド内のすべての描画可能なエンティティ(Transform + MeshRenderer)を
 * 自動的に描画します。単色描画とテクスチャ描画の両方に対応し、
 * 立方体、球体、円柱、円錐、平面、カプセルなどの基本形状をサポートします。
 *
 * ### レンダリングパイプライン:
 * 1. Transform から World 行列を計算
 * 2. Camera から View・Projection 行列を取得
 * 3. MeshRenderer の色・テクスチャ・形状設定を適用
 * 4. 指定された形状のメッシュを描画
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
    Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterState_;  ///< ラスタライザステート
    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState_;    ///< サンプラーステート

    /**
     * @struct MeshData
     * @brief メッシュデータ(頂点バッファとインデックスバッファ)
     */
    struct MeshData {
        Microsoft::WRL::ComPtr<ID3D11Buffer> vb;  ///< 頂点バッファ
        Microsoft::WRL::ComPtr<ID3D11Buffer> ib;  ///< インデックスバッファ
        UINT indexCount = 0;                      ///< インデックス数
    };

    // 各形状のメッシュデータ
    MeshData meshes_[6];  ///< 各MeshType用のメッシュデータ配列

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
        DEBUGLOG("RenderSystem::~RenderSystem() - デストラクタ呼び出し");
        if (!isShutdown_) { DEBUGLOG_WARNING("RenderSystem::Shutdown()が明示的に呼ばれていません。デストラクタで自動クリーンアップします。"); }
        Shutdown();
    }

    /**
     * @brief リソースの明示的解放
     */
    void Shutdown() {
        if (isShutdown_) return;
        DEBUGLOG_CATEGORY(DebugLog::Category::Render, "RenderSystem::Shutdown() - リソースを解放中");

        int releasedCount = 0;

        if (vs_) {
            DEBUGLOG_CATEGORY(DebugLog::Category::Render, "頂点シェーダーを解放");
            vs_.Reset();
            releasedCount++;
        }

        if (ps_) {
            DEBUGLOG_CATEGORY(DebugLog::Category::Render, "ピクセルシェーダーを解放");
            ps_.Reset();
            releasedCount++;
        }

        if (layout_) {
            DEBUGLOG_CATEGORY(DebugLog::Category::Render, "入力レイアウトを解放");
            layout_.Reset();
            releasedCount++;
        }

        if (cb_) {
            DEBUGLOG_CATEGORY(DebugLog::Category::Render, "VS定数バッファを解放");
            cb_.Reset();
            releasedCount++;
        }

        if (psCb_) {
            DEBUGLOG_CATEGORY(DebugLog::Category::Render, "PS定数バッファを解放");
            psCb_.Reset();
            releasedCount++;
        }

        if (rasterState_) {
            DEBUGLOG_CATEGORY(DebugLog::Category::Render, "ラスタライザステートを解放");
            rasterState_.Reset();
            releasedCount++;
        }

        if (samplerState_) {
            DEBUGLOG_CATEGORY(DebugLog::Category::Render, "サンプラーステートを解放");
            samplerState_.Reset();
            releasedCount++;
        }

        for (int i = 0; i < 6; ++i) {
            if (meshes_[i].vb) {
                DEBUGLOG_CATEGORY(DebugLog::Category::Render, "メッシュ(vb)を解放");
                meshes_[i].vb.Reset();
                releasedCount++;
            }
            if (meshes_[i].ib) {
                DEBUGLOG_CATEGORY(DebugLog::Category::Render, "メッシュ(ib)を解放");
                meshes_[i].ib.Reset();
                releasedCount++;
            }
        }

        isShutdown_ = true;
        DEBUGLOG_CATEGORY(DebugLog::Category::Render, "RenderSystem::Shutdown() 完了 (解放リソース数: " + std::to_string(releasedCount) + ")");
    }

    /**
     * @brief 立方体メッシュの作成
     */
    bool CreateCubeMesh(GfxDevice& gfx) {
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

        D3D11_BUFFER_DESC vbd{};
        vbd.ByteWidth = (UINT)sizeof(verts);
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.Usage = D3D11_USAGE_IMMUTABLE;
        D3D11_SUBRESOURCE_DATA vinit{ verts, 0, 0 };
        if (FAILED(gfx.Dev()->CreateBuffer(&vbd, &vinit, meshes_[(int)MeshType::Cube].vb.GetAddressOf()))) {
            return false;
        }

        D3D11_BUFFER_DESC ibd{};
        ibd.ByteWidth = (UINT)sizeof(idx);
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.Usage = D3D11_USAGE_IMMUTABLE;
        D3D11_SUBRESOURCE_DATA iinit{ idx, 0, 0 };
        if (FAILED(gfx.Dev()->CreateBuffer(&ibd, &iinit, meshes_[(int)MeshType::Cube].ib.GetAddressOf()))) {
            return false;
        }

        meshes_[(int)MeshType::Cube].indexCount = (UINT)(sizeof(idx) / sizeof(idx[0]));
        return true;
    }

    /**
     * @brief 球体メッシュの作成
     * @param[in] gfx グラフィックスデバイス
     * @param[in] segments 緯度・経度の分割数(デフォルト:16)
     */
    bool CreateSphereMesh(GfxDevice& gfx, int segments = 16) {
        struct V { DirectX::XMFLOAT3 pos; DirectX::XMFLOAT2 tex; };
        std::vector<V> verts;
        std::vector<uint16_t> idx;

        const float radius = 0.5f;

        // 頂点生成
        for (int lat = 0; lat <= segments; ++lat) {
            float theta = DirectX::XM_PI * (float)lat / (float)segments;
            float sinTheta = sinf(theta);
            float cosTheta = cosf(theta);

            for (int lon = 0; lon <= segments; ++lon) {
                float phi = 2.0f * DirectX::XM_PI * (float)lon / (float)segments;
                float sinPhi = sinf(phi);
                float cosPhi = cosf(phi);

                V v;
                v.pos.x = radius * sinTheta * cosPhi;
                v.pos.y = radius * cosTheta;
                v.pos.z = radius * sinTheta * sinPhi;
                v.tex.x = (float)lon / (float)segments;
                v.tex.y = (float)lat / (float)segments;
                verts.push_back(v);
            }
        }

        // インデックス生成
        for (int lat = 0; lat < segments; ++lat) {
            for (int lon = 0; lon < segments; ++lon) {
                int first = lat * (segments + 1) + lon;
                int second = first + segments + 1;

                idx.push_back((uint16_t)first);
                idx.push_back((uint16_t)second);
                idx.push_back((uint16_t)(first + 1));

                idx.push_back((uint16_t)second);
                idx.push_back((uint16_t)(second + 1));
                idx.push_back((uint16_t)(first + 1));
            }
        }

        D3D11_BUFFER_DESC vbd{};
        vbd.ByteWidth = (UINT)(verts.size() * sizeof(V));
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.Usage = D3D11_USAGE_IMMUTABLE;
        D3D11_SUBRESOURCE_DATA vinit{ verts.data(), 0, 0 };
        if (FAILED(gfx.Dev()->CreateBuffer(&vbd, &vinit, meshes_[(int)MeshType::Sphere].vb.GetAddressOf()))) {
            return false;
        }

        D3D11_BUFFER_DESC ibd{};
        ibd.ByteWidth = (UINT)(idx.size() * sizeof(uint16_t));
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.Usage = D3D11_USAGE_IMMUTABLE;
        D3D11_SUBRESOURCE_DATA iinit{ idx.data(), 0, 0 };
        if (FAILED(gfx.Dev()->CreateBuffer(&ibd, &iinit, meshes_[(int)MeshType::Sphere].ib.GetAddressOf()))) {
            return false;
        }

        meshes_[(int)MeshType::Sphere].indexCount = (UINT)idx.size();
        return true;
    }

    /**
     * @brief 円柱メッシュの作成
     * @param[in] gfx グラフィックスデバイス
     * @param[in] segments 円周の分割数(デフォルト:16)
     */
    bool CreateCylinderMesh(GfxDevice& gfx, int segments = 16) {
        struct V { DirectX::XMFLOAT3 pos; DirectX::XMFLOAT2 tex; };
        std::vector<V> verts;
        std::vector<uint16_t> idx;

        const float radius = 0.5f;
        const float height = 1.0f;
        const float halfHeight = height * 0.5f;

        // 側面の頂点
        for (int i = 0; i <= segments; ++i) {
            float angle = 2.0f * DirectX::XM_PI * (float)i / (float)segments;
            float x = radius * cosf(angle);
            float z = radius * sinf(angle);
            float u = (float)i / (float)segments;

            // 上部
            verts.push_back({ {x, halfHeight, z}, {u, 0.0f} });
            // 下部
            verts.push_back({ {x, -halfHeight, z}, {u, 1.0f} });
        }

        // 側面のインデックス
        for (int i = 0; i < segments; ++i) {
            int base = i * 2;
            idx.push_back((uint16_t)base);
            idx.push_back((uint16_t)(base + 1));
            idx.push_back((uint16_t)(base + 2));

            idx.push_back((uint16_t)(base + 1));
            idx.push_back((uint16_t)(base + 3));
            idx.push_back((uint16_t)(base + 2));
        }

        // 上面・下面の中心点
        int topCenterIdx = (int)verts.size();
        verts.push_back({ {0, halfHeight, 0}, {0.5f, 0.5f} });
        int bottomCenterIdx = (int)verts.size();
        verts.push_back({ {0, -halfHeight, 0}, {0.5f, 0.5f} });

        // 上面・下面の周回頂点の開始位置（top, bottom を交互に追加していく）
        int ringStart = (int)verts.size();

        // 上面・下面の頂点とインデックス
        for (int i = 0; i < segments; ++i) {
            float angle = 2.0f * DirectX::XM_PI * (float)i / (float)segments;
            float x = radius * cosf(angle);
            float z = radius * sinf(angle);
            float u = 0.5f + 0.5f * cosf(angle);
            float v = 0.5f + 0.5f * sinf(angle);

            int topIdx = (int)verts.size();
            verts.push_back({ {x, halfHeight, z}, {u, v} });
            int bottomIdx = (int)verts.size();
            verts.push_back({ {x, -halfHeight, z}, {u, v} });

            // 次の頂点（ラップ考慮）
            int nextTop = (i == segments - 1) ? ringStart : topIdx + 2;
            int nextBottom = (i == segments - 1) ? ringStart + 1 : bottomIdx + 2;

            // 上面（CW）
            idx.push_back((uint16_t)topCenterIdx);
            idx.push_back((uint16_t)topIdx);
            idx.push_back((uint16_t)nextTop);

            // 下面（CW）
            idx.push_back((uint16_t)bottomCenterIdx);
            idx.push_back((uint16_t)nextBottom);
            idx.push_back((uint16_t)bottomIdx);
        }

        D3D11_BUFFER_DESC vbd{};
        vbd.ByteWidth = (UINT)(verts.size() * sizeof(V));
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.Usage = D3D11_USAGE_IMMUTABLE;
        D3D11_SUBRESOURCE_DATA vinit{ verts.data(), 0, 0 };
        if (FAILED(gfx.Dev()->CreateBuffer(&vbd, &vinit, meshes_[(int)MeshType::Cylinder].vb.GetAddressOf()))) {
            return false;
        }

        D3D11_BUFFER_DESC ibd{};
        ibd.ByteWidth = (UINT)(idx.size() * sizeof(uint16_t));
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.Usage = D3D11_USAGE_IMMUTABLE;
        D3D11_SUBRESOURCE_DATA iinit{ idx.data(), 0, 0 };
        if (FAILED(gfx.Dev()->CreateBuffer(&ibd, &iinit, meshes_[(int)MeshType::Cylinder].ib.GetAddressOf()))) {
            return false;
        }

        meshes_[(int)MeshType::Cylinder].indexCount = (UINT)idx.size();
        return true;
    }

    /**
     * @brief 円錐メッシュの作成
     * @param[in] gfx グラフィックスデバイス
     * @param[in] segments 円周の分割数(デフォルト:16)
     */
    bool CreateConeMesh(GfxDevice& gfx, int segments = 16) {
        struct V { DirectX::XMFLOAT3 pos; DirectX::XMFLOAT2 tex; };
        std::vector<V> verts;
        std::vector<uint16_t> idx;

        const float radius = 0.5f;
        const float height = 1.0f;
        const float halfHeight = height * 0.5f;

        // 頂点(先端)
        int apexIdx = (int)verts.size();
        verts.push_back({ {0, halfHeight, 0}, {0.5f, 0.0f} });

        // 底面の中心
        int baseCenterIdx = (int)verts.size();
        verts.push_back({ {0, -halfHeight, 0}, {0.5f, 0.5f} });

        // 側面のリング（segments+1 で閉ループ）
        for (int i = 0; i <= segments; ++i) {
            float angle = 2.0f * DirectX::XM_PI * (float)i / (float)segments;
            float x = radius * cosf(angle);
            float z = radius * sinf(angle);
            float u = (float)i / (float)segments;

            // 側面用
            verts.push_back({ {x, -halfHeight, z}, {u, 1.0f} });
        }

        // 側面のインデックス
        for (int i = 0; i < segments; ++i) {
            idx.push_back((uint16_t)apexIdx);
            idx.push_back((uint16_t)(2 + i));
            idx.push_back((uint16_t)(2 + i + 1));
        }

        // 底面のリング開始位置
        int baseStart = (int)verts.size();
        // 底面の頂点（ディスクUV）
        for (int i = 0; i < segments; ++i) {
            float angle = 2.0f * DirectX::XM_PI * (float)i / (float)segments;
            float x = radius * cosf(angle);
            float z = radius * sinf(angle);
            float u = 0.5f + 0.5f * cosf(angle);
            float v = 0.5f + 0.5f * sinf(angle);

            verts.push_back({ {x, -halfHeight, z}, {u, v} });
        }
        // 底面のインデックス（CW）
        for (int i = 0; i < segments; ++i) {
            int curr = baseStart + i;
            int next = (i == segments - 1) ? baseStart : baseStart + i + 1;
            idx.push_back((uint16_t)baseCenterIdx);
            idx.push_back((uint16_t)next);
            idx.push_back((uint16_t)curr);
        }

        D3D11_BUFFER_DESC vbd{};
        vbd.ByteWidth = (UINT)(verts.size() * sizeof(V));
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.Usage = D3D11_USAGE_IMMUTABLE;
        D3D11_SUBRESOURCE_DATA vinit{ verts.data(), 0, 0 };
        if (FAILED(gfx.Dev()->CreateBuffer(&vbd, &vinit, meshes_[(int)MeshType::Cone].vb.GetAddressOf()))) {
            return false;
        }

        D3D11_BUFFER_DESC ibd{};
        ibd.ByteWidth = (UINT)(idx.size() * sizeof(uint16_t));
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.Usage = D3D11_USAGE_IMMUTABLE;
        D3D11_SUBRESOURCE_DATA iinit{ idx.data(), 0, 0 };
        if (FAILED(gfx.Dev()->CreateBuffer(&ibd, &iinit, meshes_[(int)MeshType::Cone].ib.GetAddressOf()))) {
            return false;
        }

        meshes_[(int)MeshType::Cone].indexCount = (UINT)idx.size();
        return true;
    }

    /**
     * @brief 平面メッシュの作成
     */
    bool CreatePlaneMesh(GfxDevice& gfx) {
        struct V { DirectX::XMFLOAT3 pos; DirectX::XMFLOAT2 tex; };
        const float c = 0.5f;
        V verts[] = {
            {{-c, 0, -c}, {0, 1}},
            {{-c, 0, +c}, {0, 0}},
            {{+c, 0, +c}, {1, 0}},
            {{+c, 0, -c}, {1, 1}}
        };
        uint16_t idx[] = { 0, 1, 2, 0, 2, 3 };

        D3D11_BUFFER_DESC vbd{};
        vbd.ByteWidth = (UINT)sizeof(verts);
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.Usage = D3D11_USAGE_IMMUTABLE;
        D3D11_SUBRESOURCE_DATA vinit{ verts, 0, 0 };
        if (FAILED(gfx.Dev()->CreateBuffer(&vbd, &vinit, meshes_[(int)MeshType::Plane].vb.GetAddressOf()))) {
            return false;
        }

        D3D11_BUFFER_DESC ibd{};
        ibd.ByteWidth = (UINT)sizeof(idx);
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.Usage = D3D11_USAGE_IMMUTABLE;
        D3D11_SUBRESOURCE_DATA iinit{ idx, 0, 0 };
        if (FAILED(gfx.Dev()->CreateBuffer(&ibd, &iinit, meshes_[(int)MeshType::Plane].ib.GetAddressOf()))) {
            return false;
        }

        meshes_[(int)MeshType::Plane].indexCount = (UINT)(sizeof(idx) / sizeof(idx[0]));
        return true;
    }

    /**
     * @brief カプセルメッシュの作成
     * @param[in] gfx グラフィックスデバイス
     * @param[in] segments 分割数(デフォルト:16)
     */
    bool CreateCapsuleMesh(GfxDevice& gfx, int segments = 16) {
        struct V { DirectX::XMFLOAT3 pos; DirectX::XMFLOAT2 tex; };
        std::vector<V> verts;
        std::vector<uint16_t> idx;

        // 安全策：偶数を強制（奇数だと上下の分割で段差が出る）
        if (segments < 4) segments = 4;
        if (segments & 1) segments += 1; // 偶数化

        const float radius = 0.5f;
        const float cylinderHeight = 0.5f; // 必要に応じて調整
        const float halfCylinderHeight = cylinderHeight * 0.5f;

        const int hemiLat = segments / 2; // 半球の緯度分割

        // 上半球（yは +halfCylinderHeight 偏移）
        for (int lat = 0; lat <= hemiLat; ++lat) {
            float theta = (DirectX::XM_PI * 0.5f) * (float)lat / (float)hemiLat; // 0 -> π/2
            float sT = sinf(theta), cT = cosf(theta);
            for (int lon = 0; lon <= segments; ++lon) {
                float phi = 2.0f * DirectX::XM_PI * (float)lon / (float)segments;
                float sP = sinf(phi), cP = cosf(phi);
                V v;
                v.pos = { radius * sT * cP, halfCylinderHeight + radius * cT, radius * sT * sP };
                v.tex = { (float)lon / (float)segments, (float)lat / (float)hemiLat * 0.5f };
                verts.push_back(v);
            }
        }

        // 下半球（yは -halfCylinderHeight 偏移）
        const int lowerStart = (int)verts.size();
        for (int lat = 0; lat <= hemiLat; ++lat) {
            float theta = (DirectX::XM_PI * 0.5f) * (float)lat / (float)hemiLat; // 0 -> π/2（下→赤道）
            float sT = sinf(theta), cT = cosf(theta);
            for (int lon = 0; lon <= segments; ++lon) {
                float phi = 2.0f * DirectX::XM_PI * (float)lon / (float)segments;
                float sP = sinf(phi), cP = cosf(phi);
                V v;
                v.pos = { radius * sT * cP, -halfCylinderHeight - radius * cT, radius * sT * sP };
                v.tex = { (float)lon / (float)segments, 0.5f + (float)lat / (float)hemiLat * 0.5f };
                verts.push_back(v);
            }
        }

        // 上半球インデックス（CW）
        for (int lat = 0; lat < hemiLat; ++lat) {
            for (int lon = 0; lon < segments; ++lon) {
                int first = lat * (segments + 1) + lon;
                int second = (lat + 1) * (segments + 1) + lon;

                // CW: first, second, first+1 / second, second+1, first+1
                idx.push_back((uint16_t)first);
                idx.push_back((uint16_t)second);
                idx.push_back((uint16_t)(first + 1));

                idx.push_back((uint16_t)second);
                idx.push_back((uint16_t)(second + 1));
                idx.push_back((uint16_t)(first + 1));
            }
        }

        // 下半球インデックス
        for (int lat = 0; lat < hemiLat; ++lat) {
            for (int lon = 0; lon < segments; ++lon) {
                int first = lowerStart + lat * (segments + 1) + lon;
                int second = lowerStart + (lat + 1) * (segments + 1) + lon;

                // CWに統一（上半球の並びとは**逆**にするのがポイント）
                // CW: first, first+1, second / first+1, second+1, second
                idx.push_back((uint16_t)first);
                idx.push_back((uint16_t)(first + 1));
                idx.push_back((uint16_t)second);

                idx.push_back((uint16_t)(first + 1));
                idx.push_back((uint16_t)(second + 1));
                idx.push_back((uint16_t)second);
            }
        }

        // 円柱部（赤道リング同士を接続）
        int upperEq = hemiLat * (segments + 1);                 // 上半球の最終行（赤道）
        int lowerEq = lowerStart + hemiLat * (segments + 1);    // 下半球の最終行（赤道）
        for (int lon = 0; lon < segments; ++lon) {
            int a = upperEq + lon;       // 上 今
            int b = upperEq + lon + 1;   // 上 次
            int c = lowerEq + lon;       // 下 今
            int d = lowerEq + lon + 1;   // 下 次

            // 上三角: a, c, b（CW） / 下三角: b, c, d（CW）
            idx.push_back((uint16_t)a);
            idx.push_back((uint16_t)c);
            idx.push_back((uint16_t)b);

            idx.push_back((uint16_t)b);
            idx.push_back((uint16_t)c);
            idx.push_back((uint16_t)d);
        }

        // バッファ作成はそのまま
        D3D11_BUFFER_DESC vbd{};
        vbd.ByteWidth = (UINT)(verts.size() * sizeof(V));
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.Usage = D3D11_USAGE_IMMUTABLE;
        D3D11_SUBRESOURCE_DATA vinit{ verts.data(), 0, 0 };
        if (FAILED(gfx.Dev()->CreateBuffer(&vbd, &vinit, meshes_[(int)MeshType::Capsule].vb.GetAddressOf())))
            return false;

        D3D11_BUFFER_DESC ibd{};
        ibd.ByteWidth = (UINT)(idx.size() * sizeof(uint16_t));
        ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        ibd.Usage = D3D11_USAGE_IMMUTABLE;
        D3D11_SUBRESOURCE_DATA iinit{ idx.data(), 0, 0 };
        if (FAILED(gfx.Dev()->CreateBuffer(&ibd, &iinit, meshes_[(int)MeshType::Capsule].ib.GetAddressOf())))
            return false;

        meshes_[(int)MeshType::Capsule].indexCount = (UINT)idx.size();
        return true;
    }

    /**
     * @brief 初期化
     * @param[in] gfx グラフィックスデバイス
     * @param[in] texMgr テクスチャマネージャー
     * @return bool 初期化が成功した場合は true
     *
     * @details
     * シェーダーのコンパイル、パイプラインステートの作成、
     * 全ての基本形状メッシュの作成を行います。
     */
    bool Init(GfxDevice& gfx, TextureManager& texMgr) {
        texManager_ = &texMgr;
        isShutdown_ = false;

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

        // 各形状のメッシュを作成
        if (!CreateCubeMesh(gfx)) {
            DEBUGLOG_ERROR("Failed to create Cube mesh");
            return false;
        }
        if (!CreateSphereMesh(gfx)) {
            DEBUGLOG_ERROR("Failed to create Sphere mesh");
            return false;
        }
        if (!CreateCylinderMesh(gfx)) {
            DEBUGLOG_ERROR("Failed to create Cylinder mesh");
            return false;
        }
        if (!CreateConeMesh(gfx)) {
            DEBUGLOG_ERROR("Failed to create Cone mesh");
            return false;
        }
        if (!CreatePlaneMesh(gfx)) {
            DEBUGLOG_ERROR("Failed to create Plane mesh");
            return false;
        }
        if (!CreateCapsuleMesh(gfx)) {
            DEBUGLOG_ERROR("Failed to create Capsule mesh");
            return false;
        }

        DEBUGLOG("RenderSystem initialized with all mesh types");
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
     * 各エンティティの形状タイプに応じて適切なメッシュを使用します。
     */
    void Render(GfxDevice& gfx, World& w, const Camera& cam) {
        gfx.Ctx()->IASetInputLayout(layout_.Get());
        gfx.Ctx()->VSSetShader(vs_.Get(), nullptr, 0);
        gfx.Ctx()->PSSetShader(ps_.Get(), nullptr, 0);
        gfx.Ctx()->VSSetConstantBuffers(0, 1, cb_.GetAddressOf());
        gfx.Ctx()->PSSetConstantBuffers(0, 1, psCb_.GetAddressOf());
        gfx.Ctx()->PSSetSamplers(0, 1, samplerState_.GetAddressOf());
        gfx.Ctx()->RSSetState(rasterState_.Get());
        gfx.Ctx()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        w.ForEach<MeshRenderer>([&](Entity e, MeshRenderer& mr) {
            auto* t = w.TryGet<Transform>(e);
            if (!t) return;

            // 形状タイプを取得
            int meshIndex = (int)mr.meshType;
            if (meshIndex < 0 || meshIndex >= 6) {
                meshIndex = 0;  // デフォルトはCube
            }

            const MeshData& mesh = meshes_[meshIndex];
            if (!mesh.vb || !mesh.ib) return;

            // メッシュを設定
            UINT stride = sizeof(DirectX::XMFLOAT3) + sizeof(DirectX::XMFLOAT2);
            UINT offset = 0;
            gfx.Ctx()->IASetVertexBuffers(0, 1, mesh.vb.GetAddressOf(), &stride, &offset);
            gfx.Ctx()->IASetIndexBuffer(mesh.ib.Get(), DXGI_FORMAT_R16_UINT, 0);

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

            gfx.Ctx()->DrawIndexed(mesh.indexCount, 0, 0);
        });
    }

    bool isShutdown_ = false; ///< シャットダウン済みフラグ
};
