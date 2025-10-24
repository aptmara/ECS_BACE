/**
 * @file SampleScenes.h
 * @brief 学習用サンプルシーン集
 * @author 山内陽
 * @date 2025
 * @version 6.0
 * 
 * @details
 * 初学者がコンポーネント指向を段階的に学べるサンプル集です。
 * 使い方: 各関数をコピーして改造してみよう。
 */
#pragma once

#include "ecs/World.h"
#include "ecs/Entity.h"
#include "components/Transform.h"
#include "components/MeshRenderer.h"
#include "components/Rotator.h"
#include "animation/Animation.h"
#include "samples/ComponentSamples.h"
#include "gameplay/EnemySpawner.h"  // 敵スポーンシステムを追加
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
    // エンティティを作成(ビルダーパターン)
    Transform transform;
    transform.position = DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f};
    transform.rotation = DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f};
    transform.scale = DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f};

    MeshRenderer renderer;
    renderer.color = DirectX::XMFLOAT3{1.0f, 0.0f, 0.0f}; // 赤色

    Entity cube = world.Create()
        .With<Transform>(transform)
        .With<MeshRenderer>(renderer)
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
 * - Behaviourコンポーネント(Rotator)の使い方
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
    Transform transform;
    transform.position = position;
    transform.rotation = DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f};
    transform.scale = DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f};

    MeshRenderer renderer;
    renderer.color = DirectX::XMFLOAT3{0.0f, 1.0f, 0.0f}; // 緑色

    Entity cube = world.Create()
        .With<Transform>(transform)
        .With<MeshRenderer>(renderer)
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
    Transform transform;
    transform.position = DirectX::XMFLOAT3{-3.0f, 0.0f, 0.0f};
    transform.rotation = DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f};
    transform.scale = DirectX::XMFLOAT3{0.8f, 0.8f, 0.8f};

    MeshRenderer renderer;
    renderer.color = DirectX::XMFLOAT3{1.0f, 1.0f, 0.0f}; // 黄色

    Entity cube = world.Create()
        .With<Transform>(transform)
        .With<MeshRenderer>(renderer)
        .With<Bouncer>() // 上下に跳ねる(ComponentSamples.h参照)
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
    Transform transform;
    transform.position = DirectX::XMFLOAT3{3.0f, 0.0f, 0.0f};
    transform.rotation = DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f};
    transform.scale = DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f};

    MeshRenderer renderer;
    renderer.color = DirectX::XMFLOAT3{1.0f, 0.0f, 1.0f}; // マゼンタ

    Entity cube = world.Create()
        .With<Transform>(transform)
        .With<MeshRenderer>(renderer)
        .With<Rotator>(30.0f)    // 回転動作
        .With<PulseScale>()      // 大きさが変わる(ComponentSamples.h参照)
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
// レベル6: コンポーネントを後から変更
// ========================================================
/**
 * @brief レベル6: コンポーネントを後から変更
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
            // 位置を計算(中央を原点に)
            float x = (col - cols / 2.0f) * spacing;
            float z = (row - rows / 2.0f) * spacing;
            
            // 色を計算(位置によって変わる)
            float r = static_cast<float>(col) / static_cast<float>(cols - 1);
            float b = static_cast<float>(row) / static_cast<float>(rows - 1);
            
            // キューブを作成
            Transform transform;
            transform.position = DirectX::XMFLOAT3{x, 0.0f, z};
            transform.rotation = DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f};
            transform.scale = DirectX::XMFLOAT3{0.8f, 0.8f, 0.8f};

            MeshRenderer renderer;
            renderer.color = DirectX::XMFLOAT3{r, 0.5f, b};

            world.Create()
                .With<Transform>(transform)
                .With<MeshRenderer>(renderer)
                .With<Rotator>(45.0f + static_cast<float>(row * 10 + col * 5))
                .Build();
        }
    }
}

// ========================================================
// レベル9: 練習問題の解答例
// ========================================================

/**
 * @brief 練習: 虹色に回転するキューブを作成
 * 
 * @param[in,out] world ワールド参照
 * @return Entity 作成されたエンティティ
 * 
 * @author 山内陽
 */
inline Entity CreateRainbowCube(World& world) {
    Transform transform;
    transform.position = DirectX::XMFLOAT3{0.0f, 3.0f, 0.0f};
    transform.rotation = DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f};
    transform.scale = DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f};

    MeshRenderer renderer;
    renderer.color = DirectX::XMFLOAT3{1.0f, 0.0f, 0.0f};

    Entity cube = world.Create()
        .With<Transform>(transform)
        .With<MeshRenderer>(renderer)
        .With<Rotator>(120.0f) // 速く回転
        .With<ColorCycle>()    // 色が変わる(ComponentSamples.h)
        .Build();
    
    return cube;
}

