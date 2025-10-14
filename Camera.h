#pragma once
#include <DirectXMath.h>

// ========================================================
// Camera - カメラ（ビュー・プロジェクション行列）
// ========================================================
struct Camera {
    DirectX::XMMATRIX View;
    DirectX::XMMATRIX Proj;
    
    // カメラの位置と向き
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 target;
    DirectX::XMFLOAT3 up;
    
    // 投影パラメータ
    float fovY;
    float aspect;
    float nearZ;
    float farZ;

    // LookAtLH カメラの作成
    static Camera LookAtLH(
        float fovY, float aspect, float znear, float zfar,
        DirectX::XMFLOAT3 eye, DirectX::XMFLOAT3 at, DirectX::XMFLOAT3 upVec)
    {
        Camera c;
        c.position = eye;
        c.target = at;
        c.up = upVec;
        c.fovY = fovY;
        c.aspect = aspect;
        c.nearZ = znear;
        c.farZ = zfar;
        
        c.View = DirectX::XMMatrixLookAtLH(
            DirectX::XMLoadFloat3(&eye),
            DirectX::XMLoadFloat3(&at),
            DirectX::XMLoadFloat3(&upVec));
        c.Proj = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, znear, zfar);
        return c;
    }
    
    // カメラの更新（位置や注視点を変更した後に呼ぶ）
    void Update() {
        View = DirectX::XMMatrixLookAtLH(
            DirectX::XMLoadFloat3(&position),
            DirectX::XMLoadFloat3(&target),
            DirectX::XMLoadFloat3(&up));
    }
    
    // オービットカメラ（ターゲット周りを回転）
    void Orbit(float deltaYaw, float deltaPitch) {
        // 現在の位置からターゲットへのベクトル
        DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(&position);
        DirectX::XMVECTOR targetVec = DirectX::XMLoadFloat3(&target);
        DirectX::XMVECTOR toTarget = DirectX::XMVectorSubtract(targetVec, posVec);
        
        float radius = DirectX::XMVectorGetX(DirectX::XMVector3Length(toTarget));
        
        // 球面座標系で回転
        DirectX::XMMATRIX rotY = DirectX::XMMatrixRotationY(deltaYaw);
        DirectX::XMMATRIX rotX = DirectX::XMMatrixRotationAxis(
            DirectX::XMVector3Cross(toTarget, DirectX::XMLoadFloat3(&up)), 
            deltaPitch);
        
        DirectX::XMVECTOR newDir = DirectX::XMVector3TransformNormal(toTarget, rotY);
        newDir = DirectX::XMVector3TransformNormal(newDir, rotX);
        newDir = DirectX::XMVector3Normalize(newDir);
        
        DirectX::XMVECTOR newPos = DirectX::XMVectorAdd(
            targetVec, 
            DirectX::XMVectorScale(newDir, -radius));
        
        DirectX::XMStoreFloat3(&position, newPos);
        Update();
    }
    
    // ズーム（視野角の変更）
    void Zoom(float delta) {
        fovY += delta;
        // 視野角を制限
        if (fovY < DirectX::XM_PIDIV4 * 0.5f) fovY = DirectX::XM_PIDIV4 * 0.5f;
        if (fovY > DirectX::XM_PIDIV2) fovY = DirectX::XM_PIDIV2;
        
        Proj = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
    }
};
