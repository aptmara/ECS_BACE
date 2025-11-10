/**
 * @file CollisionSamples.h
 * @brief 衝突判定システムの使用例
 * @author 山内陽
 * @date 2025
 * @version 1.0
 * 
 * @details
 * Collisionシステムの様々な使用パターンを示すサンプルコードです。
 */
#pragma once

#include "components/Collision.h"
#include "components/Component.h"
#include "components/Transform.h"
#include "components/MeshRenderer.h"
#include "components/GameTags.h"
#include "ecs/World.h"
#include "app/DebugLog.h"
#include <DirectXMath.h>

namespace CollisionSamples {

// ========================================================
// サンプル1: 基本的な衝突判定
// ========================================================

/**
 * @brief 基本的な衝突判定のセットアップ
 * 
 * @details
 * プレイヤーとアイテムの衝突を検出する最もシンプルな例です。
 */
inline void Sample1_BasicCollision(World& world) {
    DEBUGLOG("=== Sample1: 基本的な衝突判定 ===");

    // 衝突検出システムを作成
    Entity collisionSystem = world.Create()
        .With<CollisionDetectionSystem>()
        .Build();

    // プレイヤーを作成(ボックス型)
    Entity player = world.Create()
        .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
      .With<MeshRenderer>(DirectX::XMFLOAT3{0, 1, 0})  // 緑色
  .With<CollisionBox>(DirectX::XMFLOAT3{1, 2, 1})  // 幅1, 高さ2, 奥行き1
        .With<PlayerTag>()
   .Build();

    // アイテムを作成(球体型)
  Entity item = world.Create()
        .With<Transform>(DirectX::XMFLOAT3{2, 0, 0})
      .With<MeshRenderer>(DirectX::XMFLOAT3{1, 1, 0})  // 黄色
        .With<CollisionSphere>(0.5f)  // 半径0.5
     .Build();

    // 衝突コールバックを登録
    auto* colSys = world.TryGet<CollisionDetectionSystem>(collisionSystem);
    if (colSys) {
  colSys->OnCollision([&](Entity a, Entity b, const CollisionInfo& info) {
         bool isPlayerItem = 
              (world.Has<PlayerTag>(a) && a == player && b == item) ||
       (world.Has<PlayerTag>(b) && b == player && a == item);

            if (isPlayerItem) {
      DEBUGLOG("? プレイヤーがアイテムを取得!");
         // アイテムを削除
   world.DestroyEntity(item);
      }
        });
    }

    DEBUGLOG("サンプル1完了: プレイヤーをX=2方向に移動させるとアイテムと衝突します");
}

// ========================================================
// サンプル2: レイヤーによる衝突フィルタリング
// ========================================================

/**
 * @brief レイヤーシステムを使った衝突フィルタリング
 * 
 * @details
 * 特定のレイヤー間でのみ衝突判定を行います。
 * 例: プレイヤーは敵と衝突するが、敵同士は衝突しない
 */
inline void Sample2_CollisionLayers(World& world) {
    DEBUGLOG("=== Sample2: 衝突レイヤーシステム ===");

    // レイヤー定義
    const uint8_t LAYER_PLAYER = 0;
    const uint8_t LAYER_ENEMY = 1;
    const uint8_t LAYER_WALL = 2;

    // プレイヤー: 敵(レイヤー1)と壁(レイヤー2)とのみ衝突
    Entity player = world.Create()
  .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
        .With<CollisionBox>(1.0f)
        .With<CollisionLayer>(LAYER_PLAYER, 0b0110)  // bit1とbit2が立っている
        .With<PlayerTag>()
    .Build();

    // 敵1: プレイヤー(レイヤー0)とのみ衝突
    Entity enemy1 = world.Create()
        .With<Transform>(DirectX::XMFLOAT3{3, 0, 0})
        .With<CollisionSphere>(0.5f)
 .With<CollisionLayer>(LAYER_ENEMY, 0b0001)  // bit0のみ
        .With<EnemyTag>()
        .Build();

    // 敵2: プレイヤー(レイヤー0)とのみ衝突(敵同士は衝突しない)
    Entity enemy2 = world.Create()
        .With<Transform>(DirectX::XMFLOAT3{3.5f, 0, 0})  // enemy1と近い位置
   .With<CollisionSphere>(0.5f)
        .With<CollisionLayer>(LAYER_ENEMY, 0b0001)
      .With<EnemyTag>()
    .Build();

    // 壁: すべてのレイヤーと衝突
  Entity wall = world.Create()
        .With<Transform>(DirectX::XMFLOAT3{5, 0, 0})
    .With<CollisionBox>(DirectX::XMFLOAT3{0.5f, 10.0f, 10.0f})
        .With<CollisionLayer>(LAYER_WALL, 0xFF)  // すべてのレイヤー
        .Build();

    DEBUGLOG("サンプル2完了: 敵同士は接触しても衝突しません");
}

// ========================================================
// サンプル3: 物理的な押し出し処理
// ========================================================

/**
 * @struct PhysicalCollisionResponse
 * @brief 衝突時に物理的な押し出しを行うBehaviour
 */
DEFINE_BEHAVIOUR(PhysicalCollisionResponse,
    // データメンバーなし
,
    // 衝突検出システムから衝突情報を取得して押し出し処理
 w.ForEach<CollisionDetectionSystem>([&](Entity sysEntity, CollisionDetectionSystem& sys) {
        // 今フレームで衝突したエンティティを処理
        // (実際には CollisionDetectionSystem に衝突ペアリストの取得メソッドが必要)
    // ここでは簡易実装として省略
    });
);

/**
 * @brief 物理的な押し出し処理のデモ
 */
inline void Sample3_PhysicalResponse(World& world) {
    DEBUGLOG("=== Sample3: 物理的な押し出し処理 ===");

    // 衝突検出システム
    Entity collisionSystem = world.Create()
.With<CollisionDetectionSystem>()
   .Build();

    // 動くオブジェクト
    Entity movable = world.Create()
   .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
        .With<CollisionBox>(1.0f)
        .Build();

  // 固定オブジェクト
    Entity fixed = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{2, 0, 0})
        .With<CollisionBox>(1.0f)
        .Build();

