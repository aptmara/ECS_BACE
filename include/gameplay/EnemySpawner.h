/**
 * @file EnemySpawner.h
 * @brief 敵スポーンシステム
 * @author 山内陽
 * @date 2025
 * @version 1.0
 * 
 * @details
 * ランダムな形状と色の敵を自動的にスポーンするシステムです。
 */
#pragma once

#include "pch.h"
#include "components/Component.h"
#include "components/Transform.h"
#include "components/MeshRenderer.h"
#include "components/Rotator.h"
#include "ecs/World.h"
#include <cstdlib>
#include <ctime>
#include <DirectXMath.h>

/**
 * @struct EnemyTag
 * @brief 敵を識別するタグコンポーネント
 * 
 * @details
 * エンティティが敵であることを示すマーカーです。
 * 衝突判定やForEach処理で敵を絞り込むために使用します。
 * 
 * @author 山内陽
 */
struct EnemyTag : IComponent {};

/**
 * @struct EnemyMovement
 * @brief 敵の移動Behaviour
 * 
 * @details
 * 敵を下方向に移動させ、画面外に出たら自動的に削除します。
 * 
 * @par 使用例
 * @code
 * Entity enemy = world.Create()
 *     .With<Transform>(DirectX::XMFLOAT3{0, 5, 0})
 *     .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
 *     .With<EnemyTag>()
 *     .With<EnemyMovement>()
 *     .Build();
 * @endcode
 * 
 * @author 山内陽
 */
struct EnemyMovement : Behaviour {
    float speed = 2.0f;  ///< 下方向への移動速度
    float destroyY = -10.0f;  ///< この高さ以下になったら削除
    
    /**
     * @brief 毎フレーム更新処理
     * @param[in,out] w ワールド参照
     * @param[in] self 自身のエンティティ
     * @param[in] dt デルタタイム(秒単位)
     */
    void OnUpdate(World& w, Entity self, float dt) override {
        auto* t = w.TryGet<Transform>(self);
        if (!t) return;
        
        // 下方向に移動
        t->position.y -= speed * dt;
        
        // 画面下に出たら削除
        if (t->position.y < destroyY) {
            w.DestroyEntityWithCause(self, World::Cause::LifetimeExpired);
        }
    }
};

/**
 * @struct EnemySpawner
 * @brief 敵を定期的にスポーンするBehaviour
 * 
 * @details
 * 指定された間隔でランダムな形状・色・位置の敵を生成します。
 * 
 * ### スポーンされる敵の特徴:
 * - ランダムな形状(Cube, Sphere, Cylinder, Cone, Capsule)
 * - ランダムな色
 * - ランダムなX座標
 * - ランダムな回転速度
 * 
 * @par 使用例
 * @code
 * // シーン初期化時に1つだけスポーナーを配置
 * Entity spawner = world.Create()
 *     .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
 *     .With<EnemySpawner>()
 *     .Build();
 * @endcode
 * 
 * @note このコンポーネントは1つのエンティティに1つだけ付けることを推奨
 * 
 * @author 山内陽
 */
struct EnemySpawner : Behaviour {
    float spawnInterval = 1.5f;  ///< スポーン間隔(秒)
    float timer = 0.0f;          ///< 内部タイマー
    float spawnY = 10.0f;        ///< スポーン位置のY座標
    float spawnRangeX = 8.0f;    ///< スポーン範囲の幅(-spawnRangeX ~ +spawnRangeX)
    
    /**
     * @brief 初回起動時の処理
     * @param[in,out] w ワールド参照
     * @param[in] self 自身のエンティティ
     */
    void OnStart(World& w, Entity self) override {
        // 乱数シードを初期化(最初の1回のみ)
        static bool initialized = false;
        if (!initialized) {
            srand(static_cast<unsigned int>(time(nullptr)));
            initialized = true;
        }
    }
    
