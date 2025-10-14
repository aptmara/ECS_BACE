#pragma once
#include <DirectXMath.h>

// ========================================================
// Transform - 位置・回転・スケールコンポーネント
// ========================================================
// 【役割】
// 3D空間での「場所」「向き」「大きさ」を表すデータコンポーネント
//
// 【メンバ変数】
// - position: どこにあるか（X, Y, Z座標）
// - rotation: どの方向を向いているか（度数法）
// - scale: どれくらいの大きさか（1.0 = 等倍）
//
// 【使い方】
// Transform t;
// t.position = DirectX::XMFLOAT3{0, 5, 0};  // Y軸方向に5上
// t.rotation = DirectX::XMFLOAT3{0, 45, 0}; // Y軸周りに45度回転
// t.scale = DirectX::XMFLOAT3{2, 2, 2};     // 2倍の大きさ
// ========================================================
struct Transform {
    DirectX::XMFLOAT3 position{ 0, 0, 5 }; // 位置（デフォルト: カメラ前方5m）
    DirectX::XMFLOAT3 rotation{ 0, 0, 0 }; // 回転（度数法: X=ピッチ, Y=ヨー, Z=ロール）
    DirectX::XMFLOAT3 scale{ 1, 1, 1 };    // スケール（1.0 = 等倍）
};
