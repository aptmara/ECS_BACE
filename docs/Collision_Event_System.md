# 🎉 Collision System v2.1 - OnEnter/OnStay/OnExit Events

## 📋 新機能: イベントシステム

**バージョン**: v2.1  
**リリース日**: 2025年  
**作成者**: 山内陽

---

##  何が追加されたか

### ICollisionHandler インターフェース

Unityの`OnCollisionEnter`や`OnCollisionExit`のようなイベントドリブンな衝突処理が可能になりました！

```cpp
struct ICollisionHandler : IComponent {
    // 衝突が開始した瞬間 (1フレームのみ)
    virtual void OnCollisionEnter(World& w, Entity self, Entity other, const CollisionInfo& info);
    
    // 衝突中 (毎フレーム)
    virtual void OnCollisionStay(World& w, Entity self, Entity other, const CollisionInfo& info);
    
    // 衝突が終了した瞬間 (1フレームのみ)
    virtual void OnCollisionExit(World& w, Entity self, Entity other);
};
```

---

## 🚀 使い方

### 基本的な使い方(3ステップ)

#### ステップ1: ハンドラーを定義

```cpp
struct PlayerCollisionHandler : ICollisionHandler {
    void OnCollisionEnter(World& w, Entity self, Entity other, const CollisionInfo& info) override {
        if (w.Has<EnemyTag>(other)) {
    DEBUGLOG("⚔️ 敵と衝突!");
            
      // ダメージ処理
          auto* health = w.TryGet<Health>(self);
      if (health) {
       health->TakeDamage(10.0f);
}
   }
    }
    
    void OnCollisionExit(World& w, Entity self, Entity other) override {
        DEBUGLOG("✅ 衝突終了");
    }
};
```

#### ステップ2: エンティティに追加

```cpp
Entity player = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
    .With<CollisionBox>(1.0f)
    .With<PlayerTag>()
  .With<PlayerCollisionHandler>()  // ハンドラーを追加
    .Build();
```

#### ステップ3: 衝突検出システムを作成

```cpp
Entity collisionSystem = world.Create()
  .With<CollisionDetectionSystem>()
    .Build();
```

**これだけ！** 自動的にイベントが呼ばれます。

---

## 📚 実用例

### 例1: アイテム取得

```cpp
struct ItemCollector : ICollisionHandler {
    int itemsCollected = 0;

    void OnCollisionEnter(World& w, Entity self, Entity other, const CollisionInfo& info) override {
        if (w.Has<ItemTag>(other)) {
    itemsCollected++;
        DEBUGLOG("✨ アイテム取得! 合計: " + std::to_string(itemsCollected));
            w.DestroyEntity(other);  // アイテムを削除
        }
    }
};
```

### 例2: ダメージゾーン

```cpp
struct DamageZoneHandler : ICollisionHandler {
    float damagePerSecond = 10.0f;

    void OnCollisionEnter(World& w, Entity self, Entity other, const CollisionInfo& info) override {
      DEBUGLOG("⚠️ ダメージゾーンに入った!");
    }

    void OnCollisionStay(World& w, Entity self, Entity other, const CollisionInfo& info) override {
        // 継続的にダメージ
        auto* health = w.TryGet<Health>(self);
        if (health) {
          health->TakeDamage(damagePerSecond / 60.0f);  // 60FPS想定
        }
    }

    void OnCollisionExit(World& w, Entity self, Entity other) override {
   DEBUGLOG("✅ ダメージゾーンから脱出!");
    }
};
```

### 例3: チェックポイント

```cpp
struct CheckpointTrigger : ICollisionHandler {
    bool activated = false;

    void OnCollisionEnter(World& w, Entity self, Entity other, const CollisionInfo& info) override {
        if (activated) return;

        if (w.Has<PlayerTag>(other)) {
  activated = true;
    DEBUGLOG("🚩 チェックポイント到達!");
       
    // 色を変える
            auto* renderer = w.TryGet<MeshRenderer>(self);
      if (renderer) {
      renderer->color = DirectX::XMFLOAT3{0, 1, 0};  // 緑色
            }
        }
    }
};
```

### 例4: 物理的な押し出し

```cpp
struct PhysicsCollisionHandler : ICollisionHandler {
    void OnCollisionStay(World& w, Entity self, Entity other, const CollisionInfo& info) override {
        if (w.Has<WallTag>(other)) {
   auto* transform = w.TryGet<Transform>(self);
  if (transform) {
                // 衝突法線方向に押し出す
          transform->position.x -= info.normal.x * info.penetrationDepth;
  transform->position.y -= info.normal.y * info.penetrationDepth;
             transform->position.z -= info.normal.z * info.penetrationDepth;
     }
        }
    }
};
```

---

## 🎯 イベントの発火タイミング

```
フレーム 1: 衝突なし
         ↓
フレーム 2: 衝突開始
→ OnCollisionEnter() 呼び出し
         ↓
フレーム 3: 衝突継続
   → OnCollisionStay() 呼び出し
  ↓
フレーム 4: 衝突継続
 → OnCollisionStay() 呼び出し
    ↓
フレーム 5: 衝突終了
    → OnCollisionExit() 呼び出し
         ↓
フレーム 6: 衝突なし
```

