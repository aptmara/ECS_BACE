/**
 * @file MiniGame.h
 * @brief シンプルなシューティングゲーム
 * @author 山内陽
 * @date 2024
 * @version 4.0
 * 
 * @details
 * ### ゲーム内容:
 * - プレイヤー(緑キューブ)をA/Dキーで左右に移動
 * - スペースキーで弾を発射
 * - 敵(赤キューブ)が自動で降ってくる
 * - 弾が敵に当たると敵が消滅
 * - スコアが貯まっていく！
 */
#pragma once

#include "scenes/SceneManager.h"
#include "components/Transform.h"
#include "components/MeshRenderer.h"
#include "components/Component.h"
#include <cstdlib>
#include <ctime>
#include <vector>

// ========================================================
// ゲーム用コンポーネント
// ========================================================

/**
 * @struct Player
 * @brief プレイヤータグ
 * @details プレイヤーエンティティを識別するためのマーカー
 */
struct Player : IComponent {};

/**
 * @struct Enemy
 * @brief 敵タグ
 * @details 敵エンティティを識別するためのマーカー
 */
struct Enemy : IComponent {};

/**
 * @struct Bullet
 * @brief 弾タグ
 * @details 弾エンティティを識別するためのマーカー
 */
struct Bullet : IComponent {};

/**
 * @struct PlayerMovement
 * @brief プレイヤーの移動制御Behaviour
 * 
 * @details
 * プレイヤーの位置を制限し、画面外に出ないようにします。
 * 
 * @author 山内陽
 */
struct PlayerMovement : Behaviour {
    float speed = 8.0f;  ///< 移動速度

    /**
     * @brief 毎フレーム更新処理
     * @param[in,out] w ワールド参照
     * @param[in] self 自身のエンティティ
     * @param[in] dt デルタタイム（秒）
     */
    void OnUpdate(World& w, Entity self, float dt) override {
        auto* t = w.TryGet<Transform>(self);
        if (!t) return;

        // キーボード入力はGameSceneで処理
        // ここでは位置の制限のみ
        if (t->position.x < -8.0f) t->position.x = -8.0f;
        if (t->position.x > 8.0f) t->position.x = 8.0f;
    }
};

/**
 * @struct BulletMovement
 * @brief 弾の移動Behaviour
 * 
 * @details
 * 弾を上方向に移動させ、画面外に出たら削除します。
 * 
 * @author 山内陽
 */
struct BulletMovement : Behaviour {
    float speed = 15.0f;  ///< 移動速度

    /**
     * @brief 毎フレーム更新処理
     * @param[in,out] w ワールド参照
     * @param[in] self 自身のエンティティ
     * @param[in] dt デルタタイム（秒）
     */
    void OnUpdate(World& w, Entity self, float dt) override {
        auto* t = w.TryGet<Transform>(self);
        if (!t) return;

        t->position.y += speed * dt;

        // 画面外に出たら削除
        if (t->position.y > 10.0f) {
            w.DestroyEntity(self);
        }
    }
};

/**
 * @struct EnemyMovement
 * @brief 敵の移動Behaviour
 * 
 * @details
 * 敵を下方向に移動させ、画面外に出たら削除します。
 * 
 * @author 山内陽
 */
struct EnemyMovement : Behaviour {
    float speed = 3.0f;  ///< 移動速度

    /**
     * @brief 毎フレーム更新処理
     * @param[in,out] w ワールド参照
     * @param[in] self 自身のエンティティ
     * @param[in] dt デルタタイム（秒）
     */
    void OnUpdate(World& w, Entity self, float dt) override {
        auto* t = w.TryGet<Transform>(self);
        if (!t) return;

        t->position.y -= speed * dt;

        // 画面下に出たら削除
        if (t->position.y < -8.0f) {
            w.DestroyEntity(self);
        }
    }
};

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
 * - 敵が上から降ってくる
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
     * プレイヤーを生成し、スコアとタイマーを初期化します。
     */
    void OnEnter(World& world) override {
        // 乱数シードを設定
        srand(static_cast<unsigned int>(time(nullptr)));

        // プレイヤーを作成
        playerEntity_ = world.Create()
            .With<Transform>(
                DirectX::XMFLOAT3{0.0f, -6.0f, 0.0f},   // 画面下部
                DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f},
                DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f}
            )
            .With<MeshRenderer>(DirectX::XMFLOAT3{0.0f, 1.0f, 0.0f}) // 緑色
            .With<Player>()
            .With<PlayerMovement>()
            .Build();

        score_ = 0;
        enemySpawnTimer_ = 0.0f;
        shootCooldown_ = 0.0f;
    }

    /**
     * @brief 毎フレーム更新処理
     * @param[in,out] world ワールド参照
     * @param[in] input 入力システム参照
     * @param[in] deltaTime デルタタイム（秒）
     */
    void OnUpdate(World& world, InputSystem& input, float deltaTime) override {
        // プレイヤー移動
        UpdatePlayerMovement(world, input, deltaTime);

        // 弾の発射
        UpdateShooting(world, input, deltaTime);

        // 敵の生成
        UpdateEnemySpawning(world, deltaTime);

        // 衝突判定
        CheckCollisions(world);

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
        // 全エンティティを削除(イテレータ破壊を回避)
        std::vector<Entity> entitiesToDestroy;

        world.ForEach<Transform>([&](Entity e, Transform& t) {
            entitiesToDestroy.push_back(e);
        });

        for (const auto& entity : entitiesToDestroy) {
            world.DestroyEntity(entity);
        }
    }

    /**
     * @brief 現在のスコアを取得
     * @return int 現在のスコア
     */
    int GetScore() const { return score_; }

