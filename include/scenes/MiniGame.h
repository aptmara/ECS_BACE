/**
 * @file MiniGame.h
 * @brief シンプルなシューティングゲーム
 * @author 山内陽
 * @date 2025
 */
#pragma once

#include "pch.h"
#include <cstdlib>
#include <ctime>
#include "util/Random.h"

// ========================================================
// ゲームシーン
// ========================================================

/**
 * @class GameScene
 * @brief シューティングゲームのメインシーン
 *
 * @details
 * プレイヤー操作、敵の生成、弾の発射、衝突判定を管理します。
 *
 * ### ゲームルール:
 * - A/Dキーでプレイヤーを左右に移動
 * - スペースキーで弾を発射
 * - ランダムな形状・色の敵が上から降ってくる
 * - 弾が敵に当たると両方消滅し、スコア+10
 *
 * @author 山内陽
 */
class GameScene : public IScene {
public:
    /**
     * @brief シーン開始時の初期化
     * @param[in,out] world ワールド参照
     *
     * @details
     * プレイヤー、地面を生成し、スコアとタイマーを初期化します。
     */
    void OnEnter(World& world) override {

    }

    /**
     * @brief 毎フレーム更新処理
     * @param[in,out] world ワールド参照
     * @param[in] input 入力システム参照
     * @param[in] deltaTime デルタタイム(秒単位)
     */
    void OnUpdate(World& world, InputSystem& input, float deltaTime) override {

        // ゲームロジックの更新
        world.Tick(deltaTime);
    }

    /**
     * @brief シーン終了時のクリーンアップ
     * @param[in,out] world ワールド参照
     *
     * @details
     * すべてのエンティティを削除します。
     */
    void OnExit(World& world) override {
        for (const auto& entity : ownedEntities_) {
            if (world.IsAlive(entity)) {
                world.DestroyEntityWithCause(entity, World::Cause::SceneUnload);
            }
        }
        ownedEntities_.clear();
    }
private:


    Entity playerEntity_;        ///< プレイヤーエンティティ
    int score_;                  ///< 現在のスコア
    std::vector<Entity> ownedEntities_; ///< シーンが生成したエンティティ
};