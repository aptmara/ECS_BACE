/**
 * @file TransformHierarchySystem.h
 * @brief Transform階層構造を更新するシステム
 * @author 山内陽
 * @date 2025
 * @version 6.0
 * 
 * @details
 * 親子関係を持つTransformの階層的な変換を計算し、
 * 子エンティティのワールド座標を親の変換に基づいて更新します。
 */
#pragma once

#include "ecs/World.h"
#include "components/Component.h"
#include "components/Transform.h"
#include "components/TransformHierarchy.h"
#include <DirectXMath.h>
#include <unordered_set>

/**
 * @struct TransformHierarchySystem
 * @brief Transform階層を更新するBehaviourシステム
 * 
 * @details
 * このシステムは毎フレーム実行され、親子関係を持つすべてのエンティティの
 * Transform（位置・回転・スケール）を階層的に更新します。
 * 
 * ### 更新順序:
 * 1. ルートノード（親を持たないエンティティ）から開始
 * 2. 親のワールド変換行列を計算
 * 3. 子のローカル変換を親のワールド変換に適用
 * 4. 子の子（孫）に対して再帰的に処理
 * 
 * ### 使用方法:
 * WorldにTransformHierarchySystemを追加することで自動的に動作します。
 * 
 * @par 使用例
 * @code
 * // システムをWorldに追加
 * Entity system = world.Create()
 *     .With<TransformHierarchySystem>()
 *     .Build();
 * 
 * // 親子関係を持つエンティティを作成
 * Entity parent = world.Create()
 *     .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
 *     .With<TransformHierarchy>()
 * .Build();
 * 
 * Entity child = world.Create()
 *     .With<Transform>(DirectX::XMFLOAT3{2, 0, 0})
 *     .With<TransformHierarchy>()
 *     .Build();
 * 
 * // 親子関係を設定
 * world.Get<TransformHierarchy>(child).SetParent(parent);
 * world.Get<TransformHierarchy>(parent).AddChild(child);
 * 
 * // 以降、毎フレーム自動的に階層が更新される
 * @endcode
 */
struct TransformHierarchySystem : Behaviour {
    void OnUpdate(World& world, Entity self, float dt) override {
        using namespace DirectX;

  std::unordered_set<uint32_t> processed;

        world.ForEach<Transform, TransformHierarchy>(
     [&](Entity entity, Transform& transform, TransformHierarchy& hierarchy) {
    if (hierarchy.HasParent()) {
        return;
    }
         UpdateHierarchyRecursive(world, entity, XMMatrixIdentity(), processed);
            }
        );
    }

private:
    /**
  * @brief 階層構造を再帰的に更新
     * @param world Worldインスタンス
     * @param entity 現在のエンティティ
     * @param parentWorldMatrix 親のワールド変換行列
     * @param processed 処理済みエンティティのセット（循環参照防止）
     */
    void UpdateHierarchyRecursive(
        World& world,
        Entity entity,
        DirectX::XMMATRIX parentWorldMatrix,
std::unordered_set<uint32_t>& processed
  ) {
        using namespace DirectX;

     if (processed.find(entity.id) != processed.end()) {
     return;
        }
      processed.insert(entity.id);

        auto* transform = world.TryGet<Transform>(entity);
     auto* hierarchy = world.TryGet<TransformHierarchy>(entity);

        if (!transform || !hierarchy) {
   return;
        }

    XMMATRIX localMatrix = ComputeLocalMatrix(*transform);
        XMMATRIX worldMatrix = XMMatrixMultiply(localMatrix, parentWorldMatrix);

if (hierarchy->HasParent()) {
     DecomposeWorldMatrix(worldMatrix, *transform);
        }

     for (const Entity& child : hierarchy->GetChildren()) {
  if (!world.IsAlive(child)) {
     continue;
            }
        UpdateHierarchyRecursive(world, child, worldMatrix, processed);
        }
    }

    /**
     * @brief ローカル変換行列を計算
     * @param transform Transformコンポーネント
     * @return ローカル変換行列
     */
    DirectX::XMMATRIX ComputeLocalMatrix(const Transform& transform) const {
      using namespace DirectX;

        XMMATRIX scaleMatrix = XMMatrixScaling(
            transform.scale.x,
transform.scale.y,
      transform.scale.z
        );

     float radX = XMConvertToRadians(transform.rotation.x);
        float radY = XMConvertToRadians(transform.rotation.y);
     float radZ = XMConvertToRadians(transform.rotation.z);

      XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(radX, radY, radZ);

        XMMATRIX translationMatrix = XMMatrixTranslation(
  transform.position.x,
       transform.position.y,
            transform.position.z
        );

        return XMMatrixMultiply(XMMatrixMultiply(scaleMatrix, rotationMatrix), translationMatrix);
    }

    /**
     * @brief ワールド変換行列を位置・回転・スケールに分解
     * @param worldMatrix ワールド変換行列
     * @param transform 出力先のTransformコンポーネント
     */
    void DecomposeWorldMatrix(DirectX::XMMATRIX worldMatrix, Transform& transform) const {
        using namespace DirectX;

        XMVECTOR scale;
        XMVECTOR rotation;
      XMVECTOR translation;

        XMMatrixDecompose(&scale, &rotation, &translation, worldMatrix);

        XMStoreFloat3(&transform.position, translation);
        XMStoreFloat3(&transform.scale, scale);

        XMFLOAT4 rotQuat;
    XMStoreFloat4(&rotQuat, rotation);
        XMVECTOR euler = QuaternionToEuler(rotation);
      XMFLOAT3 eulerDeg;
        XMStoreFloat3(&eulerDeg, euler);
   
        transform.rotation.x = XMConvertToDegrees(eulerDeg.x);
        transform.rotation.y = XMConvertToDegrees(eulerDeg.y);
     transform.rotation.z = XMConvertToDegrees(eulerDeg.z);
    }

    /**
     * @brief クォータニオンをオイラー角に変換
     * @param q クォータニオン
     * @return オイラー角（ラジアン）
     */
DirectX::XMVECTOR QuaternionToEuler(DirectX::XMVECTOR q) const {
        using namespace DirectX;

        XMFLOAT4 qf;
  XMStoreFloat4(&qf, q);

        float sinr_cosp = 2.0f * (qf.w * qf.x + qf.y * qf.z);
   float cosr_cosp = 1.0f - 2.0f * (qf.x * qf.x + qf.y * qf.y);
        float roll = atan2f(sinr_cosp, cosr_cosp);

float sinp = 2.0f * (qf.w * qf.y - qf.z * qf.x);
  float pitch;
        if (fabsf(sinp) >= 1.0f) {
   pitch = copysignf(XM_PIDIV2, sinp);
        } else {
        pitch = asinf(sinp);
        }

float siny_cosp = 2.0f * (qf.w * qf.z + qf.x * qf.y);
        float cosy_cosp = 1.0f - 2.0f * (qf.y * qf.y + qf.z * qf.z);
        float yaw = atan2f(siny_cosp, cosy_cosp);

   return XMVectorSet(pitch, yaw, roll, 0.0f);
    }
};