---

## ⚙️ 内部実装の詳細

### CollisionDetectionSystem の拡張

```cpp
struct CollisionDetectionSystem : Behaviour {
    void OnUpdate(World& w, Entity self, float dt) override {
    // 前フレームの衝突情報を保存
        previousCollisions_.swap(currentCollisions_);
        currentCollisions_.clear();

 // 衝突判定を実行
        for (/* すべてのペア */) {
            if (collision) {
          uint64_t pairKey = MakePairKey(a, b);
             currentCollisions_.insert(pairKey);

bool wasColliding = previousCollisions_.find(pairKey) != previousCollisions_.end();

       if (!wasColliding) {
   TriggerCollisionEnter(w, a, b, info);  // 
   } else {
       TriggerCollisionStay(w, a, b, info);   // 🔄
   }
            }
        }

        // 前フレームにあったが今フレームにない衝突
        for (uint64_t pairKey : previousCollisions_) {
      if (currentCollisions_.find(pairKey) == currentCollisions_.end()) {
   TriggerCollisionExit(w, entityA, entityB);  // 🔚
            }
        }
    }
};
```

---

## 📊 v2.0からの改善点

| 項目 | v2.0 | v2.1 | 改善 |
|------|------|------|------|
| イベントシステム | ❌ なし | ✅ OnEnter/Stay/Exit |  |
| 前フレーム追跡 | ❌ なし | ✅ あり |  |
| 使いやすさ | 中 | **高** | ⬆️⬆️ |
| Unityライク | ❌ | ✅ | ⬆️⬆️⬆️ |

---

## 🎓 サンプルコード

詳細なサンプルは `include/samples/CollisionEventSamples.h` を参照してください。

```cpp
#include "samples/CollisionEventSamples.h"

void GameScene::OnEnter(World& world) {
    // アイテム取得システムのデモ
  CollisionEventSamples::Sample1_ItemCollection(world);
    
    // または、すべてのサンプルを実行
    // CollisionEventSamples::RunAllEventSamples(world);
}
```

---

## 💡 ベストプラクティス

### DO ✅

- ✅ 単一責任の原則: 1つのハンドラーは1つの機能のみ
- ✅ 軽量な処理: OnUpdateではなくイベントを活用
- ✅ nullチェック: コンポーネント取得時は必ず確認

### DON'T ❌

- ❌ OnCollisionStay での重い処理 (毎フレーム呼ばれる)
- ❌ イベント内でのエンティティ大量生成
- ❌ 循環参照の作成

---

## 🐛 トラブルシューティング

### Q: イベントが呼ばれない

**A**: 以下を確認してください

1. ✅ `CollisionDetectionSystem` が作成されているか
2. ✅ 両方のエンティティに `Transform` があるか
3. ✅ 両方のエンティティに衝突形状コンポーネントがあるか
4. ✅ `ICollisionHandler` を継承しているか
5. ✅ エンティティにハンドラーが追加されているか

### Q: OnCollisionExit が呼ばれない

**A**: エンティティが破棄される前に衝突が終了する必要があります。
破棄後は `IsAlive()` がfalseになるため、イベントは発火しません。

---

## 🔮 今後の拡張予定

- [ ] OnTriggerEnter/Stay/Exit (物理応答なし)
- [ ] コリジョンイベントのフィルタリング
- [ ] イベント優先度システム
- [ ] マルチスレッド対応

---

## 📝 変更履歴

### v2.1 (2025) - イベントシステム追加

- ✅ `ICollisionHandler` インターフェース追加
- ✅ OnCollisionEnter/OnStay/OnExit イベント実装
- ✅ 前フレーム衝突情報の追跡
- ✅ 6つの実用サンプル追加
- ✅ ドキュメント更新

### v2.0 (2025) - ECS準拠リファクタリング

- ✅ ECS設計原則への完全準拠
- ✅ 3種類の衝突形状実装
- ✅ コールバックシステム

---

## 🎉 まとめ

**Collision System v2.1** は、Unityライクな直感的なイベントシステムを提供し、
衝突処理を**より簡単に、より読みやすく**実装できるようになりました！

```cpp
// たった3行でアイテム取得システムが完成！
struct ItemCollector : ICollisionHandler {
    void OnCollisionEnter(World& w, Entity self, Entity other, const CollisionInfo& info) override {
   if (w.Has<ItemTag>(other)) w.DestroyEntity(other);
    }
};
```

---

**Happy Collision Event Handling! 🎮✨**

---

## 📚 関連ドキュメント

- [Collision_System_Guide.md](./Collision_System_Guide.md) - 完全ガイド
- [CollisionEventSamples.h](../include/samples/CollisionEventSamples.h) - サンプルコード
- [README.md](../README.md) - プロジェクト概要

---

**作成者**: 山内陽  
**バージョン**: v2.1  
**最終更新**: 2025年
