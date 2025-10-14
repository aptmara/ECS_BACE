#pragma once
#include <DirectXMath.h>

// ========================================================
// Camera - カメラ（ビュー・プロジェクション行列）
// ========================================================
struct Camera {
    DirectX::XMMATRIX View;
    DirectX::XMMATRIX Proj;

    // LookAtLH カメラの作成
    static Camera LookAtLH(
        float fovY, float aspect, float znear, float zfar,
        DirectX::XMFLOAT3 eye, DirectX::XMFLOAT3 at, DirectX::XMFLOAT3 up)
    {
        Camera c;
        c.View = DirectX::XMMatrixLookAtLH(
            DirectX::XMLoadFloat3(&eye),
            DirectX::XMLoadFloat3(&at),
            DirectX::XMLoadFloat3(&up));
        c.Proj = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, znear, zfar);
        return c;
    }
};