/**
 * @brief 練習: ランダムに動き回るキューブ
 * 
 * @param[in,out] world ワールド参照
 * @return Entity 作成されたエンティティ
 * 
 * @author 山内陽
 */
inline Entity CreateWanderingCube(World& world) {
    Transform transform;
    transform.position = DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f};
    transform.rotation = DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f};
    transform.scale = DirectX::XMFLOAT3{0.6f, 0.6f, 0.6f};

    MeshRenderer renderer;
    renderer.color = DirectX::XMFLOAT3{0.8f, 0.3f, 0.9f};

    Entity cube = world.Create()
        .With<Transform>(transform)
        .With<MeshRenderer>(renderer)
        .With<RandomWalk>() // ランダム移動(ComponentSamples.h)
        .Build();
    
    return cube;
}

/**
 * @brief 練習: 時間経過で消えるキューブ
 * 
 * @param[in,out] world ワールド参照
 * @param[in] lifeTime 生存時間(秒単位)
 * @return Entity 作成されたエンティティ
 * 
 * @author 山内陽
 */
inline Entity CreateTemporaryCube(World& world, float lifeTime = 5.0f) {
    Transform transform;
    transform.position = DirectX::XMFLOAT3{0.0f, 5.0f, 0.0f};
    transform.rotation = DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f};
    transform.scale = DirectX::XMFLOAT3{0.5f, 0.5f, 0.5f};

    MeshRenderer renderer;
    renderer.color = DirectX::XMFLOAT3{1.0f, 0.5f, 0.0f};

    Entity cube = world.Create()
        .With<Transform>(transform)
        .With<MeshRenderer>(renderer)
        .With<Rotator>(200.0f)
        .Build();
    
    // 寿命コンポーネントを追加
    LifeTime lt;
    lt.remainingTime = lifeTime;
    world.Add<LifeTime>(cube, lt);
    
    return cube;
}

// ========================================================
// レベル10: 様々な形状を使う
// ========================================================
/**
 * @brief レベル10: 様々な形状を使う
 * @details
 * 学べること:
 * - MeshTypeを使って形状を指定
 * - 球体、円柱、円錐、平面、カプセルの使い方
 */

/**
 * @brief 赤い球体を作成
 * 
 * @param[in,out] world ワールド参照
 * @param[in] position 配置する位置
 * @return Entity 作成されたエンティティ
 * 
 * @author 山内陽
 */
inline Entity CreateSphere(World& world, const DirectX::XMFLOAT3& position) {
    Transform transform;
    transform.position = position;
    transform.rotation = DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f};
    transform.scale = DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f};

    MeshRenderer renderer;
    renderer.meshType = MeshType::Sphere;  // 球体を指定
    renderer.color = DirectX::XMFLOAT3{1.0f, 0.3f, 0.3f};  // 赤系

    return world.Create()
        .With<Transform>(transform)
        .With<MeshRenderer>(renderer)
        .Build();
}

/**
 * @brief 緑の円柱を作成
 * 
 * @param[in,out] world ワールド参照
 * @param[in] position 配置する位置
 * @return Entity 作成されたエンティティ
 * 
 * @author 山内陽
 */
inline Entity CreateCylinder(World& world, const DirectX::XMFLOAT3& position) {
    Transform transform;
    transform.position = position;
    transform.rotation = DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f};
    transform.scale = DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f};

    MeshRenderer renderer;
    renderer.meshType = MeshType::Cylinder;  // 円柱を指定
    renderer.color = DirectX::XMFLOAT3{0.3f, 1.0f, 0.3f};  // 緑系

    return world.Create()
        .With<Transform>(transform)
        .With<MeshRenderer>(renderer)
        .Build();
}

/**
 * @brief 青い円錐を作成
 * 
 * @param[in,out] world ワールド参照
 * @param[in] position 配置する位置
 * @return Entity 作成されたエンティティ
 * 
 * @author 山内陽
 */
inline Entity CreateCone(World& world, const DirectX::XMFLOAT3& position) {
    Transform transform;
    transform.position = position;
    transform.rotation = DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f};
    transform.scale = DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f};

    MeshRenderer renderer;
    renderer.meshType = MeshType::Cone;  // 円錐を指定
    renderer.color = DirectX::XMFLOAT3{0.3f, 0.3f, 1.0f};  // 青系

    return world.Create()
        .With<Transform>(transform)
        .With<MeshRenderer>(renderer)
        .Build();
}

/**
 * @brief 灰色の平面(地面)を作成
 * 
 * @param[in,out] world ワールド参照
 * @param[in] position 配置する位置
 * @param[in] scale スケール(大きさ)
 * @return Entity 作成されたエンティティ
 * 
 * @author 山内陽
 */
