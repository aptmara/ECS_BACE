/**
 * @file WorldImprovementDemo.h
 * @brief Worldのデモのデモンストレーション
 * @author 山内陽        // デバッグモードでは重複チェック
        if (s.map.find(e.id) != s.map.end()) {
            char msg[160];
            sprintf_s(msg, "コンポーネント %s は既にエンティティに存在します (ID: %u, gen: %u)", typeid(T).name(), e.id, e.gen);
            DEBUGLOG_ERROR(std::string(msg));
            throw std::runtime_error(msg);
        }
 * @date2025
 * @version1.0
 *
 * @details
 * World v5.0の新機能（Has, Get, ForEach<T1,T2>など）の使用例を示します。
 */
#pragma once

#include "ecs/World.h"
#include "components/Transform.h"
#include "components/MeshRenderer.h"
#include "components/GameTags.h"
#include "components/GameComponents.h"
#include <DirectXMath.h>

namespace WorldImprovementDemo {

// ========================================================
// 新機能1: Has() - コンポーネントの存在確認
// ========================================================

/**
 * @brief Has()メソッドの使用例
 *
 * @details
 * Has()を使用することで、コンポーネントの存在チェックが簡単になります。
 */
inline void DemoHasMethod(World& world) {
 Entity entity = world.Create()
 .With<Transform>(DirectX::XMFLOAT3{0,0,0})
 .With<MeshRenderer>(DirectX::XMFLOAT3{1,0,0})
 .Build();

 // 新機能例: Has()で簡単にチェック
 if (world.Has<Transform>(entity)) {
 auto* transform = world.TryGet<Transform>(entity);
 transform->position.x +=1.0f;
 }

 // 複数コンポーネントのチェック
 if (world.Has<Transform>(entity) && world.Has<MeshRenderer>(entity)) {
 // 両方存在する場合の処理
 printf("Entity has both Transform and MeshRenderer\n");
 }

 // 安全な方法（存在しない場合は無視する）
 auto* transform = world.TryGet<Transform>(entity);
 if (transform) {
 transform->position.x +=1.0f;
 }
}

// ========================================================
// 新機能2: Get() - 安全な取得
// ========================================================

/**
 * @brief Get()メソッドの使用例
 *
 * @details
 * 必ず存在するコンポーネントに対して Get() を使用することで
 * nullptrチェック不要になります。
 */
inline void DemoGetMethod(World& world) {
 Entity player = world.Create()
 .With<Transform>(DirectX::XMFLOAT3{0,0,0})
 .With<MeshRenderer>(DirectX::XMFLOAT3{0,1,0})
 .Build();

 // 新機能例: Get()でnullptrチェック不要
 try {
 Transform& transform = world.Get<Transform>(player);
 transform.position.x +=1.0f;

 MeshRenderer& renderer = world.Get<MeshRenderer>(player);
 renderer.color = DirectX::XMFLOAT3{1,1,1};

 } catch (const std::runtime_error& e) {
 printf("Error: %s\n", e.what());
 }

 // 安全な方法（例外を避ける）
 auto* transform = world.TryGet<Transform>(player);
 if (transform) {
 transform->position.x +=1.0f;
 }
 auto* renderer = world.TryGet<MeshRenderer>(player);
 if (renderer) {
 renderer->color = DirectX::XMFLOAT3{1,1,1};
 }
}

// ========================================================
// 新機能3: ForEach<T1, T2> - 複数コンポーネントの一括処理
// ========================================================

/**
 * @brief2つのコンポーネント一括処理の使用例
 *
 * @details
 * 複数のコンポーネントを持つエンティティに対して効率的に処理を行えます。
 */
inline void DemoForEachTwoComponents(World& world) {
 // テストデータ作成
 for (int i =0; i <5; ++i) {
 world.Create()
 .With<Transform>(DirectX::XMFLOAT3{static_cast<float>(i),0,0})
 .With<MeshRenderer>(DirectX::XMFLOAT3{1,0,0})
 .Build();
 }

 // 新機能例:2つのコンポーネントを効率的に処理
 world.ForEach<Transform, MeshRenderer>(
 [](Entity e, Transform& t, MeshRenderer& r) {
 //位置に応じて色を変更
 r.color.x = t.position.x /10.0f;
 r.color.y =1.0f - (t.position.x /10.0f);
 r.color.z =0.5f;
 }
 );

 // 安全な方法（例外を避ける処理）
 world.ForEach<Transform>([&](Entity e, Transform& t) {
 auto* renderer = world.TryGet<MeshRenderer>(e);
 if (renderer) {
 renderer->color.x = t.position.x /10.0f;
 }
 });
}

// ========================================================
// 新機能4:物理演算（Transform + Velocity）
// ========================================================

/**
 * @brief物理演算システムの例
 *
 * @details
 * TransformとVelocityを持つエンティティを効率的に移動させます。
 */
inline void DemoPhysicsSystem(World& world, float dt) {
 // Velocityコンポーネントを持つオブジェクトを作成
 Entity movingObject = world.Create()
 .With<Transform>(DirectX::XMFLOAT3{0,0,0})
 .With<Velocity>()
 .With<MeshRenderer>(DirectX::XMFLOAT3{0,1,1})
 .Build();

 // 初速度設定
 world.Get<Velocity>(movingObject).velocity = DirectX::XMFLOAT3{5,0,0};

 //物理演算: TransformとVelocityを持つ全エンティティを移動
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
 * エンティティ数やコンポーネント数を取得してパフォーマンスを確認します。
 */
inline void DemoDebugInfo(World& world) {
 // エンティティ数を取得
 printf("Total entities: %zu\n", world.GetEntityCount());

 // 各コンポーネントの数を取得
 printf("Entities with Transform: %zu\n", world.GetComponentCount<Transform>());
 printf("Entities with MeshRenderer: %zu\n", world.GetComponentCount<MeshRenderer>());
 printf("Entities with Enemy: %zu\n", world.GetComponentCount<EnemyTag>());

 // エンティティ数のチェック
 if (world.GetEntityCount() <1000) {
 printf("Safe to spawn more entities\n");
 } else {
 printf("Warning: Too many entities!\n");
 }

#ifdef _DEBUG
 // デバッグビルドでは詳細情報を表示
 world.PrintDebugInfo();
#endif
}

// ========================================================
// 新機能6: Reserve -事前予約でパフォーマンス向上
// ========================================================

/**
 * @brief Reserve()の使用例
 *
 * @details
 * 大量のエンティティを作成する前に事前予約することで
 * 効率的なメモリのオーバーヘッドを削減します。
 */
inline void DemoReserve(World& world) {
 //100個のエンティティを作成する前に事前予約
 world.Reserve(100);

 for (int i =0; i <100; ++i) {
 world.Create()
 .With<Transform>(DirectX::XMFLOAT3{
 static_cast<float>(i %10) *2.0f,
0,
 static_cast<float>(i /10) *2.0f
 })
 .With<MeshRenderer>(DirectX::XMFLOAT3{1,0,0})
 .With<EnemyTag>()
 .Build();
 }

 printf("Created100 enemies efficiently!\n");
}

// ========================================================
// 新機能7: ID再利用の例
// ========================================================

/**
 * @brief ID再利用の確認
 *
 * @details
 * エンティティを削除した後のIDを再利用することで、
 * 無駄なCreateEntity()を防ぎます。
 */
inline void DemoIDReuse(World& world) {
 printf("=== ID Reuse Demo ===\n");

 // エンティティを3つ作成
 Entity e1 = world.CreateEntity();
 Entity e2 = world.CreateEntity();
 Entity e3 = world.CreateEntity();

 printf("Created: ID=%u, %u, %u\n", e1.id, e2.id, e3.id);

 //2番目を削除
 world.DestroyEntity(e2);
 printf("Deleted: ID=%u\n", e2.id);

 // 新しいエンティティを作成（削除されたIDを再利用する）
 Entity e4 = world.CreateEntity();
 printf("Created: ID=%u (reused!)\n", e4.id);

 printf("=====================");
}

// ========================================================
// 応用例: ヘルスシステムと敵の削除
// ========================================================

/**
 * @brief ヘルスシステムの例
 *
 * @details
 * HealthとEnemyを持つエンティティを管理し、
 * ヘルスが0になった敵を削除します。
 */
inline void DemoHealthSystem(World& world) {
 // 敵を5体作成
 for (int i =0; i <5; ++i) {
 Entity enemy = world.Create()
 .With<Transform>(DirectX::XMFLOAT3{static_cast<float>(i) *2.0f,0,0})
 .With<MeshRenderer>(DirectX::XMFLOAT3{1,0,0})
 .With<EnemyTag>()
 .Build();

 // Healthを追加
 Health hp;
 hp.current =100.0f;
 hp.max =100.0f;
 world.Add<Health>(enemy, hp);
 }

 printf("Created %zu enemies with health\n", world.GetComponentCount<EnemyTag>());

 // 全ての敵にダメージを与える
 world.ForEach<EnemyTag, Health>([](Entity e, EnemyTag&, Health& hp) {
 hp.TakeDamage(50.0f);
 printf("Enemy %u: HP = %.1f\n", e.id, hp.current);
 });

 // 死亡した敵を削除
 world.ForEach<EnemyTag, Health>([&](Entity e, EnemyTag&, Health& hp) {
 if (hp.IsDead()) {
 printf("Enemy %u died!\n", e.id);
 world.DestroyEntity(e);
 }
 });

 printf("Remaining enemies: %zu\n", world.GetComponentCount<EnemyTag>());
}

// ========================================================
// 総合デモ
// ========================================================

/**
 * @brief 総合デモの実行
 */
inline void RunComprehensiveDemo(World& world) {
 printf("\n========================================\n");
 printf(" World v5.0 Improvement Demo\n");
 printf("========================================\n\n");

 //1. Has()のデモ
 printf("--- Demo1: Has() Method ---\n");
 DemoHasMethod(world);
 printf("\n");

 //2. Get()のデモ
 printf("--- Demo2: Get() Method ---\n");
 DemoGetMethod(world);
 printf("\n");

 //3. ForEach<T1,T2>のデモ
 printf("--- Demo3: ForEach<T1,T2> ---\n");
 DemoForEachTwoComponents(world);
 printf("\n");

 //4.物理演算のデモ
 printf("--- Demo4: Physics System ---\n");
 DemoPhysicsSystem(world,0.016f);
 printf("\n");

 //5. デバッグ情報のデモ
 printf("--- Demo5: Debug Info ---\n");
 DemoDebugInfo(world);
 printf("\n");

 //6. Reserve()のデモ
 printf("--- Demo6: Reserve() ---\n");
 DemoReserve(world);
 printf("\n");

 //7. ID再利用のデモ
 printf("--- Demo7: ID Reuse ---\n");
 DemoIDReuse(world);
 printf("\n");

 //8. ヘルスシステムのデモ
 printf("--- Demo8: Health System ---\n");
 DemoHealthSystem(world);
 printf("\n");

 printf("========================================\n");
 printf(" All Demos Completed!\n");
 printf("========================================\n\n");
}

} // namespace WorldImprovementDemo

// ========================================================
// 実行例
// ========================================================
/*

// App.cpp または main.cppで使用

#include "samples/WorldImprovementDemo.h"

void TestWorldImprovements() {
 World world;

 // 個別のデモを実行
 WorldImprovementDemo::DemoHasMethod(world);
 WorldImprovementDemo::DemoGetMethod(world);
 WorldImprovementDemo::DemoForEachTwoComponents(world);

 // または、総合的なデモを実行
 WorldImprovementDemo::RunComprehensiveDemo(world);
}

*/

// ========================================================
// 作成者: 山内陽
// バージョン: v1.0 - World v5.0新機能デモ
// ========================================================
