/**
 * @file MainGame.h
 * @brief シンプルなシューティングゲーム
 * @author 山内陽
 * @date 2025
 */
#pragma once

#include "pch.h"
#include "components/GameTags.h"
#include "components/PlayerComponents.h"
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
    void OnEnter(World& world) override {
        DEBUGLOG("GameScene::OnEnter() - ゲーム開始");

        // システムエンティティを作成
        world.Create().With<ModelLoadingSystem>();

        // ライト作成
        world.Create().With<DirectionalLight>();

        // プレイヤー作成
        CreatePlayer(world);

        // モデルを持つエンティティを作成
        Entity modelEntity = world.Create()
            .With<Transform>(DirectX::XMFLOAT3{0, 1, 0}, DirectX::XMFLOAT3{0, 0, 0}, DirectX::XMFLOAT3{1, 1, 1})
            .With<Model>(Model{"Assets/Models/test.fbx"}) // 修正: Model オブジェクトを明示的に構築
            .With<Rotator>(Rotator{45.0f}) // 修正: Rotator のコンストラクタ呼び出しを明示
            .Build();
        ownedEntities_.push_back(modelEntity);

        DEBUGLOG("GameScene::OnEnter() - 初期化完了");
    }

    void OnUpdate(World& world, InputSystem& input, float deltaTime) override {
        world.Tick(deltaTime);
    }

    /**
     * @brief シーン終了時のクリーンアップ
     * @param[in,out] world ワールド参照
     */
    void OnExit(World& world) override {
        DEBUGLOG("GameScene::OnExit() - ゲーム終了");

        // シーンが管理するエンティティを削除
        for (const auto& entity : ownedEntities_) {
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
    void CreatePlayer(World& world) {
        // エンティティ作成
        Entity player = world.Create()
        .With<Transform>()
        .With<MeshRenderer>()
        .With<PlayerTag>()
        .With<PlayerMovement>()
        .Build();

        ownedEntities_.push_back(player);
        playerEntity_ = player;
    }
    Entity playerEntity_;          ///< プレイヤーエンティティ
    std::vector<Entity> ownedEntities_;     ///< シーンが管理するエンティティ
};