inline Entity CreatePlane(World& world, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& scale) {
    Transform transform;
    transform.position = position;
    transform.rotation = DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f};
    transform.scale = scale;

    MeshRenderer renderer;
    renderer.meshType = MeshType::Plane;  // 平面を指定
    renderer.color = DirectX::XMFLOAT3{0.6f, 0.6f, 0.6f};  // 灰色

    return world.Create()
        .With<Transform>(transform)
        .With<MeshRenderer>(renderer)
        .Build();
}

/**
 * @brief 紫のカプセルを作成
 * 
 * @param[in,out] world ワールド参照
 * @param[in] position 配置する位置
 * @return Entity 作成されたエンティティ
 * 
 * @author 山内陽
 */
inline Entity CreateCapsule(World& world, const DirectX::XMFLOAT3& position) {
    Transform transform;
    transform.position = position;
    transform.rotation = DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f};
    transform.scale = DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f};

    MeshRenderer renderer;
    renderer.meshType = MeshType::Capsule;  // カプセルを指定
    renderer.color = DirectX::XMFLOAT3{0.8f, 0.3f, 0.8f};  // 紫系

    return world.Create()
        .With<Transform>(transform)
        .With<MeshRenderer>(renderer)
        .Build();
}

/**
 * @brief 様々な形状を回転させながら配置するデモ
 * 
 * @param[in,out] world ワールド参照
 * 
 * @author 山内陽
 */
inline void CreateShapeShowcase(World& world) {
    const float spacing = 3.0f;
    
    // 地面(大きな平面)
    CreatePlane(world, DirectX::XMFLOAT3{0.0f, -1.5f, 0.0f}, DirectX::XMFLOAT3{20.0f, 1.0f, 20.0f});
    
    // 中央に立方体
    Entity cube = world.Create()
        .With<Transform>(DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f})
        .With<MeshRenderer>(DirectX::XMFLOAT3{1.0f, 1.0f, 0.3f})  // 黄色
        .With<Rotator>(30.0f)
        .Build();
    
    // 左に球体
    Entity sphere = CreateSphere(world, DirectX::XMFLOAT3{-spacing, 0.0f, 0.0f});
    world.Add<Rotator>(sphere, Rotator{45.0f});
    
    // 右に円柱
    Entity cylinder = CreateCylinder(world, DirectX::XMFLOAT3{spacing, 0.0f, 0.0f});
    world.Add<Rotator>(cylinder, Rotator{60.0f});
    
    // 手前に円錐
    Entity cone = CreateCone(world, DirectX::XMFLOAT3{0.0f, 0.0f, -spacing});
    world.Add<Rotator>(cone, Rotator{75.0f});
    
    // 奥にカプセル
    Entity capsule = CreateCapsule(world, DirectX::XMFLOAT3{0.0f, 0.0f, spacing});
    world.Add<Rotator>(capsule, Rotator{90.0f});
}

// ========================================================
// レベル11: 敵スポーンシステムを使う
// ========================================================
/**
 * @brief レベル11: 敵スポーンシステムを使う
 * @details
 * 学べること:
 * - 自動的に敵を生成するシステムの作り方
 * - ランダムな形状・色の敵を生成
 * - ゲームループの基本構造
 */

/**
 * @brief ランダム敵スポーンシステムのデモ
 * 
 * @param[in,out] world ワールド参照
 * 
 * @details
 * 1.5秒ごとにランダムな形状・色の敵が上から降ってきます。
 * 
 * @author 山内陽
 */
inline void CreateEnemySpawnerDemo(World& world) {
    // 地面を作成
    Transform groundTransform;
    groundTransform.position = DirectX::XMFLOAT3{0.0f, -2.0f, 0.0f};
    groundTransform.scale = DirectX::XMFLOAT3{30.0f, 1.0f, 30.0f};
    
    MeshRenderer groundRenderer;
    groundRenderer.meshType = MeshType::Plane;
    groundRenderer.color = DirectX::XMFLOAT3{0.2f, 0.6f, 0.2f};  // 草原風
    
    world.Create()
        .With<Transform>(groundTransform)
        .With<MeshRenderer>(groundRenderer)
        .Build();
    
    // スポーナーエンティティを作成(見えない管理用エンティティ)
    Entity spawner = world.Create()
        .With<Transform>(DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f})
        .With<EnemySpawner>()
        .Build();
}

/**
 * @brief ウェーブ形式の敵スポーンデモ
 * 
 * @param[in,out] world ワールド参照
 * 
 * @details
 * 5秒ごとに5体の敵が横並びで出現します。
 * ウェーブごとに色のテーマが変わります。
 * 
 * @author 山内陽
 */
