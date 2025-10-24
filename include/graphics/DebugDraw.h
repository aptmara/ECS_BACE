/**
 * @file DebugDraw.h
 * @brief デバッグ用の線描画システム
 * @author 山内陽
 * @date 2025
 * @version 5.0
 */
#pragma once
#include "graphics/GfxDevice.h"
#include "graphics/Camera.h"
#include "app/DebugLog.h" // デバッグビルド/リリースビルド両方で必要
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl/client.h>
#include <vector>
#include <cstring>
#include <cstdio>
#include <string> // std::to_string のために追加

#pragma comment(lib, "d3dcompiler.lib")

/**
 * @class DebugDraw
 * @brief デバッグ用の線描画システム
 *
 * @details
 * 開発中にグリッド、座標軸、任意の線を描画するためのクラスです。
 * ワールド空間でのデバッグ情報の可視化に使用します。
 *
 * ### 主な用途:
 * - グリッド表示(基準となる平面)
 * - 座標軸表示(X, Y, Z軸)
 * - 当たり判定の可視化
 * - 移動経路の表示
 *
 * @par 使用例
 * @code
 * DebugDraw debugDraw;
 * debugDraw.Init(gfx);
 *
 * // グリッドと軸を描画
 * debugDraw.DrawGrid(20.0f, 20);
 * debugDraw.DrawAxes(5.0f);
 *
 * // カスタム線を描画
 * debugDraw.AddLine(
 *     DirectX::XMFLOAT3{0, 0, 0},
 *     DirectX::XMFLOAT3{5, 5, 5},
 *     DirectX::XMFLOAT3{1, 1, 0}  // 黄色
 * );
 *
 * // 描画実行
 * debugDraw.Render(gfx, camera);
 *
 * // フレーム終了時にクリア
 * debugDraw.Clear();
 * @endcode
 *
 * @note デバッグビルド(_DEBUG定義時)のみ使用を推奨
 *
 * @author 山内陽
 */
class DebugDraw {
public:
    /**
     * @struct Line
     * @brief 線分の定義(開始点、終了点、色)
     */
    struct Line {
        DirectX::XMFLOAT3 start; ///< 線の開始点
        DirectX::XMFLOAT3 end;   ///< 線の終了点
        DirectX::XMFLOAT3 color; ///< 線の色(RGB: 0.0～1.0)
    };

