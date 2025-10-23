/**
 * @file WorldImprovementDemo.h
 * @brief World改善機能のデモンストレーション
 * @author 山内陽
 * @date 2025
 * @version 1.0
 *
 * @details
 * World v5.0の新機能（Has, Get, ForEach<T1,T2>など）の使用例を提供します。
 */
#pragma once

#include "ecs/World.h"
#include "components/Transform.h"
#include "components/MeshRenderer.h"
#include "samples/ComponentSamples.h"
#include <DirectXMath.h>

namespace WorldImprovementDemo {

// ========================================================
// 新機能1: Has() - コンポーネントの存在確認
// ========================================================

/**
 * @brief Has()メソッドの使用例
 * 
 * @details
 * Has()を使うことで、コンポーネントの存在チェックが明示的になります。
 */
inline void DemoHasMethod(World& world) {
    Entity entity = world.Create()
        .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
        .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
        .Build();
    
    // ? 新しい方法: Has()で明示的にチェック
    if (world.Has<Transform>(entity)) {
        auto* transform = world.TryGet<Transform>(entity);
        transform->position.x += 1.0f;
    }
    
    // ? 複数コンポーネントのチェック
    if (world.Has<Transform>(entity) && world.Has<MeshRenderer>(entity)) {
        // 両方持っている場合の処理
        printf("Entity has both Transform and MeshRenderer\n");
    }
    
    // ? 古い方法（動作はするが意図が不明確）
    auto* transform = world.TryGet<Transform>(entity);
    if (transform) {
        transform->position.x += 1.0f;
    }
}

// ========================================================
// 新機能2: Get() - 例外版の取得
// ========================================================

/**
 * @brief Get()メソッドの使用例
 * 
 * @details
 * 必ず存在するコンポーネントには Get() を使うことで
 * nullptrチェックが不要になります。
 */
inline void DemoGetMethod(World& world) {
    Entity player = world.Create()
        .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
        .With<MeshRenderer>(DirectX::XMFLOAT3{0, 1, 0})
        .Build();
    
    // ? 新しい方法: Get()でnullptrチェック不要
    try {
        Transform& transform = world.Get<Transform>(player);
        transform.position.x += 1.0f;
        
        MeshRenderer& renderer = world.Get<MeshRenderer>(player);
        renderer.color = DirectX::XMFLOAT3{1, 1, 1};
        
    } catch (const std::runtime_error& e) {
        printf("Error: %s\n", e.what());
    }
    
    // ? 古い方法（冗長）
    auto* transform = world.TryGet<Transform>(player);
    if (transform) {
        transform->position.x += 1.0f;
    }
    auto* renderer = world.TryGet<MeshRenderer>(player);
    if (renderer) {
        renderer->color = DirectX::XMFLOAT3{1, 1, 1};
    }
}

// ========================================================
// 新機能3: ForEach<T1, T2> - 複数コンポーネントクエリ
// ========================================================

/**
 * @brief 2つのコンポーネントクエリの使用例
 * 
 * @details
 * 複数のコンポーネントを持つエンティティに対して効率的に処理できます。
 */
inline void DemoForEachTwoComponents(World& world) {
    // テストデータ作成
    for (int i = 0; i < 5; ++i) {
        world.Create()
            .With<Transform>(DirectX::XMFLOAT3{static_cast<float>(i), 0, 0})
            .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
            .Build();
    }
    
    // ? 新しい方法: 2つのコンポーネントを同時に処理
    world.ForEach<Transform, MeshRenderer>(
        [](Entity e, Transform& t, MeshRenderer& r) {
            // 位置に応じて色を変える
            r.color.x = t.position.x / 10.0f;
            r.color.y = 1.0f - (t.position.x / 10.0f);
            r.color.z = 0.5f;
        }
    );
    
    // ? 古い方法（冗長で非効率）
    world.ForEach<Transform>([&](Entity e, Transform& t) {
        auto* renderer = world.TryGet<MeshRenderer>(e);
        if (renderer) {
            renderer->color.x = t.position.x / 10.0f;
        }
    });
}

// ========================================================
// 新機能4: 物理演算の例（Transform + Velocity）
// ========================================================

/**
 * @brief 物理演算システムの例
 * 
 * @details
 * TransformとVelocityを持つエンティティを自動的に移動させます。
 */
inline void DemoPhysicsSystem(World& world, float dt) {
    // Velocityコンポーネントを持つオブジェクトを作成
    Entity movingObject = world.Create()
        .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
        .With<Velocity>()
        .With<MeshRenderer>(DirectX::XMFLOAT3{0, 1, 1})
        .Build();
    
    // 速度を設定
    world.Get<Velocity>(movingObject).velocity = DirectX::XMFLOAT3{5, 0, 0};
    
    // ? 物理演算: TransformとVelocityを持つ全エンティティを移動
    world.ForEach<Transform, Velocity>(
        [dt](Entity e, Transform& t, Velocity& v) {
            t.position.x += v.velocity.x * dt;
            t.position.y += v.velocity.y * dt;
            t.position.z += v.velocity.z * dt;
        }
    );
}

// ========================================================
// 新機能5: GetEntityCount / GetComponentCount
// ========================================================

/**
 * @brief デバッグ情報取得の例
 * 
 * @details
 * エンティティ数やコンポーネント数を取得してパフォーマンス分析できます。
 */
inline void DemoDebugInfo(World& world) {
    // エンティティ数を取得
    printf("Total entities: %zu\n", world.GetEntityCount());
    
    // 特定コンポーネントの数を取得
    printf("Entities with Transform: %zu\n", world.GetComponentCount<Transform>());
    printf("Entities with MeshRenderer: %zu\n", world.GetComponentCount<MeshRenderer>());
    printf("Entities with Enemy: %zu\n", world.GetComponentCount<EnemyTag>());
    
    // エンティティ数の制限チェック
    if (world.GetEntityCount() < 1000) {
        printf("Safe to spawn more entities\n");
    } else {
        printf("Warning: Too many entities!\n");
    }
    
#ifdef _DEBUG
    // デバッグビルドでは詳細情報も表示
    world.PrintDebugInfo();
#endif
}

// ========================================================
// 新機能6: Reserve - 事前確保でパフォーマンス向上
// ========================================================

/**
 * @brief Reserve()の使用例
 * 
 * @details
 * 大量のエンティティを生成する前に事前確保することで
 * メモリ再確保のオーバーヘッドを削減します。
 */
inline void DemoReserve(World& world) {
    // ? 100体の敵を生成する前に事前確保
    world.Reserve(100);
    
    for (int i = 0; i < 100; ++i) {
        world.Create()
            .With<Transform>(DirectX::XMFLOAT3{
                static_cast<float>(i % 10) * 2.0f,
                0,
                static_cast<float>(i / 10) * 2.0f
            })
            .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
            .With<EnemyTag>()
            .Build();
    }
    
    printf("Created 100 enemies efficiently!\n");
}

// ========================================================
// 新機能7: ID再利用のデモ
// ========================================================

/**
 * @brief ID再利用の動作確認
 * 
 * @details
 * エンティティを削除するとIDが再利用プールに入り、
 * 次回のCreateEntity()で再利用されます。
 */
inline void DemoIDReuse(World& world) {
    printf("=== ID Reuse Demo ===\n");
    
    // エンティティを3つ作成
    Entity e1 = world.CreateEntity();
    Entity e2 = world.CreateEntity();
    Entity e3 = world.CreateEntity();
    
    printf("Created: ID=%u, %u, %u\n", e1.id, e2.id, e3.id);
    
    // 2番目を削除
    world.DestroyEntity(e2);
    printf("Deleted: ID=%u\n", e2.id);
    
    // 新しいエンティティを作成（削除されたIDが再利用される）
    Entity e4 = world.CreateEntity();
    printf("Created: ID=%u (reused!)\n", e4.id);
    
    printf("=====================\n");
}

// ========================================================
// 実践例: 体力システムと自動削除
// ========================================================

/**
 * @brief 体力システムの実践例
 * 
 * @details
 * HealthとEnemyを持つエンティティを管理し、
 * 体力が0になったら自動削除します。
 */
inline void DemoHealthSystem(World& world) {
    // 敵を5体作成
    for (int i = 0; i < 5; ++i) {
        Entity enemy = world.Create()
            .With<Transform>(DirectX::XMFLOAT3{static_cast<float>(i) * 2.0f, 0, 0})
            .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
            .With<EnemyTag>()
            .Build();
        
        // Healthを追加
        Health hp;
        hp.current = 100.0f;
        hp.max = 100.0f;
        world.Add<Health>(enemy, hp);
    }
    
    printf("Created %zu enemies with health\n", world.GetComponentCount<EnemyTag>());
    
    // ? 全ての敵にダメージを与える
    world.ForEach<EnemyTag, Health>([](Entity e, EnemyTag&, Health& hp) {
        hp.TakeDamage(50.0f);
        printf("Enemy %u: HP = %.1f\n", e.id, hp.current);
    });
    
    // ? 死んだ敵を削除
    world.ForEach<EnemyTag, Health>([&](Entity e, EnemyTag&, Health& hp) {
        if (hp.IsDead()) {
            printf("Enemy %u died!\n", e.id);
            world.DestroyEntity(e);
        }
    });
    
    printf("Remaining enemies: %zu\n", world.GetComponentCount<EnemyTag>());
}

// ========================================================
// 包括的なデモシーン
// ========================================================

/**
 * @brief すべての新機能を使ったデモシーン
 */
inline void RunComprehensiveDemo(World& world) {
    printf("\n========================================\n");
    printf("  World v5.0 Improvement Demo\n");
    printf("========================================\n\n");
    
    // 1. Has()のデモ
    printf("--- Demo 1: Has() Method ---\n");
    DemoHasMethod(world);
    printf("\n");
    
    // 2. Get()のデモ
    printf("--- Demo 2: Get() Method ---\n");
    DemoGetMethod(world);
    printf("\n");
    
    // 3. ForEach<T1,T2>のデモ
    printf("--- Demo 3: ForEach<T1,T2> ---\n");
    DemoForEachTwoComponents(world);
    printf("\n");
    
    // 4. 物理演算のデモ
    printf("--- Demo 4: Physics System ---\n");
    DemoPhysicsSystem(world, 0.016f);
    printf("\n");
    
    // 5. デバッグ情報のデモ
    printf("--- Demo 5: Debug Info ---\n");
    DemoDebugInfo(world);
    printf("\n");
    
    // 6. Reserve()のデモ
    printf("--- Demo 6: Reserve() ---\n");
    DemoReserve(world);
    printf("\n");
    
    // 7. ID再利用のデモ
    printf("--- Demo 7: ID Reuse ---\n");
    DemoIDReuse(world);
    printf("\n");
    
    // 8. 体力システムのデモ
    printf("--- Demo 8: Health System ---\n");
    DemoHealthSystem(world);
    printf("\n");
    
    printf("========================================\n");
    printf("  All Demos Completed!\n");
    printf("========================================\n\n");
}

} // namespace WorldImprovementDemo

// ========================================================
// 使い方の例
// ========================================================
/*

// App.cpp または main.cpp で使う例

#include "samples/WorldImprovementDemo.h"

void TestWorldImprovements() {
    World world;
    
    // 個別のデモを実行
    WorldImprovementDemo::DemoHasMethod(world);
    WorldImprovementDemo::DemoGetMethod(world);
    WorldImprovementDemo::DemoForEachTwoComponents(world);
    
    // または、すべてのデモを実行
    WorldImprovementDemo::RunComprehensiveDemo(world);
}

*/

// ========================================================
// 作成者: 山内陽
// バージョン: v1.0 - World v5.0改善機能デモ
// ========================================================
