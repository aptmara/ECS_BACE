/**
 * @file SampleScenes.h
 * @brief 学習用サンプルシーン集
 * @author 山内陽
 * @date 2025
 * @version 4.0
 * 
 * @details
 * 初学者がコンポーネント指向を段階的に学べるサンプル集です。
 * 使い方：各関数をコピーして改造してみよう！
 */
#pragma once

#include "ecs/World.h"
#include "ecs/Entity.h"
#include "components/Transform.h"
#include "components/MeshRenderer.h"
#include "components/Rotator.h"
#include "animation/Animation.h"
#include "samples/ComponentSamples.h"
#include <DirectXMath.h>

namespace SampleScenes {

// ========================================================
// レベル1: 最もシンプルなエンティティ
// ========================================================
/**
 * @brief レベル1: 最もシンプルなエンティティ
 * @details
 * 学べること:
 * - エンティティの作成方法
 * - Transformコンポーネントの設定
 * - MeshRendererで色を付ける
 */

/**
 * @brief シンプルな赤いキューブを作成
 * 
 * @param[in,out] world ワールド参照
 * @return Entity 作成されたエンティティ
 * 
 * @author 山内陽
 */
inline Entity CreateSimpleCube(World& world) {
    // エンティティを作成（ビルダーパターン）
    Entity cube = world.Create()
        .With<Transform>(
            DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f},  // 位置
            DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f},  // 回転
            DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f}   // スケール
        )
        .With<MeshRenderer>(DirectX::XMFLOAT3{1.0f, 0.0f, 0.0f}) // 赤色
        .Build();
    
    return cube;
}

// ========================================================
// レベル2: 動きのあるエンティティ
// ========================================================
/**
 * @brief レベル2: 動きのあるエンティティ
 * @details
 * 学べること:
 * - Behaviourコンポーネント（Rotator）の使い方
 * - コンポーネントの組み合わせ
 */

/**
 * @brief 回転する緑のキューブを作成
 * 
 * @param[in,out] world ワールド参照
 * @param[in] position 配置する位置
 * @return Entity 作成されたエンティティ
 * 
 * @author 山内陽
 */
inline Entity CreateRotatingCube(World& world, const DirectX::XMFLOAT3& position) {
    Entity cube = world.Create()
        .With<Transform>(
            position,
            DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f},
            DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f}
        )
        .With<MeshRenderer>(DirectX::XMFLOAT3{0.0f, 1.0f, 0.0f}) // 緑色
        .With<Rotator>(45.0f) // 毎秒45度回転
        .Build();
    
    return cube;
}

// ========================================================
// レベル3: カスタムBehaviourを使う
// ========================================================
/**
 * @brief レベル3: カスタムBehaviourを使う
 * @details
 * 学べること:
 * - ComponentSamples.hのカスタムBehaviourを使う
 * - 複数のコンポーネントを組み合わせる
 */

/**
 * @brief 上下に跳ねる黄色のキューブを作成
 * 
 * @param[in,out] world ワールド参照
 * @return Entity 作成されたエンティティ
 * 
 * @author 山内陽
 */
inline Entity CreateBouncingCube(World& world) {
    Entity cube = world.Create()
        .With<Transform>(
            DirectX::XMFLOAT3{-3.0f, 0.0f, 0.0f},
            DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f},
            DirectX::XMFLOAT3{0.8f, 0.8f, 0.8f}
        )
        .With<MeshRenderer>(DirectX::XMFLOAT3{1.0f, 1.0f, 0.0f}) // 黄色
        .With<Bouncer>() // 上下に跳ねる（ComponentSamples.h参照）
        .Build();
    
    return cube;
}

// ========================================================
// レベル4: 複数のBehaviourを組み合わせる
// ========================================================
/**
 * @brief レベル4: 複数のBehaviourを組み合わせる
 * @details
 * 学べること:
 * - 1つのエンティティに複数のBehaviourを追加
 * - それぞれが独立して動作する
 */

/**
 * @brief 回転しながらサイズが変わるマゼンタのキューブを作成
 * 
 * @param[in,out] world ワールド参照
 * @return Entity 作成されたエンティティ
 * 
 * @author 山内陽
 */
inline Entity CreateComplexCube(World& world) {
    Entity cube = world.Create()
        .With<Transform>(
            DirectX::XMFLOAT3{3.0f, 0.0f, 0.0f},
            DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f},
            DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f}
        )
        .With<MeshRenderer>(DirectX::XMFLOAT3{1.0f, 0.0f, 1.0f}) // マゼンタ
        .With<Rotator>(30.0f)    // 回転動作
        .With<PulseScale>()      // 大きさが変わる（ComponentSamples.h参照）
        .Build();
    
    return cube;
}

