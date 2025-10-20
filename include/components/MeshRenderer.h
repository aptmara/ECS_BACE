#pragma once
#include <DirectXMath.h>
#include "graphics/TextureManager.h"

/**
 * @file MeshRenderer.h
 * @brief メッシュ描画コンポーネントの定義
 * @author 山内陽
 * @date 2025
 * @version 5.0
 * 
 * @details
 * このファイルは3Dオブジェクトの「見た目」を制御するコンポーネントを定義します。
 */

/**
 * @struct MeshRenderer
 * @brief オブジェクトの見た目（色・テクスチャ）を管理するデータコンポーネント
 * 
 * @details
 * このコンポーネントは3Dオブジェクトの描画設定を保持します。
 * 単色表示とテクスチャ表示の両方に対応しています。
 * 
 * ### 描画の仕組み:
 * 1. テクスチャが設定されていない場合: colorで指定した単色で描画
 * 2. テクスチャが設定されている場合: テクスチャ画像で描画（colorは色調として使用）
 * 
 * ### UV座標について:
 * UV座標は、テクスチャのどの部分を表示するかを指定します。
 * - uvOffset: テクスチャの開始位置をずらす（アニメーション等に使用）
 * - uvScale: テクスチャの繰り返し回数（タイリング）
 * 
 * @par 使用例（単色）:
 * @code
 * Entity cube = world.Create()
 *     .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
 *     .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})  // 赤色
 *     .Build();
 * @endcode
 * 
 * @par 使用例（テクスチャ）:
 * @code
 * MeshRenderer renderer;
 * renderer.color = DirectX::XMFLOAT3{1, 1, 1};  // 白（テクスチャ本来の色）
 * renderer.texture = texManager.LoadFromFile("brick.png");
 * 
 * Entity cube = world.Create()
 *     .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
 *     .With<MeshRenderer>(renderer)
 *     .Build();
 * @endcode
 * 
 * @par 使用例（UVアニメーション）:
 * @code
 * // 毎フレーム、テクスチャを横にスクロール
 * auto* renderer = world.TryGet<MeshRenderer>(entity);
 * if (renderer) {
 *     renderer->uvOffset.x += 0.01f * dt;
 * }
 * @endcode
 * 
 * @note Transformコンポーネントと組み合わせて使用します
 * @see Transform 位置・回転・スケールを管理するコンポーネント
 * @see TextureManager テクスチャ管理クラス
 * 
 * @author 山内陽
 */
struct MeshRenderer {
    /**
     * @var color
     * @brief オブジェクトの基本色（RGB: 0.0～1.0）
     * 
     * @details
     * - テクスチャなし: この色で描画
     * - テクスチャあり: テクスチャにこの色を乗算（色調補正）
     * 
     * 色の指定方法:
     * - color.x = R（赤）: 0.0～1.0
     * - color.y = G（緑）: 0.0～1.0
     * - color.z = B（青）: 0.0～1.0
     * 
     * @par よく使う色:
     * @code
     * DirectX::XMFLOAT3{1, 0, 0}  // 赤
     * DirectX::XMFLOAT3{0, 1, 0}  // 緑
     * DirectX::XMFLOAT3{0, 0, 1}  // 青
     * DirectX::XMFLOAT3{1, 1, 0}  // 黄
     * DirectX::XMFLOAT3{1, 0, 1}  // マゼンタ
     * DirectX::XMFLOAT3{0, 1, 1}  // シアン
     * DirectX::XMFLOAT3{1, 1, 1}  // 白
     * DirectX::XMFLOAT3{0, 0, 0}  // 黒
     * @endcode
     */
    DirectX::XMFLOAT3 color{ 0.3f, 0.7f, 1.0f };
    
    /**
     * @var texture
     * @brief 表面に貼り付けるテクスチャ画像のハンドル
     * 
     * @details
     * TextureManagerから取得したテクスチャハンドルを設定します。
     * INVALID_TEXTUREの場合、テクスチャは使用されず単色で描画されます。
     * 
     * @note デフォルトはINVALID_TEXTURE（テクスチャなし）
     * 
     * @par 使用例:
     * @code
     * auto* renderer = world.TryGet<MeshRenderer>(entity);
     * if (renderer) {
     *     renderer->texture = texManager.LoadFromFile("brick.png");
     * }
     * @endcode
     * 
     * @see TextureManager::LoadFromFile テクスチャ読み込み
     */
    TextureManager::TextureHandle texture = TextureManager::INVALID_TEXTURE;
    
    /**
     * @var uvOffset
     * @brief UV座標のオフセット（テクスチャ位置のずらし）
     * 
     * @details
     * テクスチャの表示開始位置をずらします。
     * アニメーションやスクロール効果に使用します。
     * 
     * - uvOffset.x: 横方向のオフセット（0.0～1.0で1周）
     * - uvOffset.y: 縦方向のオフセット（0.0～1.0で1周）
     * 
     * @par 使用例（横スクロール）:
     * @code
     * // 毎フレーム少しずつ横にずらす
     * renderer->uvOffset.x += 0.5f * dt;  // 毎秒0.5ずつスクロール
     * @endcode
     */
    DirectX::XMFLOAT2 uvOffset{ 0.0f, 0.0f };
    
    /**
     * @var uvScale
     * @brief UV座標のスケール（テクスチャの繰り返し）
     * 
     * @details
     * テクスチャの繰り返し回数を指定します（タイリング）。
     * 
     * - uvScale.x: 横方向の繰り返し回数
     * - uvScale.y: 縦方向の繰り返し回数
     * 
     * @par 使用例（2x2タイリング）:
     * @code
     * renderer->uvScale = DirectX::XMFLOAT2{2.0f, 2.0f};  // 2x2で繰り返し
     * @endcode
     * 
     * @note 1.0が等倍（繰り返しなし）
     */
    DirectX::XMFLOAT2 uvScale{ 1.0f, 1.0f };
};