inline void CreateWaveSpawnerDemo(World& world) {
    // 地面
    Transform groundTransform;
    groundTransform.position = DirectX::XMFLOAT3{0.0f, -2.0f, 0.0f};
    groundTransform.scale = DirectX::XMFLOAT3{30.0f, 1.0f, 30.0f};
    
    MeshRenderer groundRenderer;
    groundRenderer.meshType = MeshType::Plane;
    groundRenderer.color = DirectX::XMFLOAT3{0.3f, 0.3f, 0.4f};  // 暗い地面
    
    world.Create()
        .With<Transform>(groundTransform)
        .With<MeshRenderer>(groundRenderer)
        .Build();
    
    // ウェーブスポーナー
    Entity spawner = world.Create()
        .With<Transform>(DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f})
        .With<WaveSpawner>()
        .Build();
}

/**
 * @brief カスタマイズ可能な敵スポーナーの作成例
 * 
 * @param[in,out] world ワールド参照
 * @param[in] interval スポーン間隔(秒)
 * @param[in] spawnY スポーン位置のY座標
 * @return Entity スポーナーエンティティ
 * 
 * @details
 * パラメータを調整して、スポーン頻度や位置を変更できます。
 * 
 * @author 山内陽
 */
inline Entity CreateCustomEnemySpawner(World& world, float interval = 2.0f, float spawnY = 12.0f) {
    Entity spawner = world.Create()
        .With<Transform>(DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f})
        .Build();
    
    // スポーナーコンポーネントを手動で設定
    auto& spawnerComp = world.Add<EnemySpawner>(spawner);
    spawnerComp.spawnInterval = interval;
    spawnerComp.spawnY = spawnY;
    spawnerComp.spawnRangeX = 10.0f;
    
    return spawner;
}

/**
 * @brief 複数のスポーナーを配置するデモ
 * 
 * @param[in,out] world ワールド参照
 * 
 * @details
 * 異なる速度で動く複数のスポーナーを配置します。
 * より複雑な敵の出現パターンを作れます。
 * 
 * @author 山内陽
 */
inline void CreateMultiSpawnerDemo(World& world) {
    // 地面
    Transform groundTransform;
    groundTransform.position = DirectX::XMFLOAT3{0.0f, -2.0f, 0.0f};
    groundTransform.scale = DirectX::XMFLOAT3{30.0f, 1.0f, 30.0f};
    
    MeshRenderer groundRenderer;
    groundRenderer.meshType = MeshType::Plane;
    groundRenderer.color = DirectX::XMFLOAT3{0.4f, 0.4f, 0.3f};
    
    world.Create()
        .With<Transform>(groundTransform)
        .With<MeshRenderer>(groundRenderer)
        .Build();
    
    // 速いスポーナー(1秒ごと)
    CreateCustomEnemySpawner(world, 1.0f, 12.0f);
    
    // 遅いスポーナー(3秒ごと)
    CreateCustomEnemySpawner(world, 3.0f, 15.0f);
}

} // namespace SampleScenes

// ========================================================
// 使い方の例
// ========================================================
/*

// App.hのCreateDemoScene()で使う例

void CreateDemoScene() {
    // シンプルなキューブ
    SampleScenes::CreateSimpleCube(world_);
    
    // 回転するキューブ(位置を指定)
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
    
    // 新しい形状のデモ
    SampleScenes::CreateShapeShowcase(world_);
    
    // 個別の形状作成
    SampleScenes::CreateSphere(world_, DirectX::XMFLOAT3{5, 0, 0});
    SampleScenes::CreateCylinder(world_, DirectX::XMFLOAT3{-5, 0, 0});
    SampleScenes::CreateCone(world_, DirectX::XMFLOAT3{0, 3, 0});
    SampleScenes::CreatePlane(world_, DirectX::XMFLOAT3{0, -2, 0}, DirectX::XMFLOAT3{10, 1, 10});
    SampleScenes::CreateCapsule(world_, DirectX::XMFLOAT3{0, -3, 0});
    
    // 敵スポーンシステムのデモ
    SampleScenes::CreateEnemySpawnerDemo(world_);
    
    // または、ウェーブ形式
    // SampleScenes::CreateWaveSpawnerDemo(world_);
    
    // または、複数スポーナー
    // SampleScenes::CreateMultiSpawnerDemo(world_);
    
    // または、カスタム設定
    // SampleScenes::CreateCustomEnemySpawner(world_, 0.5f, 15.0f);
}

*/

// ========================================================
// 作成者: 山内陽
// バージョン: v7.0 - 敵スポーンシステム対応
// ========================================================