// ========================================================
// レベル5: 従来の方法でエンティティを作成
// ========================================================
/**
 * @brief レベル5: 従来の方法でエンティティを作成
 * @details
 * 学べること:
 * - ビルダーパターンを使わない方法
 * - 手動でコンポーネントを追加する方法
 */

/**
 * @brief 従来の方法でシアンのキューブを作成
 * 
 * @param[in,out] world ワールド参照
 * @return Entity 作成されたエンティティ
 * 
 * @author 山内陽
 */
inline Entity CreateCubeOldStyle(World& world) {
    // ステップ1: エンティティを作成
    Entity cube = world.CreateEntity();
    
    // ステップ2: Transformコンポーネントを追加
    Transform transform;
    transform.position = DirectX::XMFLOAT3{0.0f, -2.0f, 0.0f};
    transform.rotation = DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f};
    transform.scale = DirectX::XMFLOAT3{0.5f, 0.5f, 0.5f};
    world.Add<Transform>(cube, transform);
    
    // ステップ3: MeshRendererコンポーネントを追加
    MeshRenderer renderer;
    renderer.color = DirectX::XMFLOAT3{0.0f, 1.0f, 1.0f}; // シアン
    world.Add<MeshRenderer>(cube, renderer);
    
    // ステップ4: Rotatorコンポーネントを追加
    Rotator rotator;
    rotator.speedDegY = 90.0f;
    world.Add<Rotator>(cube, rotator);
    
    return cube;
}

// ========================================================
// レベル6: コンポーネントの後からの変更
// ========================================================
/**
 * @brief レベル6: コンポーネントの後からの変更
 * @details
 * 学べること:
 * - TryGetで取得して値を変更
 * - コンポーネントの動的な操作
 */

/**
 * @brief エンティティのコンポーネントを変更する例
 * 
 * @param[in,out] world ワールド参照
 * @param[in] entity 変更対象のエンティティ
 * 
 * @author 山内陽
 */
inline void ModifyEntityExample(World& world, Entity entity) {
    // Transformを取得して変更
    auto* transform = world.TryGet<Transform>(entity);
    if (transform) {
        transform->position.y += 1.0f; // Y座標を1上げる
        transform->scale = DirectX::XMFLOAT3{2.0f, 2.0f, 2.0f}; // 2倍の大きさに
    }
    
    // MeshRendererを取得して色を変更
    auto* renderer = world.TryGet<MeshRenderer>(entity);
    if (renderer) {
        renderer->color = DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f}; // 白に変更
    }
    
    // Rotatorを取得して速度を変更
    auto* rotator = world.TryGet<Rotator>(entity);
    if (rotator) {
        rotator->speedDegY = 180.0f; // 速度を2倍に
    }
}

// ========================================================
// レベル7: 全エンティティに対する処理
// ========================================================
/**
 * @brief レベル7: 全エンティティに対する処理
 * @details
 * 学べること:
 * - ForEachで全エンティティを巡回
 * - ラムダ式の使い方
 */

/**
 * @brief 全Transformを持つエンティティを少しずつ上に移動
 * 
 * @param[in,out] world ワールド参照
 * 
 * @author 山内陽
 */
inline void ProcessAllTransforms(World& world) {
    // 全てのTransformを持つエンティティに対して処理
    world.ForEach<Transform>([](Entity entity, Transform& transform) {
        // 全てのエンティティを少しずつ上に移動
        transform.position.y += 0.01f;
    });
}

/**
 * @brief 全MeshRendererの色を変更
 * 
 * @param[in,out] world ワールド参照
 * 
 * @author 山内陽
 */
inline void ChangeAllColors(World& world) {
    // 全てのMeshRendererの色を変更
    world.ForEach<MeshRenderer>([](Entity entity, MeshRenderer& renderer) {
        // 全てのエンティティを赤っぽくする
        renderer.color.x = 1.0f; // R成分を最大
    });
}

// ========================================================
// レベル8: デモシーン作成
// ========================================================
/**
 * @brief レベル8: デモシーン作成
 * @details
 * 学べること:
 * - 複数のエンティティを配置してシーンを構成
 * - 位置を計算してグリッド状に配置
 */

/**
 * @brief グリッド状にキューブを配置
 * 
 * @param[in,out] world ワールド参照
 * @param[in] rows 行数
 * @param[in] cols 列数
 * 
 * @author 山内陽
 */
