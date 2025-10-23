# GitHub Copilot カスタム指示 - HEW_ECS プロジェクト

このファイルは、GitHub Copilotがこのプロジェクト（HEW_ECS）のコードを生成・修正する際に従うべきルールと規約を定義します。

---

## ?? プロジェクト概要

**プロジェクト名**: HEW_ECS (ECS_BACE)  
**目的**: Entity Component System (ECS) を活用したチームゲーム開発  
**言語**: C++14（厳守）  
**プラットフォーム**: Windows (DirectX 11)  
**アーキテクチャ**: Entity Component System (ECS)  
**開発スタイル**: チーム開発（Git/GitHub使用）

---

## ?? 重要な制約事項

### 1. C++標準とバージョン

#### ? 使用可能（C++14）
```cpp
// auto型推論
auto entity = world.CreateEntity();

// ラムダ式
world.ForEach<Transform>([](Entity e, Transform& t) {
    t.position.x += 1.0f;
});

// スマートポインタ
std::unique_ptr<Component> component;
std::shared_ptr<Resource> resource;

// 範囲for
for (const auto& entity : entities) { }

// 初期化リスト
DirectX::XMFLOAT3 position{0.0f, 0.0f, 0.0f};
```

#### ? 使用禁止（C++17以降）
```cpp
// std::optional（C++17）
std::optional<Transform> GetTransform(Entity e);  // NG

// if constexpr（C++17）
if constexpr (std::is_same_v<T, Transform>) { }  // NG

// std::filesystem（C++17）
std::filesystem::path filePath;  // NG

// 構造化束縛（C++17）
auto [x, y, z] = GetPosition();  // NG

// インライン変数（C++17）
inline constexpr int MAX_ENTITIES = 1000;  // NG
```

**対処法**: C++14互換の代替手段を使用
```cpp
// ポインタで代用
Transform* GetTransform(Entity e) {
    return world.TryGet<Transform>(e);
}

// テンプレート特殊化で代用
template<typename T>
void Process(T& component);

// 従来のファイルシステムAPI
#include <windows.h>
// または boost::filesystem（利用可能な場合）
```

---

## ??? ECSアーキテクチャの原則

### Entity（エンティティ）

**役割**: 一意なID（識別子）のみを持つ

```cpp
struct Entity {
    uint32_t id;   // エンティティID
    uint32_t gen;  // 世代番号（削除管理用）
};
```

**禁止事項**:
```cpp
// ? NG: Entityにメソッドやデータを追加しない
struct Entity {
    uint32_t id;
    void Move(float x, float y);  // NG
    float health;                 // NG
};
```

### Component（コンポーネント）

**2種類のコンポーネント**:

#### 1. データコンポーネント（IComponent継承）
```cpp
// ? 正しい: データのみ保持
struct Health : IComponent {
    float current = 100.0f;
    float max = 100.0f;
    
    // ヘルパー関数はOK
    void TakeDamage(float dmg) {
        current -= dmg;
        if (current < 0.0f) current = 0.0f;
    }
    
    bool IsDead() const {
        return current <= 0.0f;
    }
};

// ? 間違い: IComponentを継承していない
struct Health {  // NG
    float current = 100.0f;
};
```

#### 2. Behaviourコンポーネント（Behaviour継承）
```cpp
// ? 正しい: 毎フレーム更新されるロジック
struct Rotator : Behaviour {
    float speedDegY = 45.0f;
    
    void OnStart(World& w, Entity self) override {
        // 初回起動時に1度だけ実行（オプション）
    }
    
    void OnUpdate(World& w, Entity self, float dt) override {
        auto* t = w.TryGet<Transform>(self);
        if (t) {
            t->rotation.y += speedDegY * dt;
        }
    }
};

// ? 間違い: Behaviourを継承していない
struct Rotator {  // NG
    void Update() { }
};
```

### System（システム）

**2つの実装パターン**:

#### パターン1: Behaviourパターン（推奨）
```cpp
struct PlayerController : Behaviour {
    InputSystem* input_ = nullptr;
    float speed = 5.0f;
    
    void OnUpdate(World& w, Entity self, float dt) override {
        auto* t = w.TryGet<Transform>(self);
        if (!t || !input_) return;
        
        if (input_->GetKey('W')) t->position.z += speed * dt;
        if (input_->GetKey('S')) t->position.z -= speed * dt;
    }
};
```

#### パターン2: ForEachパターン
```cpp
void UpdateMovementSystem(World& world, float dt) {
    world.ForEach<Transform, Velocity>([dt](Entity e, Transform& t, Velocity& v) {
        t.position.x += v.velocity.x * dt;
        t.position.y += v.velocity.y * dt;
        t.position.z += v.velocity.z * dt;
    });
}
```