    /**
     * @brief 毎フレーム更新処理
     * @param[in,out] w ワールド参照
     * @param[in] self 自身のエンティティ
     * @param[in] dt デルタタイム(秒単位)
     */
    void OnUpdate(World& w, Entity self, float dt) override {
        timer += dt;
        
        // スポーン間隔に達したら敵を生成
        if (timer >= spawnInterval) {
            timer = 0.0f;
            SpawnEnemy(w);
        }
    }
    
private:
    /**
     * @brief ランダムな敵を生成
     * @param[in,out] w ワールド参照
     */
    void SpawnEnemy(World& w) {
        // ランダムなX座標
        float randomX = ((rand() % 2000 - 1000) / 100.0f) * spawnRangeX / 10.0f;
        
        // ランダムな形状(0-4: Cube, Sphere, Cylinder, Cone, Capsule)
        // Planeは除外(地面用なので)
        int shapeIndex = rand() % 5;
        if (shapeIndex >= static_cast<int>(MeshType::Plane)) {
            shapeIndex++;  // Planeをスキップ
        }
        MeshType randomShape = static_cast<MeshType>(shapeIndex);
        
        // ランダムな色(明るめの色)
        DirectX::XMFLOAT3 randomColor = {
            (rand() % 100 + 50) / 150.0f,  // R: 0.33～1.0
            (rand() % 100 + 50) / 150.0f,  // G: 0.33～1.0
            (rand() % 100 + 50) / 150.0f   // B: 0.33～1.0
        };
        
        // ランダムな回転速度
        float randomRotSpeed = (rand() % 100 + 30) * (rand() % 2 == 0 ? 1.0f : -1.0f);
        
        // ランダムなスケール(0.8～1.5倍)
        float randomScale = 0.8f + (rand() % 70) / 100.0f;
        
        // Transform設定
        Transform enemyTransform;
        enemyTransform.position = DirectX::XMFLOAT3{randomX, spawnY, 0.0f};
        enemyTransform.rotation = DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f};
        enemyTransform.scale = DirectX::XMFLOAT3{randomScale, randomScale, randomScale};
        
        // MeshRenderer設定
        MeshRenderer enemyRenderer;
        enemyRenderer.meshType = randomShape;
        enemyRenderer.color = randomColor;
        
        // 敵エンティティを作成
        Entity enemy = w.Create()
            .With<Transform>(enemyTransform)
            .With<MeshRenderer>(enemyRenderer)
            .With<EnemyTag>()
            .WithCause<EnemyMovement>(World::Cause::Spawner)
            .WithCause<Rotator>(World::Cause::Spawner, randomRotSpeed)
            .Build();
    }
};

/**
 * @struct WaveSpawner
 * @brief ウェーブ形式で敵をスポーンするBehaviour
 * 
 * @details
 * 一定時間ごとに複数の敵を一度にスポーンします。
 * ウェーブゲームなどに使用できます。
 * 
 * @par 使用例
 * @code
 * Entity spawner = world.Create()
 *     .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
 *     .With<WaveSpawner>()
 *     .Build();
 * @endcode
 * 
 * @author 山内陽
 */
struct WaveSpawner : Behaviour {
    float waveInterval = 5.0f;   ///< ウェーブ間隔(秒)
    int enemiesPerWave = 5;      ///< 1ウェーブあたりの敵数
    float timer = 0.0f;          ///< 内部タイマー
    int currentWave = 0;         ///< 現在のウェーブ番号
    
    void OnStart(World& w, Entity self) override {
        static bool initialized = false;
        if (!initialized) {
            srand(static_cast<unsigned int>(time(nullptr)));
            initialized = true;
        }
    }
    
    void OnUpdate(World& w, Entity self, float dt) override {
        timer += dt;
        
        if (timer >= waveInterval) {
            timer = 0.0f;
            currentWave++;
            SpawnWave(w);
        }
    }
    
private:
    void SpawnWave(World& w) {
        for (int i = 0; i < enemiesPerWave; ++i) {
            // 横並びに配置
            float spacing = 2.5f;
            float startX = -(enemiesPerWave - 1) * spacing * 0.5f;
            float x = startX + i * spacing;
            
            // ランダムな形状
            int shapeIndex = rand() % 5;
            if (shapeIndex >= static_cast<int>(MeshType::Plane)) {
                shapeIndex++;
            }
            MeshType shape = static_cast<MeshType>(shapeIndex);
            
            // ウェーブごとに色のテーマを変える
            DirectX::XMFLOAT3 color;
            switch (currentWave % 3) {
                case 0: // 赤系
                    color = DirectX::XMFLOAT3{1.0f, 0.3f, 0.3f};
                    break;
                case 1: // 緑系
                    color = DirectX::XMFLOAT3{0.3f, 1.0f, 0.3f};
                    break;
                case 2: // 青系
                    color = DirectX::XMFLOAT3{0.3f, 0.3f, 1.0f};
                    break;
            }
            
            Transform t;
            t.position = DirectX::XMFLOAT3{x, 10.0f, 0.0f};
            t.scale = DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f};
            
            MeshRenderer mr;
            mr.meshType = shape;
            mr.color = color;
            
            Entity enemy = w.Create()
                .With<Transform>(t)
                .With<MeshRenderer>(mr)
                .With<EnemyTag>()
                .WithCause<EnemyMovement>(World::Cause::WaveTimer)
                .WithCause<Rotator>(World::Cause::WaveTimer, 60.0f)
                .Build();
        }
    }
};

