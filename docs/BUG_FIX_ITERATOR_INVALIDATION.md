# Iterator Invalidation Bug Fix

## 問題の概要 (Problem Summary)

Debug Assertion Failed エラーが発生していました：
```
File: C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Tools\MSVC\14.44.35207\include\list
Line: 160
Expression: cannot increment value-initialized list iterator
```

## 根本原因 (Root Cause)

`MiniGame.h` の `CheckCollisions()` メソッドで、**イテレータ無効化（Iterator Invalidation）**が発生していました。

### 問題のあったコード：
```cpp
void CheckCollisions(World& world) {
    world.ForEach<Bullet>([&](Entity bulletEntity, Bullet& bullet) {
        world.ForEach<Enemy>([&](Entity enemyEntity, Enemy& enemy) {
            if (distance < 1.0f) {
                world.DestroyEntity(bulletEntity);  // ← ここでイテレータが無効化！
                world.DestroyEntity(enemyEntity);
                score_ += 10;
            }
        });
    });
}
```

### なぜエラーが起きたか：

1. `ForEach<Bullet>` が `std::unordered_map` をイテレート中
2. `DestroyEntity(bulletEntity)` が呼ばれる
3. これにより `Store<Bullet>::map` から要素が削除される
4. **イテレータが無効化される**
5. 外側のループが次の要素に進もうとして **クラッシュ**

## 修正内容 (Fix Applied)

### 1. MiniGame.h の CheckCollisions() を修正

**修正前：**
```cpp
void CheckCollisions(World& world) {
    world.ForEach<Bullet>([&](Entity bulletEntity, Bullet& bullet) {
        world.ForEach<Enemy>([&](Entity enemyEntity, Enemy& enemy) {
            if (distance < 1.0f) {
                world.DestroyEntity(bulletEntity);
                world.DestroyEntity(enemyEntity);
                score_ += 10;
            }
        });
    });
}
```

**修正後：**
```cpp
void CheckCollisions(World& world) {
    // 削除するエンティティを収集（イテレータ無効化を防ぐ）
    std::vector<Entity> entitiesToDestroy;
    
    world.ForEach<Bullet>([&](Entity bulletEntity, Bullet& bullet) {
        // この弾がすでに削除予定なら処理をスキップ
        for (const auto& e : entitiesToDestroy) {
            if (e.id == bulletEntity.id) return;
        }
        
        world.ForEach<Enemy>([&](Entity enemyEntity, Enemy& enemy) {
            // この敵がすでに削除予定なら処理をスキップ
            for (const auto& e : entitiesToDestroy) {
                if (e.id == enemyEntity.id) return;
            }
            
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
```

### 2. MiniGame.h の OnExit() も修正

同様の問題があったため修正：

```cpp
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
```

### 3. World.h の Tick() を改善

より安全なインデックスベースのループに変更：

**修正前：**
```cpp
void Tick(float dt) {
    for (auto& it : behaviours_) {
        if (!IsAlive(it.e)) continue;
        it.b->OnUpdate(*this, it.e, dt);
    }
}
```

**修正後：**
```cpp
void Tick(float dt) {
    // イテレーション中の削除に対応するため、インデックスベースのループを使用
    for (size_t i = 0; i < behaviours_.size(); ++i) {
        auto& it = behaviours_[i];
        if (!IsAlive(it.e)) continue;
        if (!it.started) { it.b->OnStart(*this, it.e); it.started = true; }
        it.b->OnUpdate(*this, it.e, dt);
    }
}
```

## 学んだ教訓 (Lessons Learned)

### イテレータ無効化を防ぐベストプラクティス：

1. **イテレート中にコンテナを変更しない**
   - `std::vector::erase()`, `std::unordered_map::erase()` など
   
2. **削除する要素を先に収集する**
   - `std::vector<Entity> toDelete;` を使う
   - イテレーション完了後に削除
   
3. **インデックスベースのループを使う**
   - Range-based for よりも安全な場合がある
   - サイズが変わっても対応しやすい

4. **デバッグビルドでテストする**
   - イテレータチェックが有効
   - 問題を早期発見できる

## 参考：よくあるイテレータ無効化のパターン

```cpp
// ? ダメな例
for (auto& item : container) {
    if (shouldDelete) {
        container.erase(item);  // イテレータが無効化される！
    }
}

// ? 良い例 1: 削除リスト使用
std::vector<Key> toDelete;
for (auto& item : container) {
    if (shouldDelete) {
        toDelete.push_back(item.key);
    }
}
for (auto& key : toDelete) {
    container.erase(key);
}

// ? 良い例 2: erase-remove イディオム（vector の場合）
container.erase(
    std::remove_if(container.begin(), container.end(),
        [](auto& item) { return shouldDelete(item); }),
    container.end()
);
```

## 動作確認

- ? ビルド成功
- ? エラーメッセージなし
- ? ゲームが正常に動作するはず