---

## ?? コーディング規約

### 命名規約

| 要素 | 規約 | 例 |
|------|------|-----|
| **クラス名** | PascalCase | `Transform`, `MeshRenderer`, `World` |
| **構造体名** | PascalCase | `Entity`, `Health`, `Velocity` |
| **関数名** | PascalCase | `CreateEntity()`, `TryGet()`, `OnUpdate()` |
| **変数名** | camelCase | `deltaTime`, `entityId`, `speed` |
| **メンバ変数** | camelCase + `_` 接尾辞 | `world_`, `nextId_`, `alive_` |
| **定数** | UPPER_SNAKE_CASE | `MAX_ENTITIES`, `DEFAULT_SPEED` |
| **プライベートメンバ** | アンダースコア接尾辞 | `stores_`, `behaviours_` |

### コードスタイル例

```cpp
// ? 正しい例
class PlayerController : public Behaviour {
public:
    float speed = 5.0f;  // publicメンバ（camelCase）
    
    void OnUpdate(World& w, Entity self, float dt) override {
        auto* transform = w.TryGet<Transform>(self);
        if (transform) {
            transform->position.x += speed * dt;
        }
    }
    
private:
    InputSystem* input_;  // privateメンバ（アンダースコア接尾辞）
    float acceleration_;
};

// ? 間違った例
class player_controller {  // NG: PascalCaseを使用
public:
    float Speed;           // NG: camelCaseを使用
    
    void on_update() { }   // NG: PascalCaseを使用
    
private:
    int m_health;          // NG: アンダースコア接尾辞を使用
};
```

---

## ?? Worldクラスの使用方法

### エンティティの作成

#### 方法1: ビルダーパターン（推奨）
```cpp
// ? 推奨: メソッドチェーンで直感的
Entity player = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
    .With<MeshRenderer>(DirectX::XMFLOAT3{0, 1, 0})
    .With<Rotator>(45.0f)
    .With<PlayerTag>()
    .Build();  // Build()は省略可能

// ? Build()省略版
Entity enemy = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{5, 0, 0})
    .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0});
```

#### 方法2: 従来の方法
```cpp
// 許容されるが、ビルダーパターンを推奨
Entity enemy = world.CreateEntity();
world.Add<Transform>(enemy, Transform{});
world.Add<MeshRenderer>(enemy, MeshRenderer{});
```

#### 方法3: 遅延スポーン（並列処理対応）
```cpp
// スレッドセーフなスポーン
world.EnqueueSpawn(World::Cause::Spawner, [](Entity e) {
    // 生成後の初期化（メインスレッドで実行）
});
```

### コンポーネントの操作

#### 安全な取得（TryGet推奨）
```cpp
// ? 推奨: TryGetでnullチェック
auto* transform = world.TryGet<Transform>(entity);
if (transform) {
    transform->position.x += 1.0f;
}

// ?? 許容: Getは例外を投げる可能性あり
try {
    auto& transform = world.Get<Transform>(entity);
    transform.position.x += 1.0f;
} catch (const std::runtime_error& e) {
    // エラー処理
}

// ? 間違い: nullチェックなし
auto* transform = world.TryGet<Transform>(entity);
transform->position.x += 1.0f;  // NG: クラッシュの可能性
```

#### コンポーネントの追加と削除
```cpp
// ? 追加
world.Add<Health>(entity, Health{100.0f, 100.0f});

// ? 存在確認
if (world.Has<Transform>(entity)) {
    // Transformが存在する
}

// ? 削除
world.Remove<Health>(entity);
```

#### エンティティの削除
```cpp
// ? 通常の削除（フレーム終了時に実際に削除）
world.DestroyEntity(entity);

// ? 原因付き削除（デバッグログに記録）
world.DestroyEntityWithCause(entity, World::Cause::Collision);
world.DestroyEntityWithCause(entity, World::Cause::LifetimeExpired);
```

### ForEachの使用

```cpp
// 単一コンポーネント
world.ForEach<Transform>([](Entity e, Transform& t) {
    t.position.y += 0.1f;
});

// 複数コンポーネント
world.ForEach<Transform, Velocity>([dt](Entity e, Transform& t, Velocity& v) {
    t.position.x += v.velocity.x * dt;
    t.position.y += v.velocity.y * dt;
    t.position.z += v.velocity.z * dt;
});

// タグでフィルタリング
world.ForEach<PlayerTag, Transform>([](Entity e, PlayerTag& tag, Transform& t) {
    // プレイヤーのみ処理
});
```

---

## ?? DirectXMathの使用

### データ保持はXMFLOAT3/XMFLOAT4

