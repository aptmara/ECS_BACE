/**
 * @file CollisionEventSamples.h
 * @brief OnEnter/OnStay/OnExit イベントシステムの使用例
 * @author 山内陽
 * @date 2025
 * @version 1.0
 *
 * @details
 * Collisionシステムのイベントハンドラーの実用的な使用例を示します。
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

namespace CollisionEventSamples {

// ========================================================
// サンプル1: アイテム取得システム
// ========================================================

/**
 * @struct ItemCollector
 * @brief アイテムを取得するコンポーネント
 *
 * @details
 * プレイヤーがアイテムに触れたときに自動的に取得します。
 */
struct ItemCollector : ICollisionHandler {
    int itemsCollected = 0;

    void OnCollisionEnter(World &w, Entity self, Entity other, const CollisionInfo &info) override {
        // アイテムタグを持つエンティティと衝突したか確認
        if (w.Has<ItemTag>(other)) {
            itemsCollected++;
            DEBUGLOG("✨ アイテム取得! 合計: " + std::to_string(itemsCollected));

            // アイテムを削除
            w.DestroyEntity(other);
        }
    }
};

/**
 * @brief アイテム取得システムのデモ
 */
inline void Sample1_ItemCollection(World &world) {
    DEBUGLOG("=== Sample1: アイテム取得システム ===");

    // 衝突検出システム
    Entity collisionSystem = world.Create()
                                 .With<CollisionDetectionSystem>()
                                 .Build();

    // プレイヤー
    Entity player = world.Create()
                        .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
                        .With<CollisionSphere>(0.5f)
                        .With<PlayerTag>()
                        .With<ItemCollector>() // アイテムコレクター
                        .Build();

    // アイテム配置
    for (int i = 0; i < 5; ++i) {
        Entity item = world.Create()
                          .With<Transform>(DirectX::XMFLOAT3{static_cast<float>(i * 2), 0, 0})
                          .With<CollisionSphere>(0.3f)
                          .With<ItemTag>()
                          .Build();
    }

    DEBUGLOG("プレイヤーを右に移動させるとアイテムを取得します");
}

// ========================================================
// サンプル2: ダメージゾーン
// ========================================================

/**
 * @struct DamageZoneHandler
 * @brief ダメージゾーンに触れた時の処理
 */
struct DamageZoneHandler : ICollisionHandler {
    float damagePerSecond = 10.0f;

    void OnCollisionEnter(World &w, Entity self, Entity other, const CollisionInfo &info) override {
        DEBUGLOG("⚠️ ダメージゾーンに入った!");
    }

    void OnCollisionStay(World &w, Entity self, Entity other, const CollisionInfo &info) override {
        // 継続的にダメージを与える(60FPS想定で毎フレーム1/60秒分のダメージ)
        auto *health = w.TryGet<Health>(self);
        if (health) {
            health->TakeDamage(damagePerSecond / 60.0f);

            if (health->IsDead()) {
                DEBUGLOG("💀 死亡!");
            }
        }
    }

    void OnCollisionExit(World &w, Entity self, Entity other) override {
        DEBUGLOG("✅ ダメージゾーンから脱出!");
    }
};

/**
 * @brief ダメージゾーンのデモ
 */
inline void Sample2_DamageZone(World &world) {
    DEBUGLOG("=== Sample2: ダメージゾーン ===");

    Entity collisionSystem = world.Create()
                                 .With<CollisionDetectionSystem>()
                                 .Build();

    // プレイヤー
    Entity player = world.Create()
                        .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
                        .With<CollisionBox>(1.0f)
                        .With<Health>(100.0f, 100.0f)
                        .With<DamageZoneHandler>()
                        .Build();

    // ダメージゾーン
    Entity damageZone = world.Create()
                            .With<Transform>(DirectX::XMFLOAT3{5, 0, 0})
                            .With<CollisionBox>(DirectX::XMFLOAT3{3, 5, 3})
                            .Build();

    DEBUGLOG("プレイヤーをX=5付近に移動させるとダメージを受けます");
}

// ========================================================
// サンプル3: トリガーゾーン (チェックポイント)
// ========================================================

/**
 * @struct CheckpointTrigger
 * @brief チェックポイントトリガー
 */
struct CheckpointTrigger : ICollisionHandler {
    bool activated = false;

    void OnCollisionEnter(World &w, Entity self, Entity other, const CollisionInfo &info) override {
        if (activated)
            return;

        if (w.Has<PlayerTag>(other)) {
            activated = true;
            DEBUGLOG("🚩 チェックポイント到達!");

            // 色を変える(MeshRendererがある場合)
            auto *renderer = w.TryGet<MeshRenderer>(self);
            if (renderer) {
                renderer->color = DirectX::XMFLOAT3{0, 1, 0}; // 緑色に変更
            }
        }
    }
};

