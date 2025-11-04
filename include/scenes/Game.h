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
// ゲームシーン
// ========================================================

class GameScene : public IScene {
  public:
    void OnEnter(World &world) override {
        DEBUGLOG("GameScene::OnEnter() - ゲーム開始");

        // システムエンティティを作成
        world.Create().With<ModelLoadingSystem>();

        // ライト作成
        world.Create().With<DirectionalLight>();

        // プレイヤー作成
        CreatePlayer(world);

        DEBUGLOG("GameScene::OnEnter() - 初期化完了");
    }

    void OnUpdate(World &world, InputSystem &input, float deltaTime) override {

        //collisionsというリストを作った
        std::vector<std::pair<Entity, CollisionGet>> collisions;
        //作ったリストにentityとCollisionGetを入れる
        world.ForEach<Collision>([&](Entity e, Collision &c) {
            collisions.push_back({e, c.data});
        });

        //collisionsに入っている値を順番に判定
        for (int i = 0; i < collisions.size(); i++) {
            for (int j = i + 1; j < collisions.size(); j++) {
                auto &a = collisions[i].second;
                auto &b = collisions[j].second;
                Check_AABB_AABB(a, b);
            }
        }

        // PlayerMovementコンポーネントにInputSystemとGamepadSystemの参照を設定
        world.ForEach<PlayerMovement>([&](Entity e, PlayerMovement &pm) {
            if (!pm.input_) {
                pm.input_ = &input;
            }
            if (!pm.gamepad_) {
                pm.gamepad_ = &ServiceLocator::Get<GamepadSystem>();
            }
        });

        world.Tick(deltaTime);
    }

    /**
     * @brief シーン終了時のクリーンアップ
     * @param[in,out] world ワールド参照
     */
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
    /**
     * @brief プレイヤーを作成
     * @param[in,out] world ワールド参照
     */
    void CreatePlayer(World &world) {
        // エンティティ作成
        // カメラは{0, 20, -20}から{0, 0, 0}を見ているため、
        // プレイヤーをZ=5付近に配置してカメラの視野内に表示
        Transform transform{
            {0.0f, 0.0f, 5.0f},
            {0.0f, 0.0f, 0.0f},
            {1.0f, 1.0f, 1.0f},
        };

        // MeshRendererを使ってキューブとして描画
        MeshRenderer renderer;
        renderer.meshType = MeshType::Cube;
        renderer.color = DirectX::XMFLOAT3{0.0f, 1.0f, 0.0f}; // 緑色

        // プレイヤーエンティティを作成
        Entity player = world.Create()
                            .With<Transform>(transform)
                            .With<MeshRenderer>(renderer)
                            .With<PlayerTag>()
                            .With<PlayerMovement>() // プレイヤー移動コンポーネントを追加
                            .With<Rotator>(45.0f)   // 回転速度を45度/秒に修正
                            .Build();

        DEBUGLOG("CreatePlayer: Player entity created - ID: " + std::to_string(player.id) + ", Gen: " + std::to_string(player.gen));
        DEBUGLOG("CreatePlayer: Position: (" + std::to_string(transform.position.x) + ", " +
                 std::to_string(transform.position.y) + ", " +
                 std::to_string(transform.position.z) + ")");
        DEBUGLOG("CreatePlayer: Has Transform: " + std::string(world.Has<Transform>(player) ? "YES" : "NO"));
        DEBUGLOG("CreatePlayer: Has PlayerTag: " + std::string(world.Has<PlayerTag>(player) ? "YES" : "NO"));
        DEBUGLOG("CreatePlayer: Has MeshRenderer: " + std::string(world.Has<MeshRenderer>(player) ? "YES" : "NO"));

        ownedEntities_.push_back(player);
        playerEntity_ = player;
    }

    bool Check_AABB_AABB(const CollisionGet &a, const CollisionGet &b) {
        XMFLOAT3 aMin = {a.center.x - a.size.x * 0.5f, a.center.y - a.size.y * 0.5f, a.center.z - a.size.z * 0.5f}; //aの最小の値(イメージだと左下の手前)
        XMFLOAT3 aMax = {a.center.x + a.size.x * 0.5f, a.center.y + a.size.y * 0.5f, a.center.z + a.size.z * 0.5f}; //aの最大の値(右上の奥)
        XMFLOAT3 bMin = {b.center.x - b.size.x * 0.5f, b.center.y - b.size.y * 0.5f, b.center.z - b.size.z * 0.5f}; //bの最小の値(左下の手前)
        XMFLOAT3 bMax = {b.center.x + b.size.x * 0.5f, b.center.y + b.size.y * 0.5f, b.center.z + b.size.z * 0.5f}; //bの最大の値(右上の奥)

        return (aMin.x <= bMax.x && aMax.x >= bMin.x) && //x軸で当たっているか
               (aMin.y <= bMax.y && aMax.y >= bMin.y) && //y軸で当たっているか
               (aMin.z <= bMax.z && aMax.z >= bMin.z);   //z軸で当たっているか
    }
    Entity playerEntity_;               ///< プレイヤーエンティティ
    std::vector<Entity> ownedEntities_; ///< シーンが管理するエンティティ
};