```cpp
// ? 正しい: コンポーネント内ではXMFLOAT3を使用
struct Transform : IComponent {
    DirectX::XMFLOAT3 position{0.0f, 0.0f, 0.0f};
    DirectX::XMFLOAT3 rotation{0.0f, 0.0f, 0.0f};
    DirectX::XMFLOAT3 scale{1.0f, 1.0f, 1.0f};
};
```

### 計算時はXMVECTORを使用

```cpp
// ? 推奨: SIMD最適化のためXMVECTORで計算
void MoveTowards(Transform& t, const DirectX::XMFLOAT3& target, float speed) {
    using namespace DirectX;
    
    // XMFLOAT3 → XMVECTOR に変換
    XMVECTOR pos = XMLoadFloat3(&t.position);
    XMVECTOR tgt = XMLoadFloat3(&target);
    
    // ベクトル計算（SIMD最適化）
    XMVECTOR dir = XMVectorSubtract(tgt, pos);
    dir = XMVector3Normalize(dir);
    XMVECTOR move = XMVectorScale(dir, speed);
    XMVECTOR newPos = XMVectorAdd(pos, move);
    
    // XMVECTOR → XMFLOAT3 に戻す
    XMStoreFloat3(&t.position, newPos);
}

// ? 非推奨: 直接計算は非効率（許容はされる）
void MoveTowards(Transform& t, const DirectX::XMFLOAT3& target, float speed) {
    t.position.x += (target.x - t.position.x) * speed;
    t.position.y += (target.y - t.position.y) * speed;
    t.position.z += (target.z - t.position.z) * speed;
}
```

---

## ?? ドキュメンテーション規約

### Doxygenスタイルのコメント

```cpp
/**
 * @file MyComponent.h
 * @brief コンポーネントの簡潔な説明
 * @author [あなたの名前]
 * @date 2025
 * @version 6.0
 *
 * @details
 * 詳細な説明をここに記述します。
 */

/**
 * @struct MyComponent
 * @brief コンポーネントの簡潔な説明
 *
 * @details
 * より詳しい説明。このコンポーネントの役割や使い方を記述します。
 *
 * @par 使用例
 * @code
 * Entity e = world.Create()
 *     .With<MyComponent>(param1, param2)
 *     .Build();
 * @endcode
 *
 * @author [あなたの名前]
 */
struct MyComponent : IComponent {
    float value = 0.0f;  ///< 値の説明
};

/**
 * @brief 関数の簡潔な説明
 * 
 * @param[in] input 入力パラメータ
 * @param[out] output 出力パラメータ
 * @param[in,out] inout 入出力パラメータ
 * @return 戻り値の説明
 * 
 * @details
 * より詳しい動作の説明。
 * 
 * @note 補足情報
 * @warning 警告事項
 * @author [あなたの名前]
 */
ReturnType FunctionName(Type input, Type& output, Type& inout);
```

### インラインコメント

```cpp
// ? 良いコメント: 「なぜ」を説明
// 同一フレームでのID再利用を防ぐため、世代番号をインクリメント
generations_[id]++;

// sin関数で滑らかな上下運動を実現（範囲: [-amplitude, +amplitude]）
t->position.y = startY + sinf(time * speed) * amplitude;

// ? 悪いコメント: コードの繰り返し
// idを1増やす
id++;  // NG: コードを読めば分かる

// transformを取得
auto* t = world.TryGet<Transform>(entity);  // NG
```

---

## ?? 禁止事項

### アーキテクチャの破壊

```cpp
// ? NG: Entityにロジックを追加
struct Entity {
    void Update();  // NG
    void Render();  // NG
    float health;   // NG
};

// ? NG: グローバル変数でエンティティ管理
Entity g_player;           // NG: Worldで管理すべき
World* g_globalWorld;      // NG

// ? NG: コンポーネントが他のコンポーネントを直接参照
struct MyComponent : IComponent {
    Transform* transform_;  // NG: World経由で取得すべき
    Entity target_;         // NG: 無効化される可能性
};

// ? 正しい: World経由で取得
struct MyBehaviour : Behaviour {
    void OnUpdate(World& w, Entity self, float dt) override {
        auto* transform = w.TryGet<Transform>(self);
        if (transform) {
            // 使用
        }
    }
};
```

### 非推奨なコーディングパターン