/**
 * @brief チェックポイントシステムのデモ
 */
inline void Sample3_Checkpoint(World &world) {
    DEBUGLOG("=== Sample3: チェックポイント ===");

    Entity collisionSystem = world.Create()
                                 .With<CollisionDetectionSystem>()
                                 .Build();

    // プレイヤー
    Entity player = world.Create()
                        .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
                        .With<CollisionSphere>(0.5f)
                        .With<PlayerTag>()
                        .Build();

    // チェックポイント配置
    for (int i = 1; i <= 3; ++i) {
        Entity checkpoint = world.Create()
                                .With<Transform>(DirectX::XMFLOAT3{static_cast<float>(i * 10), 0, 0})
                                .With<CollisionBox>(DirectX::XMFLOAT3{2, 5, 2})
                                .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0}) // 赤色
                                .With<CheckpointTrigger>()
                                .Build();
    }
}

// ========================================================
// サンプル4: 敵の攻撃判定
// ========================================================

/**
 * @struct EnemyAttackHandler
 * @brief 敵の攻撃判定
 */
struct EnemyAttackHandler : ICollisionHandler {
    float attackDamage = 20.0f;
    float attackCooldown = 1.0f;
    float cooldownTimer = 0.0f;

    void OnCollisionEnter(World &w, Entity self, Entity other, const CollisionInfo &info) override {
        if (w.Has<PlayerTag>(other)) {
            Attack(w, other);
        }
    }

    void OnCollisionStay(World &w, Entity self, Entity other, const CollisionInfo &info) override {
        // クールダウン中は攻撃しない
        if (cooldownTimer > 0.0f) {
            cooldownTimer -= 1.0f / 60.0f; // 60FPS想定
            return;
        }

        if (w.Has<PlayerTag>(other)) {
            Attack(w, other);
        }
    }

  private:
    void Attack(World &w, Entity target) {
        auto *health = w.TryGet<Health>(target);
        if (health) {
            health->TakeDamage(attackDamage);
            cooldownTimer = attackCooldown;
            DEBUGLOG("⚔️ 敵の攻撃! ダメージ: " + std::to_string(attackDamage));
        }
    }
};

/**
 * @brief 敵の攻撃判定のデモ
 */
inline void Sample4_EnemyAttack(World &world) {
    DEBUGLOG("=== Sample4: 敵の攻撃判定 ===");

    Entity collisionSystem = world.Create()
                                 .With<CollisionDetectionSystem>()
                                 .Build();

    // プレイヤー
    Entity player = world.Create()
                        .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
                        .With<CollisionBox>(1.0f)
                        .With<Health>(100.0f, 100.0f)
                        .With<PlayerTag>()
                        .Build();

    // 敵
    Entity enemy = world.Create()
                       .With<Transform>(DirectX::XMFLOAT3{3, 0, 0})
                       .With<CollisionBox>(1.0f)
                       .With<EnemyTag>()
                       .With<EnemyAttackHandler>()
                       .Build();
}

// ========================================================
// サンプル5: 物理的な押し出し処理
// ========================================================

/**
 * @struct PhysicsCollisionHandler
 * @brief 物理的な押し出し処理を行うハンドラー
 */
struct PhysicsCollisionHandler : ICollisionHandler {
    void OnCollisionStay(World &w, Entity self, Entity other, const CollisionInfo &info) override {
        // 壁など静的オブジェクトと衝突した場合、押し出す
        if (w.Has<WallTag>(other)) {
            auto *transform = w.TryGet<Transform>(self);
            if (transform) {
                // 衝突法線方向に侵入深度分だけ押し出す
                transform->position.x -= info.normal.x * info.penetrationDepth;
                transform->position.y -= info.normal.y * info.penetrationDepth;
                transform->position.z -= info.normal.z * info.penetrationDepth;
            }
        }
    }
};

/**
 * @brief 物理的な押し出しのデモ
 */
inline void Sample5_PhysicsCollision(World &world) {
    DEBUGLOG("=== Sample5: 物理的な押し出し ===");

    Entity collisionSystem = world.Create()
                                 .With<CollisionDetectionSystem>()
                                 .Build();

    // プレイヤー
    Entity player = world.Create()
                        .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
                        .With<CollisionBox>(1.0f)
                        .With<PhysicsCollisionHandler>()
                        .Build();

    // 壁
    Entity wall = world.Create()
                      .With<Transform>(DirectX::XMFLOAT3{5, 0, 0})
                      .With<CollisionBox>(DirectX::XMFLOAT3{0.5f, 10.0f, 10.0f})
                      .With<WallTag>()
                      .Build();

    DEBUGLOG("プレイヤーが壁に近づくと自動的に押し出されます");
}

