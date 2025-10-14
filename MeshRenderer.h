#pragma once
#include <DirectXMath.h>
#include "TextureManager.h"

// ========================================================
// MeshRenderer - メッシュレンダリングコンポーネント
// ========================================================
// 【役割】
// オブジェクトの「見た目」を定義するデータコンポーネント
//
// 【メンバ変数】
// - color: 基本色（R, G, B: 0.0～1.0）
// - texture: 表面に貼るテクスチャ画像
// - uvOffset: テクスチャの位置ずらし（アニメーション用）
// - uvScale: テクスチャの拡大縮小
//
// 【使い方】
// MeshRenderer mr;
// mr.color = DirectX::XMFLOAT3{1, 0, 0};  // 赤色
// mr.texture = texManager.LoadFromFile("brick.png"); // レンガのテクスチャ
// ========================================================
struct MeshRenderer {
    // カラー（テクスチャがない場合に使用、または色付け）
    DirectX::XMFLOAT3 color{ 0.3f, 0.7f, 1.0f }; // デフォルト: 水色
    
    // テクスチャ（画像を貼り付ける）
    TextureManager::TextureHandle texture = TextureManager::INVALID_TEXTURE;
    
    // UV座標のオフセット（テクスチャをずらす: UVアニメーション用）
    DirectX::XMFLOAT2 uvOffset{ 0.0f, 0.0f };
    
    // UV座標のスケール（テクスチャを拡大縮小）
    DirectX::XMFLOAT2 uvScale{ 1.0f, 1.0f };
};
