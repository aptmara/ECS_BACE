/**
 * @file Collision.cpp
 * @brief 衝突判定システムの実装
 * @author 立山悠朔・上手凉太郎・山内陽
 * @date 2025
 * @version 2.1
 */

#include "pch.h"
#include "components/Collision.h"
#include "scenes/Game.h" // ✅ PlayerCollisionHandler と EnemyCollisionHandler をインクルード

#ifdef _DEBUG
#include "graphics/DebugDraw.h"
#include "app/ServiceLocator.h"

/**
 * @brief CollisionDebugRendererの実装
 */
void CollisionDebugRenderer::OnUpdate(World& w, Entity self, float dt) {
   if (!enabled) return;

    // DebugDrawシステムを取得(try-catchでエラーハンドリング)
 DebugDraw* debugDraw = nullptr;
    try {
        debugDraw = &ServiceLocator::Get<DebugDraw>();
    } catch (const std::runtime_error&) {
        // DebugDrawが登録されていない場合は警告を出して終了
     static bool warningShown = false;
  if (!warningShown) {
   DEBUGLOG_WARNING("CollisionDebugRenderer: DebugDrawシステムが登録されていません。ServiceLocatorに登録してください。");
            warningShown = true;
   }
        return;
    }

    if (!debugDraw || !debugDraw->IsInitialized()) {
    return;
    }

    // CollisionBoxを持つエンティティを描画
    w.ForEach<Transform, CollisionBox>([&](Entity e, Transform& t, CollisionBox& box) {
 auto center = box.GetWorldCenter(t);
      auto size = box.GetScaledSize(t);

  // ボックスをワイヤーフレームで描画
        // DebugDraw::DrawBox は center と halfExtents を受け取るため
    DirectX::XMFLOAT3 halfExtents = {
   size.x * 0.5f,
  size.y * 0.5f,
      size.z * 0.5f
        };
        debugDraw->DrawBox(center, halfExtents, boxColor);
    });

    // CollisionSphereを持つエンティティを描画
    w.ForEach<Transform, CollisionSphere>([&](Entity e, Transform& t, CollisionSphere& sphere) {
auto center = sphere.GetWorldCenter(t);
        float radius = sphere.GetScaledRadius(t);

     // 球をワイヤーフレームで描画
      debugDraw->DrawSphere(center, radius, sphereColor);
    });

    // CollisionCapsuleを持つエンティティを描画
    w.ForEach<Transform, CollisionCapsule>([&](Entity e, Transform& t, CollisionCapsule& capsule) {
  auto top = capsule.GetTopPoint(t);
     auto bottom = capsule.GetBottomPoint(t);
   float radius = capsule.radius * std::max({t.scale.x, t.scale.y, t.scale.z});

        // カプセルをワイヤーフレームで描画(簡易実装: 球2つ+線分)
        debugDraw->DrawSphere(top, radius, sphereColor);
     debugDraw->DrawSphere(bottom, radius, sphereColor);
     debugDraw->AddLine(top, bottom, sphereColor);
    });
}

#endif // _DEBUG

// ========================================================
// CollisionDetectionSystem のイベントハンドラー実装
// ========================================================

void CollisionDetectionSystem::ForEachHandler(World& w, Entity e, const std::function<void(ICollisionHandler*)>& func) {
    // すべての既知のハンドラー型を試行
    if (auto* h = w.TryGet<PlayerCollisionHandler>(e)) {
        func(static_cast<ICollisionHandler*>(h));
        return;
    }
    if (auto* h = w.TryGet<EnemyCollisionHandler>(e)) {
        func(static_cast<ICollisionHandler*>(h));
        return;
    }
    // 🔧 新しいハンドラー型を追加する場合はここに追記
}

void CollisionDetectionSystem::TriggerCollisionEnter(World& w, Entity a, Entity b, const CollisionInfo& info) {
    DEBUGLOG("🔥 OnCollisionEnter: Entity " + std::to_string(a.id) + " <-> Entity " + std::to_string(b.id));

    // エンティティAのハンドラーを呼び出す
    ForEachHandler(w, a, [&](ICollisionHandler* handler) {
        DEBUGLOG("  ✅ Entity " + std::to_string(a.id) + " has handler, calling OnCollisionEnter");
        handler->OnCollisionEnter(w, a, b, info);
    });

    // エンティティBのハンドラーを呼び出す(法線を反転)
    ForEachHandler(w, b, [&](ICollisionHandler* handler) {
        DEBUGLOG("  ✅ Entity " + std::to_string(b.id) + " has handler, calling OnCollisionEnter");
        CollisionInfo reversedInfo = info;
  std::swap(reversedInfo.entityA, reversedInfo.entityB);
      reversedInfo.normal.x = -reversedInfo.normal.x;
        reversedInfo.normal.y = -reversedInfo.normal.y;
        reversedInfo.normal.z = -reversedInfo.normal.z;
        handler->OnCollisionEnter(w, b, a, reversedInfo);
    });
}

void CollisionDetectionSystem::TriggerCollisionStay(World& w, Entity a, Entity b, const CollisionInfo& info) {
    ForEachHandler(w, a, [&](ICollisionHandler* handler) {
        handler->OnCollisionStay(w, a, b, info);
    });

    ForEachHandler(w, b, [&](ICollisionHandler* handler) {
        CollisionInfo reversedInfo = info;
        std::swap(reversedInfo.entityA, reversedInfo.entityB);
      reversedInfo.normal.x = -reversedInfo.normal.x;
    reversedInfo.normal.y = -reversedInfo.normal.y;
        reversedInfo.normal.z = -reversedInfo.normal.z;
      handler->OnCollisionStay(w, b, a, reversedInfo);
    });
}

void CollisionDetectionSystem::TriggerCollisionExit(World& w, Entity a, Entity b) {
  if (!w.IsAlive(a) || !w.IsAlive(b)) return;

    DEBUGLOG("🔚 OnCollisionExit: Entity " + std::to_string(a.id) + " <-> Entity " + std::to_string(b.id));

    ForEachHandler(w, a, [&](ICollisionHandler* handler) {
        handler->OnCollisionExit(w, a, b);
    });

    ForEachHandler(w, b, [&](ICollisionHandler* handler) {
        handler->OnCollisionExit(w, b, a);
    });
}
