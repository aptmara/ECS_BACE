/**
 * @file Game.h
 * @brief ゲームのメインシーン
 * @author 山内陽
 * @date 2025
 */
#pragma once

#include "pch.h"
#include "components/GameTags.h"
#include "components/PlayerComponents.h"
#include "components/MeshRenderer.h"
#include "components/Collision.h"
#include "input/InputSystem.h"
#include "input/GamepadSystem.h"
#include "components/Model.h"
#include "components/ModelComponent.h"
#include "components/Rotator.h"
#include "components/Light.h"
#include "systems/ModelLoadingSystem.h"
#include "app/ServiceLocator.h"
#include "app/ResourceManager.h"

// ========================================================
// 実用的な衝突ハンドラー (NEW!)
// ========================================================

/**
 * @struct PlayerCollisionHandler
 * @brief プレイヤーの衝突イベントを処理
 */
struct PlayerCollisionHandler : ICollisionHandler {
    void OnCollisionEnter(World &w, Entity self, Entity other, const CollisionInfo &info) override {
        // 敵との衝突
        if (w.Has<EnemyTag>(other)) {
            DEBUGLOG("⚔️ プレイヤーが敵と衝突! 侵入深度: " + std::to_string(info.penetrationDepth));

            // TODO: ダメージ処理
            // auto* health = w.TryGet<Health>(self);
            // if (health) health->TakeDamage(10.0f);
        }
    }

    void OnCollisionStay(World &w, Entity self, Entity other, const CollisionInfo &info) override {
        // 継続的な衝突処理(例: ダメージゾーンなど)
    }

    void OnCollisionExit(World &w, Entity self, Entity other) override {
        if (w.Has<EnemyTag>(other)) {
            DEBUGLOG("✅ 敵との衝突終了");
        }
    }
};

/**
 * @struct EnemyCollisionHandler
 * @brief 敵の衝突イベントを処理
 */
struct EnemyCollisionHandler : ICollisionHandler {
    void OnCollisionEnter(World &w, Entity self, Entity other, const CollisionInfo &info) override {
        if (w.Has<PlayerTag>(other)) {
            DEBUGLOG("💥 敵がプレイヤーに接触!");
        }
    }
};

// ========================================================
// ゲームシーン
// ========================================================

class GameScene : public IScene {
  public:
    void OnEnter(World &world) override {
        DEBUGLOG("GameScene::OnEnter() - ゲーム開始");

        // システムエンティティを作成
        world.Create().With<ModelLoadingSystem>();

        // 衝突検出システムを作成
        Entity collisionSystem = world.Create()
                                     .With<CollisionDetectionSystem>()
                                     .Build();

        // 衝突コールバックを登録(グローバル)
        auto *colSys = world.TryGet<CollisionDetectionSystem>(collisionSystem);
        if (colSys) {
            colSys->SetDebugLog(true); // 🔧 一時的に有効化して衝突検出を確認

            // グローバルコールバック(すべての衝突を検出)
            colSys->OnCollision([&](Entity a, Entity b, const CollisionInfo &info) {
                // グローバルな衝突処理(統計など)
            });
        }

        ownedEntities_.push_back(collisionSystem);

#ifdef _DEBUG
        // デバッグビルド時のみ衝突形状を可視化
        Entity debugRenderer = world.Create()
                                   .With<CollisionDebugRenderer>()
                                   .Build();
        ownedEntities_.push_back(debugRenderer);
#endif

        // ライト作成
        world.Create().With<DirectionalLight>();

        // プレイヤー作成
        CreatePlayer(world);

        // テスト用の敵を作成
        CreateTestEnemy(world);

        DEBUGLOG("GameScene::OnEnter() - 初期化完了");
    }

    void OnUpdate(World &world, InputSystem &input, float deltaTime) override {
        // PlayerMovementコンポーネントにInputSystemとGamepadSystemの参照を設定
        world.ForEach<PlayerMovement>([&](Entity e, PlayerMovement &pm) {
            if (!pm.input_) {
                pm.input_ = &input;
            }
            if (!pm.gamepad_) {
                pm.gamepad_ = &ServiceLocator::Get<GamepadSystem>();
            }
        });

        // ECSシステムを更新(Behaviourコンポーネントのみ自動更新)
        world.Tick(deltaTime);
    }

