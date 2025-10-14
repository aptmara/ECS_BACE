#pragma once
// ========================================================
// MiniGame.h - シンプルなシューティングゲーム
// ========================================================
// 【ゲーム内容】
// - プレイヤー（緑キューブ）をA/Dキーで左右に移動
// - スペースキーで弾を発射
// - 敵（赤キューブ）が上から降ってくる
// - 弾が敵に当たると敵が消える
// - スコアを稼ごう！
// ========================================================

#include "SceneManager.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "Component.h"
#include <cstdlib>
#include <ctime>
#include <vector>

// ========================================================
// ゲーム用コンポーネント
// ========================================================

// プレイヤータグ
struct Player : IComponent {};

// 敵タグ
struct Enemy : IComponent {};

// 弾タグ
struct Bullet : IComponent {};

// プレイヤーの移動
struct PlayerMovement : Behaviour {
    float speed = 8.0f;
    
    void OnUpdate(World& w, Entity self, float dt) override {
        auto* t = w.TryGet<Transform>(self);
        if (!t) return;
        
        // キーボード入力はGameSceneで処理
        // ここでは位置の制限のみ
        if (t->position.x < -8.0f) t->position.x = -8.0f;
        if (t->position.x > 8.0f) t->position.x = 8.0f;
    }
};

// 弾の移動（上に進む）
struct BulletMovement : Behaviour {
    float speed = 15.0f;
    
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

// 敵の移動（下に進む）
struct EnemyMovement : Behaviour {
    float speed = 3.0f;
    
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
class GameScene : public IScene {
public:
    void OnEnter(World& world) override {
        // ランダムシード初期化
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
    
    void OnExit(World& world) override {
        // 全エンティティを削除（イテレータ無効化を防ぐ）
        std::vector<Entity> entitiesToDestroy;
        
        world.ForEach<Transform>([&](Entity e, Transform& t) {
            entitiesToDestroy.push_back(e);
        });
        
        for (const auto& entity : entitiesToDestroy) {
            world.DestroyEntity(entity);
        }
    }
    
    int GetScore() const { return score_; }

private:
    // プレイヤーの移動処理
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
    
    // 弾の発射処理
    void UpdateShooting(World& world, InputSystem& input, float deltaTime) {
        shootCooldown_ -= deltaTime;
        
        // スペースキーで弾を発射（クールダウン中は発射できない）
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
    
    // 敵の生成処理
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
    
    // 衝突判定
    void CheckCollisions(World& world) {
        // 削除するエンティティを収集（イテレータ無効化を防ぐ）
        std::vector<Entity> entitiesToDestroy;
        
        // 弾と敵の衝突をチェック
        world.ForEach<Bullet>([&](Entity bulletEntity, Bullet& bullet) {
            auto* bulletTransform = world.TryGet<Transform>(bulletEntity);
            if (!bulletTransform) return;
            
            // この弾がすでに削除予定なら処理をスキップ
            for (const auto& e : entitiesToDestroy) {
                if (e.id == bulletEntity.id) return;
            }
            
            world.ForEach<Enemy>([&](Entity enemyEntity, Enemy& enemy) {
                auto* enemyTransform = world.TryGet<Transform>(enemyEntity);
                if (!enemyTransform) return;
                
                // この敵がすでに削除予定なら処理をスキップ
                for (const auto& e : entitiesToDestroy) {
                    if (e.id == enemyEntity.id) return;
                }
                
                // 簡易的な距離判定（球の衝突）
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
        
        // イテレーション完了後に一括削除
        for (const auto& entity : entitiesToDestroy) {
            world.DestroyEntity(entity);
        }
    }

    Entity playerEntity_;
    int score_;
    float enemySpawnTimer_;
    float shootCooldown_;
};