// ========================================================
// サンプル6: 複合ハンドラー (複数の機能を持つ)
// ========================================================

/**
 * @struct AdvancedPlayerHandler
 * @brief 高度なプレイヤー衝突処理
 */
struct AdvancedPlayerHandler : ICollisionHandler {
    int itemsCollected = 0;
    int enemiesDefeated = 0;

    void OnCollisionEnter(World &w, Entity self, Entity other, const CollisionInfo &info) override {
        // アイテム取得
        if (w.Has<ItemTag>(other)) {
            itemsCollected++;
            DEBUGLOG("✨ アイテム取得! 合計: " + std::to_string(itemsCollected));
            w.DestroyEntity(other);
        }

        // 敵と衝突
        else if (w.Has<EnemyTag>(other)) {
            DEBUGLOG("⚔️ 敵と接触!");
            auto *health = w.TryGet<Health>(self);
            if (health) {
                health->TakeDamage(10.0f);
            }
        }

        // チェックポイント
        else if (w.Has<CheckpointTag>(other)) {
            DEBUGLOG("🚩 チェックポイント到達!");
        }
    }

    void OnCollisionExit(World &w, Entity self, Entity other) override {
        if (w.Has<EnemyTag>(other)) {
            DEBUGLOG("敵との接触終了");
        }
    }
};

/**
 * @brief 複合ハンドラーのデモ
 */
inline void Sample6_AdvancedHandler(World &world) {
    DEBUGLOG("=== Sample6: 複合ハンドラー ===");

    Entity collisionSystem = world.Create()
                                 .With<CollisionDetectionSystem>()
                                 .Build();

    // プレイヤー (すべての衝突を1つのハンドラーで処理)
    Entity player = world.Create()
                        .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
                        .With<CollisionBox>(1.0f)
                        .With<Health>(100.0f, 100.0f)
                        .With<PlayerTag>()
                        .With<AdvancedPlayerHandler>()
                        .Build();

    // アイテム
    Entity item = world.Create()
                      .With<Transform>(DirectX::XMFLOAT3{3, 0, 0})
                      .With<CollisionSphere>(0.3f)
                      .With<ItemTag>()
                      .Build();

    // 敵
    Entity enemy = world.Create()
                       .With<Transform>(DirectX::XMFLOAT3{6, 0, 0})
                       .With<CollisionBox>(1.0f)
                       .With<EnemyTag>()
                       .Build();
}

// ========================================================
// 統合デモ
// ========================================================

/**
 * @brief すべてのサンプルを実行
 */
inline void RunAllEventSamples(World &world) {
    DEBUGLOG("\n========================================");
    DEBUGLOG(" Collision Event System Samples");
    DEBUGLOG("========================================\n");

    // Sample1_ItemCollection(world);
    // Sample2_DamageZone(world);
    // Sample3_Checkpoint(world);
    // Sample4_EnemyAttack(world);
    // Sample5_PhysicsCollision(world);
    Sample6_AdvancedHandler(world);

    DEBUGLOG("\n========================================");
    DEBUGLOG(" Event Samples Completed!");
    DEBUGLOG("========================================\n");
}

} // namespace CollisionEventSamples

// ========================================================
// 必要なタグの定義 (GameTags.hに定義されていない場合)
// ========================================================

#ifndef ITEM_TAG_DEFINED
struct ItemTag : IComponent {};
#define ITEM_TAG_DEFINED
#endif

#ifndef WALL_TAG_DEFINED
struct WallTag : IComponent {};
#define WALL_TAG_DEFINED
#endif

#ifndef CHECKPOINT_TAG_DEFINED
struct CheckpointTag : IComponent {};
#define CHECKPOINT_TAG_DEFINED
#endif

// Health コンポーネントがない場合の簡易定義
#ifndef HEALTH_COMPONENT_DEFINED
struct Health : IComponent {
    float current = 100.0f;
    float max = 100.0f;

    Health(float c = 100.0f, float m = 100.0f)
        : current(c), max(m) {}

    void TakeDamage(float damage) {
        current -= damage;
        if (current < 0.0f)
            current = 0.0f;
    }

    bool IsDead() const {
        return current <= 0.0f;
    }
};
#define HEALTH_COMPONENT_DEFINED
#endif

// ========================================================
// 使用例
// ========================================================
/*

// Game.cpp または任意のシーンで使用

#include "samples/CollisionEventSamples.h"

void GameScene::OnEnter(World& world) {
    // サンプルを実行
  CollisionEventSamples::Sample1_ItemCollection(world);

 // または、すべてのサンプルを実行
    // CollisionEventSamples::RunAllEventSamples(world);
}

*/

// ========================================================
// 作成者: 山内陽
// バージョン: v1.0 - Collision Event System Samples
// ========================================================