    // 衝突時の押し出し処理
    auto* colSys = world.TryGet<CollisionDetectionSystem>(collisionSystem);
    if (colSys) {
        colSys->OnCollision([&](Entity a, Entity b, const CollisionInfo& info) {
            // movableオブジェクトを押し出す
        auto* transformA = w.TryGet<Transform>(a);
         auto* transformB = w.TryGet<Transform>(b);

      if (!transformA || !transformB) return;

          // エンティティAがmovableの場合
            if (a == movable) {
           transformA->position.x -= info.normal.x * info.penetrationDepth;
    transformA->position.y -= info.normal.y * info.penetrationDepth;
        transformA->position.z -= info.normal.z * info.penetrationDepth;
          }
       // エンティティBがmovableの場合
   else if (b == movable) {
transformB->position.x += info.normal.x * info.penetrationDepth;
          transformB->position.y += info.normal.y * info.penetrationDepth;
   transformB->position.z += info.normal.z * info.penetrationDepth;
   }
        });
    }

    DEBUGLOG("サンプル3完了: 衝突時に自動的に押し出されます");
}

// ========================================================
// サンプル4: カプセル型キャラクターコントローラー
// ========================================================

/**
 * @brief カプセル型の衝突判定を使ったキャラクター
 * 
 * @details
 * カプセル形状は人型キャラクターに最適です。
 */
inline void Sample4_CapsuleCharacter(World& world) {
    DEBUGLOG("=== Sample4: カプセル型キャラクター ===");

    // カプセル型のキャラクター
    Entity character = world.Create()
        .With<Transform>(DirectX::XMFLOAT3{0, 1, 0})  // 地面から1単位上
 .With<CollisionCapsule>(
            0.5f,   // 半径
            2.0f,   // 高さ
   DirectX::XMFLOAT3{0, 0, 0}  // オフセット
        )
        .Build();

    // 地面(大きな平面ボックス)
    Entity ground = world.Create()
        .With<Transform>(DirectX::XMFLOAT3{0, -0.5f, 0})
  .With<CollisionBox>(DirectX::XMFLOAT3{100.0f, 1.0f, 100.0f})
        .Build();

    DEBUGLOG("サンプル4完了: カプセル型キャラクターは滑らかに移動できます");
}

// ========================================================
// サンプル5: トリガー(接触検出のみ)
// ========================================================