private:
    /**
     * @brief プレイヤーの移動処理
     * @param[in,out] world ワールド参照
     * @param[in] input 入力システム参照
     * @param[in] deltaTime デルタタイム（秒）
     */
    void UpdatePlayerMovement(World& world, InputSystem& input, float deltaTime) {
        auto* playerTransform = world.TryGet<Transform>(playerEntity_);
        if (!playerTransform) return;

        const float moveSpeed = 8.0f;

        // Aキーで左移動
        if (input.GetKey('A')) {
            playerTransform->position.x -= moveSpeed * deltaTime;
        }

        // Dキーで右移動
        if (input.GetKey('D')) {
            playerTransform->position.x += moveSpeed * deltaTime;
        }
    }

    /**
     * @brief 弾の発射処理
     * @param[in,out] world ワールド参照
     * @param[in] input 入力システム参照
     * @param[in] deltaTime デルタタイム（秒）
     */
    void UpdateShooting(World& world, InputSystem& input, float deltaTime) {
        shootCooldown_ -= deltaTime;

        // スペースキーで弾を発射(クールダウン中は発射できない)
        if (input.GetKey(VK_SPACE) && shootCooldown_ <= 0.0f) {
            auto* playerTransform = world.TryGet<Transform>(playerEntity_);
            if (playerTransform) {
                // プレイヤーの位置から弾を発射
                world.Create()
                    .With<Transform>(
                        DirectX::XMFLOAT3{playerTransform->position.x, playerTransform->position.y + 1.0f, 0.0f},
                        DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f},
                        DirectX::XMFLOAT3{0.3f, 0.5f, 0.3f}  // 小さめ
                    )
                    .With<MeshRenderer>(DirectX::XMFLOAT3{1.0f, 1.0f, 0.0f}) // 黄色
                    .With<Bullet>()
                    .With<BulletMovement>()
                    .Build();

                shootCooldown_ = 0.2f; // 0.2秒のクールダウン
            }
        }
    }

    /**
     * @brief 敵の生成処理
     * @param[in,out] world ワールド参照
     * @param[in] deltaTime デルタタイム（秒）
     */
    void UpdateEnemySpawning(World& world, float deltaTime) {
        enemySpawnTimer_ += deltaTime;

        // 1秒ごとに敵を生成
        if (enemySpawnTimer_ >= 1.0f) {
            enemySpawnTimer_ = 0.0f;

            // ランダムな位置に敵を配置
            float randomX = (rand() % 1600 - 800) / 100.0f; // -8.0 ~ 8.0

            world.Create()
                .With<Transform>(
                    DirectX::XMFLOAT3{randomX, 8.0f, 0.0f},  // 画面上部
                    DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f},
                    DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f}
                )
                .With<MeshRenderer>(DirectX::XMFLOAT3{1.0f, 0.0f, 0.0f}) // 赤色
                .With<Enemy>()
                .With<EnemyMovement>()
                .Build();
        }
    }

    /**
     * @brief 衝突判定処理
     * @param[in,out] world ワールド参照
     * 
     * @details
     * 弾と敵の衝突を判定し、衝突したら両方を削除してスコアを加算します。
     */
    void CheckCollisions(World& world) {
        // 削除する予定のエンティティリスト(イテレータ破壊を回避)
        std::vector<Entity> entitiesToDestroy;

        // 弾と敵の衝突をチェック
        world.ForEach<Bullet>([&](Entity bulletEntity, Bullet& bullet) {
            auto* bulletTransform = world.TryGet<Transform>(bulletEntity);
            if (!bulletTransform) return;

            // この弾が既に削除予定なら処理をスキップ
            for (const auto& e : entitiesToDestroy) {
                if (e.id == bulletEntity.id) return;
            }

            world.ForEach<Enemy>([&](Entity enemyEntity, Enemy& enemy) {
                auto* enemyTransform = world.TryGet<Transform>(enemyEntity);
                if (!enemyTransform) return;

                // この敵が既に削除予定なら処理をスキップ
                for (const auto& e : entitiesToDestroy) {
                    if (e.id == enemyEntity.id) return;
                }

                // 簡易的な距離判定(円の衝突)
                float dx = bulletTransform->position.x - enemyTransform->position.x;
                float dy = bulletTransform->position.y - enemyTransform->position.y;
                float distance = sqrtf(dx * dx + dy * dy);

                // 衝突したら削除リストに追加してスコア加算
                if (distance < 1.0f) {
                    entitiesToDestroy.push_back(bulletEntity);
                    entitiesToDestroy.push_back(enemyEntity);
                    score_ += 10;
                }
            });
        });

        // イテレーション後にまとめて削除
        for (const auto& entity : entitiesToDestroy) {
            world.DestroyEntity(entity);
        }
    }

    Entity playerEntity_;        ///< プレイヤーエンティティ
    int score_;                  ///< 現在のスコア
    float enemySpawnTimer_;      ///< 敵生成タイマー
    float shootCooldown_;        ///< 弾発射クールダウン
};