```cpp
// ? NG: Update内での同期的エンティティ作成
void OnUpdate(World& w, Entity self, float dt) override {
    Entity newEnemy = w.CreateEntity();  // NG: イテレータ無効化の可能性
}

// ? 正しい: キューイング
void OnUpdate(World& w, Entity self, float dt) override {
    w.EnqueueSpawn(World::Cause::Spawner, [](Entity e) {
        // 生成後の初期化
    });
}

// ? NG: 毎フレームメモリ確保
void OnUpdate(World& w, Entity self, float dt) override {
    std::vector<Entity> enemies;  // NG: 毎フレーム確保
    // ...
}

// ? 改善: メンバ変数として保持
struct MySystem : Behaviour {
    std::vector<Entity> enemies_;  // メンバ変数で再利用
    
    void OnUpdate(World& w, Entity self, float dt) override {
        enemies_.clear();  // クリアして再利用
        // ...
    }
};
```

---

## ?? デバッグとログ

### DebugLogの使用

```cpp
#include "app/DebugLog.h"

// 通常のログ
DEBUGLOG("Entity created: ID=" + std::to_string(entity.id));

// 警告
DEBUGLOG_WARNING("Transform not found on entity " + std::to_string(entity.id));

// エラー
DEBUGLOG_ERROR("Failed to load resource: " + resourceName);

// 条件付きログ
#ifdef _DEBUG
    DEBUGLOG("Debug mode: Delta time = " + std::to_string(deltaTime));
#endif
```

---

## ?? マクロの活用

### DEFINE_DATA_COMPONENT

```cpp
// ? シンプルなデータコンポーネント用
DEFINE_DATA_COMPONENT(Score,
    int points = 0;
    
    void AddPoints(int p) {
        points += p;
    }
    
    void Reset() {
        points = 0;
    }
);
```

### DEFINE_BEHAVIOUR

```cpp
// ? シンプルなBehaviour用
DEFINE_BEHAVIOUR(CircularMotion,
    // メンバ変数
    float radius = 3.0f;
    float speed = 1.0f;
    float angle = 0.0f;
,
    // OnUpdate内の処理
    angle += speed * dt;
    
    auto* t = w.TryGet<Transform>(self);
    if (t) {
        t->position.x = cosf(angle) * radius;
        t->position.z = sinf(angle) * radius;
    }
);
```

---

## ?? チーム開発ルール

### ファイル編集の優先順位

#### ?? コアシステム（触らない）
以下のファイルは**変更する場合はチーム全体で相談**:
- `include/ecs/World.h`
- `include/ecs/Entity.h`
- `include/components/Component.h`
- `include/components/Transform.h`
- `include/components/MeshRenderer.h`

#### ? 自由に編集可能
- `include/scenes/` - ゲームシーンの実装
- `include/components/Custom*.h` - カスタムコンポーネント
- `src/` - 実装ファイル

#### ?? 要相談
- `include/graphics/` - グラフィックスシステム
- `include/input/` - 入力システム
- `include/app/` - アプリケーション基盤

### Gitコミットメッセージ

```bash
# ? 良い例: プレフィックスを使用
git commit -m "? Add player shooting system"
git commit -m "?? Fix collision detection bug"
git commit -m "?? Update README with component guide"
git commit -m "? Optimize render loop performance"
git commit -m "?? Refactor component structure"

# ? 悪い例: 内容が不明
git commit -m "update"
git commit -m "fix bug"
git commit -m "modified files"
```

---

## ?? 参考ファイル

新しいコンポーネントを作成する際の参考:

- `include/samples/ComponentSamples.h` - コンポーネントの実装例
- `include/samples/SampleScenes.h` - シーンの実装例
- `include/scenes/MiniGame.h` - 実践的なゲーム実装
- `include/ecs/World.h` - Worldクラスの使用方法

---

## ? コード生成時のチェックリスト

GitHub Copilotがコードを生成する際は、以下を確認してください：

- [ ] C++14標準に準拠している（C++17以降の機能は不可）
- [ ] ECS三大要素（Entity, Component, System）を正しく分離
- [ ] コンポーネントは `IComponent` または `Behaviour` を継承
- [ ] エンティティ作成にビルダーパターンを使用（推奨）
- [ ] ポインタ取得時は `TryGet` を使用し、nullチェック
- [ ] 命名規約に従っている（PascalCase/camelCase）
- [ ] Doxygenスタイルのコメントを記述
- [ ] DirectXMathの型（XMFLOAT3など）を正しく使用
- [ ] グローバル変数を使用していない
- [ ] メモリリークの可能性がない（Worldが自動管理）

---

**作成者**: 山内陽  
**最終更新**: 2025  
**バージョン**: v6.0 - チームゲーム開発フレームワーク

---

## ?? 学習リソース

- **初心者**: `include/samples/SampleScenes.h` のレベル1～3
- **中級者**: `include/samples/ComponentSamples.h` のBehaviour例
- **上級者**: `include/scenes/MiniGame.h` の実装

---

このファイルに従うことで、一貫性のある高品質なコードが生成されます。