    void OnExit(World &world) override {
        DEBUGLOG("GameScene::OnExit() - ゲーム終了");

        // シーンが管理するエンティティを削除
        for (const auto &entity : ownedEntities_) {
            if (world.IsAlive(entity)) {
                world.DestroyEntityWithCause(entity, World::Cause::SceneUnload);
            }
        }
        ownedEntities_.clear();

        DEBUGLOG("GameScene::OnExit() - クリーンアップ完了");
    }

  private:
    void CreatePlayer(World &world) {
        Transform transform{
            {0.0f, 0.0f, 5.0f},
            {0.0f, 0.0f, 0.0f},
            {1.0f, 1.0f, 1.0f},
        };

        MeshRenderer renderer;
        renderer.meshType = MeshType::Cube;
        renderer.color = DirectX::XMFLOAT3{0.0f, 1.0f, 0.0f};

        Entity player = world.Create()
                            .With<Transform>(transform)
                            .With<MeshRenderer>(renderer)
                            .With<PlayerTag>()
                            .With<PlayerMovement>()
                            .With<Rotator>(45.0f)
                            .With<CollisionBox>(DirectX::XMFLOAT3{1.0f, 2.0f, 1.0f})
                            .With<PlayerCollisionHandler>() //  イベントハンドラー追加
                            .Build();

        DEBUGLOG("CreatePlayer: Player entity created - ID: " + std::to_string(player.id));
        DEBUGLOG("CreatePlayer: Has PlayerCollisionHandler: " + std::string(world.Has<PlayerCollisionHandler>(player) ? "YES" : "NO"));

        ownedEntities_.push_back(player);
        playerEntity_ = player;
    }

    void CreateTestEnemy(World &world) {
        Transform transform{
            {1.5f, 0.0f, 5.0f}, // X=1.5に変更 (プレイヤーから1.5単位の距離)
            {0.0f, 0.0f, 0.0f},
            {1.0f, 1.0f, 1.0f},
        };

        MeshRenderer renderer;
        renderer.meshType = MeshType::Sphere;
        renderer.color = DirectX::XMFLOAT3{1.0f, 0.0f, 0.0f};

        Entity enemy = world.Create()
                           .With<Transform>(transform)
                           .With<MeshRenderer>(renderer)
                           .With<EnemyTag>()
                           .With<CollisionSphere>(0.5f)
                           .With<EnemyCollisionHandler>() //  イベントハンドラー追加
                           .Build();

        DEBUGLOG("CreateTestEnemy: Enemy entity created - ID: " + std::to_string(enemy.id));
        ownedEntities_.push_back(enemy);
    }

    Entity playerEntity_;
    std::vector<Entity> ownedEntities_;
};

// ========================================================
// テストシーン
// ========================================================

class TestScene : public IScene {
  public:
    void OnEnter(World &world) override {
        DEBUGLOG("TestScene::OnEnter() - テスト開始");

        // 衝突検出システムを作成
        Entity collisionSystem = world.Create()
                                     .With<CollisionDetectionSystem>()
                                     .Build();

        // プレイヤーエンティティを作成
        Entity player = world.Create()
                            .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
                            .With<CollisionBox>(DirectX::XMFLOAT3{1, 1, 1})
                            .With<PlayerCollisionHandler>()
                            .Build();

        // 敵エンティティを作成
        Entity enemy = world.Create()
                           .With<Transform>(DirectX::XMFLOAT3{2, 0, 0})
                           .With<CollisionBox>(DirectX::XMFLOAT3{1, 1, 1})
                           .Build();

        DEBUGLOG("TestScene::OnEnter() - 初期化完了");
    }

    void OnUpdate(World &world, InputSystem &input, float deltaTime) override {
        // プレイヤーを移動させて衝突をテスト
        world.ForEach<Transform>([&](Entity e, Transform &t) {
            if (e.id == 1) {                      // プレイヤーエンティティのIDを仮定
                t.position.x += 0.1f * deltaTime; // X方向に移動
            }
        });

        world.Tick(deltaTime);
    }

    void OnExit(World &world) override {
        DEBUGLOG("TestScene::OnExit() - テスト終了");
    }
};
