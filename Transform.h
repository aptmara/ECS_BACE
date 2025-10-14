#pragma once
#include <DirectXMath.h>

// ========================================================
// Transform - 位置、回転、スケールコンポーネント
// ========================================================
struct Transform {
    DirectX::XMFLOAT3 position{ 0, 0, 5 }; // z=+5 (カメラ前)
    DirectX::XMFLOAT3 rotation{ 0, 0, 0 }; // degrees
    DirectX::XMFLOAT3 scale{ 1, 1, 1 };
};
