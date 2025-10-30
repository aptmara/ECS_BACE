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
#include "input/GamepadSystem.h" // GamepadSystem ヘッダーをインクルード

// ========================================================
// ゲームシーン
// ========================================================

/**
 * @class GameScene
 * @brief ゲームのメインシーン
 *
 * @author 山内陽
 */
class GameScene : public IScene {
public:
    /**
     * @brief シーン開始時の初期化
     * @param[in,out] world ワールド参照
     */
    void OnEnter(World& world) override {
    DEBUGLOG("GameScene::OnEnter() - ゲーム開始");

    // プレイヤー作成
        CreatePlayer(world);

    DEBUGLOG("GameScene::OnEnter() - 初期化完了");
    }

    /**
     * @brief 毎フレーム更新処理
     * @param[in,out] world ワールド参照
     * @param[in] input 入力システム参照
     * @param[in] deltaTime デルタタイム(秒単位)
     */
    void OnUpdate(World& world, InputSystem& input, float deltaTime) override {
    // ECSシステム更新（Behaviourコンポーネントが自動実行される）
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