inline void CreateGridOfCubes(World& world, int rows = 3, int cols = 3) {
    const float spacing = 2.5f; // キューブ間の距離
    
    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            // 位置を計算（中心を原点に）
            float x = (col - cols / 2.0f) * spacing;
            float z = (row - rows / 2.0f) * spacing;
            
            // 色を計算（位置によって変わる）
            float r = static_cast<float>(col) / static_cast<float>(cols - 1);
            float b = static_cast<float>(row) / static_cast<float>(rows - 1);
            
            // キューブを作成
            world.Create()
                .With<Transform>(
                    DirectX::XMFLOAT3{x, 0.0f, z},
                    DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f},
                    DirectX::XMFLOAT3{0.8f, 0.8f, 0.8f}
                )
                .With<MeshRenderer>(DirectX::XMFLOAT3{r, 0.5f, b})
                .With<Rotator>(45.0f + static_cast<float>(row * 10 + col * 5))
                .Build();
        }
    }
}

// ========================================================
// レベル9: 練習問題の解答例
// ========================================================

/**
 * @brief 練習1: 虹色に回転するキューブを作成
 * 
 * @param[in,out] world ワールド参照
 * @return Entity 作成されたエンティティ
 * 
 * @author 山内陽
 */
inline Entity CreateRainbowCube(World& world) {
    Entity cube = world.Create()
        .With<Transform>(
            DirectX::XMFLOAT3{0.0f, 3.0f, 0.0f},
            DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f},
            DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f}
        )
        .With<MeshRenderer>(DirectX::XMFLOAT3{1.0f, 0.0f, 0.0f})
        .With<Rotator>(120.0f) // 速く回転
        .With<ColorCycle>()    // 色が変わる（ComponentSamples.h）
        .Build();
    
    return cube;
}

/**
 * @brief 練習2: ランダムに動き回るキューブ
 * 
 * @param[in,out] world ワールド参照
 * @return Entity 作成されたエンティティ
 * 
 * @author 山内陽
 */
inline Entity CreateWanderingCube(World& world) {
    Entity cube = world.Create()
        .With<Transform>(
            DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f},
            DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f},
            DirectX::XMFLOAT3{0.6f, 0.6f, 0.6f}
        )
        .With<MeshRenderer>(DirectX::XMFLOAT3{0.8f, 0.3f, 0.9f})
        .With<RandomWalk>() // ランダム移動（ComponentSamples.h）
        .Build();
    
    return cube;
}

/**
 * @brief 練習3: 時間経過で消えるキューブ
 * 
 * @param[in,out] world ワールド参照
 * @param[in] lifeTime 生存時間（秒）
 * @return Entity 作成されたエンティティ
 * 
 * @author 山内陽
 */
inline Entity CreateTemporaryCube(World& world, float lifeTime = 5.0f) {
    Entity cube = world.Create()
        .With<Transform>(
            DirectX::XMFLOAT3{0.0f, 5.0f, 0.0f},
            DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f},
            DirectX::XMFLOAT3{0.5f, 0.5f, 0.5f}
        )
        .With<MeshRenderer>(DirectX::XMFLOAT3{1.0f, 0.5f, 0.0f})
        .With<Rotator>(200.0f)
        .Build();
    
    // 寿命コンポーネントを追加
    LifeTime lt;
    lt.remainingTime = lifeTime;
    world.Add<LifeTime>(cube, lt);
    
    return cube;
}

} // namespace SampleScenes

// ========================================================
// 使い方の例
// ========================================================
/*

// App.hのCreateDemoScene()で使う場合:

void CreateDemoScene() {
    // シンプルなキューブ
    SampleScenes::CreateSimpleCube(world_);
    
    // 回転するキューブ（位置を指定）
    SampleScenes::CreateRotatingCube(world_, DirectX::XMFLOAT3{-3, 0, 0});
    
    // 上下に跳ねるキューブ
    SampleScenes::CreateBouncingCube(world_);
    
    // 複雑な動きのキューブ
    SampleScenes::CreateComplexCube(world_);
    
    // 3x3のグリッド
    SampleScenes::CreateGridOfCubes(world_, 3, 3);
    
    // 練習問題の解答例
    SampleScenes::CreateRainbowCube(world_);
    SampleScenes::CreateWanderingCube(world_);
    SampleScenes::CreateTemporaryCube(world_, 10.0f);
}

*/

// ========================================================
// 作成者: 山内陽
// バージョン: v4.0 - 段階的学習用サンプル集
// ========================================================