/**
 * @struct TriggerZone
 * @brief トリガーゾーン(物理的な衝突なし、検出のみ)
 */
struct TriggerZone : IComponent {
    bool isTriggered = false;
    std::function<void(Entity)> onEnter;
    std::function<void(Entity)> onExit;
};

/**
 * @brief トリガーゾーンのデモ
 */
inline void Sample5_TriggerZones(World& world) {
    DEBUGLOG("=== Sample5: トリガーゾーン ===");

    // ゴールエリア
    Entity goalZone = world.Create()
        .With<Transform>(DirectX::XMFLOAT3{10, 0, 0})
 .With<CollisionBox>(DirectX::XMFLOAT3{2, 5, 2})
        .Build();

    // プレイヤー
    Entity player = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
        .With<CollisionSphere>(0.5f)
   .With<PlayerTag>()
.Build();

    // 衝突検出システム
    Entity collisionSystem = world.Create()
        .With<CollisionDetectionSystem>()
  .Build();

    auto* colSys = world.TryGet<CollisionDetectionSystem>(collisionSystem);
    if (colSys) {
        colSys->OnCollision([&](Entity a, Entity b, const CollisionInfo& info) {
            bool isPlayerGoal = 
       (a == player && b == goalZone) ||
      (b == player && a == goalZone);

            if (isPlayerGoal) {
          DEBUGLOG("?? ゴール達成!");
           // ゴール処理: 次のステージへ移動など
            }
        });
    }

    DEBUGLOG("サンプル5完了: X=10付近でゴールエリアに入ります");
}

// ========================================================
// サンプル6: 複数の衝突形状を持つエンティティ
// ========================================================

/**
 * @brief 複数の衝突形状を持つ複雑なオブジェクト
 * 
 * @details
 * ECSでは1つのエンティティに複数の同じ型のコンポーネントは持てません。
 * 複雑な形状が必要な場合、複数の子エンティティを作成します。
 */
inline void Sample6_ComplexCollider(World& world) {
    DEBUGLOG("=== Sample6: 複雑な衝突形状 ===");

    // 親エンティティ(ロボット)
  Entity robot = world.Create()
  .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
        .Build();

    // 胴体(ボックス)
    Entity robotBody = world.Create()
        .With<Transform>(DirectX::XMFLOAT3{0, 1, 0})
        .With<CollisionBox>(DirectX::XMFLOAT3{1, 2, 0.5f})
      .Build();

    // 頭(球体)
    Entity robotHead = world.Create()
      .With<Transform>(DirectX::XMFLOAT3{0, 2.5f, 0})
        .With<CollisionSphere>(0.5f)
        .Build();

    DEBUGLOG("サンプル6完了: 複数パーツで構成された複雑な衝突形状");
 DEBUGLOG("注: 親子関係の実装が必要な場合は TransformHierarchy システムを追加してください");
}

// ========================================================
// 統合デモ: すべてのサンプルを実行
// ========================================================

/**
 * @brief すべてのサンプルを順番に実行
 */
inline void RunAllSamples(World& world) {
    DEBUGLOG("\n========================================");
    DEBUGLOG(" Collision System Samples");
    DEBUGLOG("========================================\n");

    Sample1_BasicCollision(world);
    DEBUGLOG("");

    // 他のサンプルは個別に実行可能
    // Sample2_CollisionLayers(world);
    // Sample3_PhysicalResponse(world);
    // Sample4_CapsuleCharacter(world);
    // Sample5_TriggerZones(world);
    // Sample6_ComplexCollider(world);

    DEBUGLOG("\n========================================");
    DEBUGLOG(" All Samples Completed!");
    DEBUGLOG("========================================\n");
}

} // namespace CollisionSamples

// ========================================================
// 実行例
// ========================================================
/*

// Game.cpp または任意のシーンで使用

#include "samples/CollisionSamples.h"

void GameScene::OnEnter(World& world) {
  // 基本的な衝突判定のデモ
    CollisionSamples::Sample1_BasicCollision(world);

    // または、すべてのサンプルを実行
    // CollisionSamples::RunAllSamples(world);
}

*/

// ========================================================
// 作成者: 山内陽
// バージョン: v1.0 - Collision System Samples
// ========================================================