    /**
     * @brief 初期化
     * @param[in] gfx グラフィックスデバイス
     * @return bool 初期化が成功した場合は true
     *
     * @details
     * シェーダーのコンパイル、パイプラインステートの作成、
     * 動的頂点バッファの作成を行います。
     */
    bool Init(GfxDevice& gfx) {
        DEBUGLOG("DebugDraw::Init() 開始");
        
        // すでに初期化済みの再初期化に対応
        if (initialized_) {
            Shutdown();
        }

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
                DEBUGLOG("[ERROR] 頂点シェーダーのコンパイル失敗: " + std::string((char*)err->GetBufferPointer()));
                MessageBoxA(nullptr, msg, "Shader Error", MB_OK | MB_ICONERROR);
            } else {
                DEBUGLOG("[ERROR] 頂点シェーダーのコンパイル失敗 HRESULT: 0x" + std::to_string(hr));
            }
            return false;
        }
        DEBUGLOG("頂点シェーダーのコンパイル成功");

        hr = D3DCompile(PS, strlen(PS), nullptr, nullptr, nullptr, "main", "ps_5_0", 0, 0, psb.GetAddressOf(), err.ReleaseAndGetAddressOf());
        if (FAILED(hr)) {
            if (err) {
                char msg[512];
                sprintf_s(msg, "DebugDraw PS compile error:\n%s", (char*)err->GetBufferPointer());
                DEBUGLOG("[ERROR] ピクセルシェーダーのコンパイル失敗: " + std::string((char*)err->GetBufferPointer()));
                MessageBoxA(nullptr, msg, "Shader Error", MB_OK | MB_ICONERROR);
            } else {
                DEBUGLOG("[ERROR] ピクセルシェーダーのコンパイル失敗 HRESULT: 0x" + std::to_string(hr));
            }
            return false;
        }
        DEBUGLOG("ピクセルシェーダーのコンパイル成功");

        if (FAILED(gfx.Dev()->CreateVertexShader(vsb->GetBufferPointer(), vsb->GetBufferSize(), nullptr, vs_.GetAddressOf()))) {
            DEBUGLOG("[ERROR] 頂点シェーダーの作成失敗");
            MessageBoxA(nullptr, "Failed to create debug vertex shader", "Shader Error", MB_OK | MB_ICONERROR);
            return false;
        }
        DEBUGLOG("頂点シェーダーを作成");

        if (FAILED(gfx.Dev()->CreatePixelShader(psb->GetBufferPointer(), psb->GetBufferSize(), nullptr, ps_.GetAddressOf()))) {
            DEBUGLOG("[ERROR] ピクセルシェーダーの作成失敗");
            MessageBoxA(nullptr, "Failed to create debug pixel shader", "Shader Error", MB_OK | MB_ICONERROR);
            return false;
        }
        DEBUGLOG("ピクセルシェーダーを作成");

        // 入力レイアウト
        D3D11_INPUT_ELEMENT_DESC il[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
        };
        if (FAILED(gfx.Dev()->CreateInputLayout(il, 2, vsb->GetBufferPointer(), vsb->GetBufferSize(), layout_.GetAddressOf()))) {
            DEBUGLOG("[ERROR] 入力レイアウトの作成失敗");
            MessageBoxA(nullptr, "Failed to create debug input layout", "Shader Error", MB_OK | MB_ICONERROR);
            return false;
        }
        DEBUGLOG("入力レイアウトを作成");

        // 定数バッファ
        D3D11_BUFFER_DESC cbd{};
        cbd.ByteWidth = sizeof(DirectX::XMMATRIX);
        cbd.Usage = D3D11_USAGE_DEFAULT;
        cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        if (FAILED(gfx.Dev()->CreateBuffer(&cbd, nullptr, cb_.GetAddressOf()))) {
            DEBUGLOG("[ERROR] 定数バッファの作成失敗");
            MessageBoxA(nullptr, "Failed to create debug constant buffer", "Buffer Error", MB_OK | MB_ICONERROR);
            return false;
        }
        DEBUGLOG("定数バッファを作成");

        // 動的頂点バッファ(最大10000線分)
        maxLines_ = 10000;
        D3D11_BUFFER_DESC vbd{};
        vbd.ByteWidth = (UINT)(maxLines_ * 2 * sizeof(Vertex)); // 1線分 = 2頂点
        vbd.Usage = D3D11_USAGE_DYNAMIC;
        vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        if (FAILED(gfx.Dev()->CreateBuffer(&vbd, nullptr, vb_.GetAddressOf()))) {
            DEBUGLOG("[ERROR] 頂点バッファの作成失敗");
            MessageBoxA(nullptr, "Failed to create debug vertex buffer", "Buffer Error", MB_OK | MB_ICONERROR);
            return false;
        }
        DEBUGLOG("頂点バッファを作成 (最大線数: " + std::to_string(maxLines_) + ")");

        initialized_ = true;
        DEBUGLOG("DebugDraw::Init() 正常に完了");
        return true;
    }

    /**
     * @brief 線を追加
     * @param[in] start 線の開始点
     * @param[in] end 線の終了点
     * @param[in] color 線の色(RGB: 0.0～1.0)
     *
     * @details
     * 描画する線をリストに追加します。
     * 実際の描画はRender()呼び出し時に行われます。
     *
     * @par 使用例
     * @code
     * // 赤い線を描画
     * debugDraw.AddLine(
     *     DirectX::XMFLOAT3{0, 0, 0},
     *     DirectX::XMFLOAT3{10, 0, 0},
     *     DirectX::XMFLOAT3{1, 0, 0}
     * );
     * @endcode
     */
    void AddLine(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const DirectX::XMFLOAT3& color) {
        if (lines_.size() >= maxLines_) {
            DEBUGLOG("[WARNING] DebugDrawの線の上限に到達 (" + std::to_string(maxLines_) + ")、AddLine()を無視します");
            return;
        }
        lines_.push_back({ start, end, color });
    }

    /**
     * @brief グリッドを描画
     * @param[in] size グリッドのサイズ(全体の幅と奥行き)
     * @param[in] divisions 分割数(何本の線を引くか)
     * @param[in] color グリッドの色(デフォルト: 灰色)
     * @param[in] yOffset Y位置のオフセット(デフォルト: -0.01f、座標軸との重なりを防ぐ)
     *
     * @details
     * X-Z平面にグリッドを描画します。
     * Y=yOffsetの平面に水平なグリッドが表示されます。
     *
     * @par 使用例
     * @code
     * // 20x20のグリッドを20本の線で描画
     * debugDraw.DrawGrid(20.0f, 20);
     * @endcode
     */
    void DrawGrid(float size = 10.0f, int divisions = 10, const DirectX::XMFLOAT3& color = {0.5f, 0.5f, 0.5f}, float yOffset = -0.01f) {
        float step = size / divisions;
        float halfSize = size * 0.5f;

        // X-Z平面のグリッド（yOffsetを適用）
        for (int i = 0; i <= divisions; ++i) {
            float pos = -halfSize + i * step;

            // Z軸に平行な線(X方向に並ぶ)
            AddLine(
                DirectX::XMFLOAT3{-halfSize, yOffset, pos},
                DirectX::XMFLOAT3{ halfSize, yOffset, pos},
                color
            );

            // X軸に平行な線(Z方向に並ぶ)
            AddLine(
                DirectX::XMFLOAT3{pos, yOffset, -halfSize},
                DirectX::XMFLOAT3{pos, yOffset,  halfSize},
                color
            );
        }
    }

    /**
     * @brief 座標軸を描画
     * @param[in] length 軸の長さ
     *
     * @details
     * X軸(赤)、Y軸(緑)、Z軸(青)を原点から描画します。
     * 3D空間の方向を確認するのに便利です。
     * より見やすくするため、明るい色を使用しています。
     *
     * @par 使用例
     * @code
     * // 5単位の長さの座標軸を描画
     * debugDraw.DrawAxes(5.0f);
     * @endcode
     */
    void DrawAxes(float length = 500.0f) {
        // X軸(明るい赤)
        AddLine(
            DirectX::XMFLOAT3{0, 0, 0},
            DirectX::XMFLOAT3{length, 0, 0},
            DirectX::XMFLOAT3{1, 0.2f, 0.2f}
        );

        // Y軸(明るい緑)
        AddLine(
            DirectX::XMFLOAT3{0, 0, 0},
            DirectX::XMFLOAT3{0, length, 0},
            DirectX::XMFLOAT3{0.2f, 1, 0.2f}
        );

        // Z軸(明るい青)
        AddLine(
            DirectX::XMFLOAT3{0, 0, 0},
            DirectX::XMFLOAT3{0, 0, length},
            DirectX::XMFLOAT3{0.3f, 0.3f, 1}
        );
    }

    /**
     * @brief すべての線を描画
     * @param[in] gfx グラフィックスデバイス
     * @param[in] cam カメラ
     *
     * @details
     * AddLine()やDrawGrid()などで追加されたすべての線を描画します。
     * カメラのView・Projection行列を使用してワールド空間から画面空間に変換します。
     */
    void Render(GfxDevice& gfx, const Camera& cam) {
        if (lines_.empty()) return;

        // 頂点データを構築
        std::vector<Vertex> vertices;
        vertices.reserve(lines_.size() * 2);

        for (const auto& line : lines_) {
            vertices.push_back({ line.start, line.color });
            vertices.push_back({ line.end, line.color });
        }

        // 頂点バッファを更新
        D3D11_MAPPED_SUBRESOURCE mapped;
        HRESULT hr = gfx.Ctx()->Map(vb_.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
        if (FAILED(hr)) {
            DEBUGLOG("[ERROR] DebugDrawの頂点バッファのマップ失敗 (HRESULT: 0x" + std::to_string(hr) + ")");
            return;
        }
        
        memcpy(mapped.pData, vertices.data(), vertices.size() * sizeof(Vertex));
        gfx.Ctx()->Unmap(vb_.Get(), 0);

        // パイプライン設定
        gfx.Ctx()->IASetInputLayout(layout_.Get());
        gfx.Ctx()->VSSetShader(vs_.Get(), nullptr, 0);
        gfx.Ctx()->PSSetShader(ps_.Get(), nullptr, 0);
        gfx.Ctx()->VSSetConstantBuffers(0, 1, cb_.GetAddressOf());

        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        gfx.Ctx()->IASetVertexBuffers(0, 1, vb_.GetAddressOf(), &stride, &offset);
        gfx.Ctx()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

        // 定数バッファ更新(ワールド行列は単位行列)
        DirectX::XMMATRIX VP = DirectX::XMMatrixTranspose(cam.View * cam.Proj);
        gfx.Ctx()->UpdateSubresource(cb_.Get(), 0, nullptr, &VP, 0, 0);

        // 描画
        gfx.Ctx()->Draw((UINT)vertices.size(), 0);
    }

    /**
     * @brief フレーム終了時にクリア
     *
     * @details
     * 蓄積された線データをクリアします。
     * 毎フレーム呼び出す必要があります。
     *
     * @par 使用例
     * @code
     * while (running) {
     *     // 線を追加
     *     debugDraw.DrawGrid(20.0f, 20);
     *
     *     // 描画
     *     debugDraw.Render(gfx, camera);
     *
     *     // フレーム終了時にクリア
     *     debugDraw.Clear();
     * }
     * @endcode
     */
    void Clear() {
        lines_.clear();
    }

    /**
     * @brief デストラクタ
     *
     * @details
     * すべてのDirectX11リソースを自動的に解放します。
     */
    ~DebugDraw() {
        DEBUGLOG("DebugDraw::~DebugDraw() - デストラクタ呼び出し");
        if (!isShutdown_) {
            DEBUGLOG_WARNING("DebugDraw::Shutdown()が明示的に呼ばれていません。デストラクタで自動クリーンアップします。");
        }
        Shutdown();
    }

    /**
     * @brief リソースの明示的解放
     */
    void Shutdown() {
        if (isShutdown_) return; // 冪等性
        DEBUGLOG_CATEGORY(DebugLog::Category::Graphics, "DebugDraw::Shutdown() - リソースを解放中");
        
        int releasedCount = 0;
        
        if (vs_) {
            DEBUGLOG_CATEGORY(DebugLog::Category::Graphics, "頂点シェーダーを解放 (DebugDraw)");
            vs_.Reset();
            releasedCount++;
        }
        
        if (ps_) {
            DEBUGLOG_CATEGORY(DebugLog::Category::Graphics, "ピクセルシェーダーを解放 (DebugDraw)");
            ps_.Reset();
            releasedCount++;
        }
        
        if (layout_) {
            DEBUGLOG_CATEGORY(DebugLog::Category::Graphics, "入力レイアウトを解放 (DebugDraw)");
            layout_.Reset();
            releasedCount++;
        }
        
        if (cb_) {
            DEBUGLOG_CATEGORY(DebugLog::Category::Graphics, "定数バッファを解放 (DebugDraw)");
            cb_.Reset();
            releasedCount++;
        }
        
        if (vb_) {
            DEBUGLOG_CATEGORY(DebugLog::Category::Graphics, "頂点バッファを解放 (DebugDraw, 最大線数: " + std::to_string(maxLines_) + ")");
            vb_.Reset();
            releasedCount++;
        }
        
        isShutdown_ = true;
        DEBUGLOG_CATEGORY(DebugLog::Category::Graphics, "DebugDraw::Shutdown() 完了 (解放リソース数: " + std::to_string(releasedCount) + ")");
    }

private:
    /**
     * @struct Vertex
     * @brief 頂点データ(位置と色)
     */
    struct Vertex {
        DirectX::XMFLOAT3 pos; ///< 位置
        DirectX::XMFLOAT3 col; ///< 色
    };

    Microsoft::WRL::ComPtr<ID3D11VertexShader> vs_;    ///< 頂点シェーダー
    Microsoft::WRL::ComPtr<ID3D11PixelShader> ps_;     ///< ピクセルシェーダー
    Microsoft::WRL::ComPtr<ID3D11InputLayout> layout_; ///< 入力レイアウト
    Microsoft::WRL::ComPtr<ID3D11Buffer> cb_;          ///< 定数バッファ
    Microsoft::WRL::ComPtr<ID3D11Buffer> vb_;          ///< 頂点バッファ

    std::vector<Line> lines_;  ///< 描画する線のリスト
    size_t maxLines_;          ///< 最大線数
    bool isShutdown_ = false;  ///< シャットダウンフラグ
    bool initialized_ = false; ///< 初期化済みフラグ
};
