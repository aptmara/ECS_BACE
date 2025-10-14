#pragma once
#include <DirectXMath.h>

// ========================================================
// MeshRenderer - メッシュレンダリングコンポーネント
// ========================================================
struct MeshRenderer {
    // 本例は色キューブのみ（テクスチャ不要）
    DirectX::XMFLOAT3 color{ 0.3f, 0.7f, 1.0f };
};
