# ?? ECS学習ガイド - ステップバイステップ

このガイドは、**何もわからない初学者**が**自分でコンポーネントを作れるようになる**までの道のりです。

---

## ?? 学習のゴール

このガイドを終えると、以下ができるようになります：

- ? エンティティとコンポーネントの概念を理解する
- ? 自分でコンポーネントを作成できる
- ? コンポーネントを組み合わせてゲームオブジェクトを作れる
- ? デバッグ方法がわかる

---

## ?? レッスン1: エンティティを理解する

### エンティティとは？

**エンティティ = ゲーム世界に存在する「物」を表すID番号**

```cpp
Entity player;   // これはID番号（例: 1）
Entity enemy;    // これもID番号（例: 2）
Entity bullet;   // これもID番号（例: 3）
```

### 重要なポイント

- ? エンティティ自体には機能がない（ただの番号）
- ? コンポーネントを付けることで機能を持つ
- ? 同じエンティティに複数のコンポーネントを付けられる

### 実習: エンティティを作ってみる

```cpp
// App.hのCreateDemoScene()に追加

void CreateDemoScene() {
    // エンティティを1つ作成
    Entity myFirstEntity = world_.CreateEntity();
    
    // まだ何も機能がない（ID番号だけ）
    // 次のレッスンでコンポーネントを付けます
}
```

**試してみよう:**
1. 上記のコードを`App.h`に追加
2. ビルド＆実行（F5）
3. 何も表示されない → 正常！（まだ見た目がないから）

---

## ?? レッスン2: コンポーネントを理解する

### コンポーネントとは？

**コンポーネント = エンティティに付ける「部品」**

例: レゴブロックのように、部品を組み合わせて物を作る

```
プレイヤー = エンティティ
           + Transform（位置）
           + MeshRenderer（見た目）
           + PlayerController（操作）
           + Health（体力）
```

### コンポーネントの種類

#### 1. データコンポーネント（状態を保存するだけ）

```cpp
struct Transform : IComponent {
    DirectX::XMFLOAT3 position; // 位置
    DirectX::XMFLOAT3 rotation; // 回転
    DirectX::XMFLOAT3 scale;    // 大きさ
};
```

#### 2. Behaviourコンポーネント（動きを定義）

```cpp
struct Rotator : Behaviour {
    float speedDegY = 45.0f; // 回転速度
    
    // 毎フレーム呼ばれる
    void OnUpdate(World& w, Entity self, float dt) override {
        auto* t = w.TryGet<Transform>(self);
        if (t) {
            t->rotation.y += speedDegY * dt; // 回転させる
        }
    }
};
```

### 実習: コンポーネントを追加してみる

```cpp
void CreateDemoScene() {
    // エンティティを作成
    Entity myFirstEntity = world_.CreateEntity();
    
    // Transform（位置情報）を追加
    Transform transform;
    transform.position = DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f}; // 原点
    transform.rotation = DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f};
    transform.scale = DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f};
    world_.Add<Transform>(myFirstEntity, transform);
    
    // MeshRenderer（見た目）を追加
    MeshRenderer renderer;
    renderer.color = DirectX::XMFLOAT3{1.0f, 0.0f, 0.0f}; // 赤色
    world_.Add<MeshRenderer>(myFirstEntity, renderer);
}
```

**試してみよう:**
1. 上記のコードを`App.h`に追加
2. ビルド＆実行
3. 赤いキューブが表示される！ ??

---

## ?? レッスン3: ビルダーパターンを使う

### もっと簡単に書く方法

レッスン2のコードは長いですね。**ビルダーパターン**を使うと短く書けます。

#### Before（長い）
```cpp
Entity myFirstEntity = world_.CreateEntity();
Transform transform;
transform.position = DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f};
// ... 省略 ...
world_.Add<Transform>(myFirstEntity, transform);
```

#### After（短い！）
```cpp
Entity myFirstEntity = world_.Create()
    .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})  // 位置だけ指定
    .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0}) // 赤色
    .Build();
```

### 実習: ビルダーパターンで書き直す

```cpp
void CreateDemoScene() {
    // 赤いキューブを作成
    Entity redCube = world_.Create()
        .With<Transform>(
            DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f},  // 位置
            DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f},  // 回転
            DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f}   // スケール
        )
        .With<MeshRenderer>(DirectX::XMFLOAT3{1.0f, 0.0f, 0.0f}) // 赤
        .Build();
}
```

**課題:**
1. 緑色のキューブを作ってみる（位置: X=2）
2. 青色のキューブを作ってみる（位置: X=-2）

<details>
<summary>解答例</summary>

```cpp
// 緑色のキューブ
Entity greenCube = world_.Create()
    .With<Transform>(DirectX::XMFLOAT3{2.0f, 0.0f, 0.0f})
    .With<MeshRenderer>(DirectX::XMFLOAT3{0.0f, 1.0f, 0.0f})
    .Build();

// 青色のキューブ
Entity blueCube = world_.Create()
    .With<Transform>(DirectX::XMFLOAT3{-2.0f, 0.0f, 0.0f})
    .With<MeshRenderer>(DirectX::XMFLOAT3{0.0f, 0.0f, 1.0f})
    .Build();
```

</details>

---

## ?? レッスン4: 動きを付ける（Behaviour）

### Rotatorを追加する

回転させるには、`Rotator`コンポーネントを追加します。

```cpp
Entity rotatingCube = world_.Create()
    .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
    .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
    .With<Rotator>(45.0f) // ← 毎秒45度回転
    .Build();
```

### 実習: 回転速度を変えてみる

```cpp
// ゆっくり回転
.With<Rotator>(10.0f)   // 毎秒10度

// 速く回転
.With<Rotator>(180.0f)  // 毎秒180度

// 逆回転
.With<Rotator>(-45.0f)  // 逆方向
```

**課題:**
1. 速く回転する緑のキューブを作る
2. 逆回転する青のキューブを作る

---

## ?? レッスン5: コンポーネントを組み合わせる

### 複数のBehaviourを追加

1つのエンティティに、複数のBehaviourを付けられます！

```cpp
Entity cube = world_.Create()
    .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
    .With<MeshRenderer>(DirectX::XMFLOAT3{1, 1, 0})
    .With<Rotator>(45.0f)      // 回転する
    .With<Bouncer>()           // 上下に動く（ComponentSamples.h）
    .Build();
```

このキューブは：
- ? 回転しながら
- ? 上下に動く

### 実習: ComponentSamples.hを使う

`ComponentSamples.h`には、便利なコンポーネントがたくさんあります。

```cpp
#include "ComponentSamples.h" // ← App.hの最初に追加

void CreateDemoScene() {
    // 上下に動くキューブ
    Entity bouncing = world_.Create()
        .With<Transform>(DirectX::XMFLOAT3{-3, 0, 0})
        .With<MeshRenderer>(DirectX::XMFLOAT3{1, 1, 0})
        .With<Bouncer>() // ← ComponentSamples.hから
        .Build();
    
    // 大きさが変わるキューブ
    Entity pulse = world_.Create()
        .With<Transform>(DirectX::XMFLOAT3{3, 0, 0})
        .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 1})
        .With<PulseScale>() // ← ComponentSamples.hから
        .Build();
}
```

**課題:**
1. `Bouncer`と`Rotator`を両方付けてみる
2. `PulseScale`と`ColorCycle`を両方付けてみる

---

## ?? レッスン6: 自分でコンポーネントを作る

### 最初のカスタムBehaviour

**目標:** 前に進むコンポーネントを作る

#### ステップ1: 構造体を定義

```cpp
// App.hのprivate:の前に追加

struct MoveForward : Behaviour {
    float speed = 2.0f; // 速度
    
    void OnUpdate(World& w, Entity self, float dt) override {
        // Transformを取得
        auto* t = w.TryGet<Transform>(self);
        if (!t) return; // Transformがなければ終了
        
        // Z軸方向に進む
        t->position.z += speed * dt;
    }
};
```

#### ステップ2: 使ってみる

```cpp
Entity movingCube = world_.Create()
    .With<Transform>(DirectX::XMFLOAT3{0, 0, -5})
    .With<MeshRenderer>(DirectX::XMFLOAT3{0, 1, 1})
    .With<MoveForward>() // ← 自作コンポーネント
    .Build();
```

**結果:** キューブが奥に向かって進んでいく！

### 実習: 改造してみる

```cpp
// 上に進む
t->position.y += speed * dt;

// 横に進む
t->position.x += speed * dt;

// 斜めに進む
t->position.x += speed * dt;
t->position.y += speed * dt;
```

**課題:**
1. 上に進むコンポーネント`MoveUp`を作る
2. 円を描くコンポーネント`CircleMotion`を作る（難しい）

<details>
<summary>CircleMotionの解答例</summary>

```cpp
struct CircleMotion : Behaviour {
    float radius = 3.0f;
    float speed = 1.0f;
    float angle = 0.0f;
    
    void OnUpdate(World& w, Entity self, float dt) override {
        angle += speed * dt;
        
        auto* t = w.TryGet<Transform>(self);
        if (t) {
            t->position.x = cosf(angle) * radius;
            t->position.z = sinf(angle) * radius;
        }
    }
};
```

</details>

---

## ?? レッスン7: コンポーネント間の連携

### 他のコンポーネントを参照する

**例:** 体力が0になったら削除する

#### ステップ1: Healthコンポーネントを追加

```cpp
Entity enemy = world_.Create()
    .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
    .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
    .Build();

// 体力を追加
Health hp;
hp.current = 100.0f;
hp.max = 100.0f;
world_.Add<Health>(enemy, hp);
```

#### ステップ2: DestroyOnDeathを追加

```cpp
world_.Add<DestroyOnDeath>(enemy, DestroyOnDeath{});
```

#### ステップ3: ダメージを与える

```cpp
// どこかで（例: キー入力時）
auto* health = world_.TryGet<Health>(enemy);
if (health) {
    health->TakeDamage(10.0f); // 10ダメージ
}
```

**結果:** 体力が0になったらエンティティが消える！

---

## ?? レッスン8: デバッグ方法

### コンポーネントの値を確認

```cpp
// Transformの値を確認
auto* t = world_.TryGet<Transform>(entity);
if (t) {
    // ブレークポイントを設定して見る
    float x = t->position.x;
    float y = t->position.y;
    float z = t->position.z;
}
```

### OutputDebugStringを使う

```cpp
#include <sstream>

void OnUpdate(World& w, Entity self, float dt) override {
    auto* t = w.TryGet<Transform>(self);
    if (t) {
        std::wostringstream oss;
        oss << L"Position: " << t->position.x << L", " 
            << t->position.y << L", " << t->position.z << L"\n";
        OutputDebugString(oss.str().c_str());
    }
}
```

Visual Studioの「出力」ウィンドウに表示されます。

---

## ?? レッスン9: 実践プロジェクト

### プロジェクト1: シンプルなゲーム

**目標:** プレイヤーが操作できるキューブを作る

```cpp
// 1. Playerタグを定義
struct PlayerTag : IComponent {};

// 2. プレイヤー操作コンポーネント
struct PlayerController : Behaviour {
    float speed = 5.0f;
    
    void OnUpdate(World& w, Entity self, float dt) override {
        auto* t = w.TryGet<Transform>(self);
        if (!t) return;
        
        // TODO: InputSystemを使ってWASDで移動
    }
};

// 3. プレイヤーエンティティ作成
Entity player = world_.Create()
    .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
    .With<MeshRenderer>(DirectX::XMFLOAT3{0, 1, 0})
    .With<PlayerTag>()
    .With<PlayerController>()
    .Build();
```

### プロジェクト2: 敵の生成システム

**目標:** 一定時間ごとに敵を生成

```cpp
struct EnemySpawner : Behaviour {
    float spawnInterval = 2.0f; // 2秒ごと
    float timer = 0.0f;
    
    void OnUpdate(World& w, Entity self, float dt) override {
        timer += dt;
        
        if (timer >= spawnInterval) {
            timer = 0.0f;
            SpawnEnemy(w);
        }
    }
    
    void SpawnEnemy(World& w) {
        // ランダムな位置
        float x = (rand() % 10) - 5.0f;
        
        w.Create()
            .With<Transform>(DirectX::XMFLOAT3{x, 0, 10})
            .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
            .With<MoveForward>()
            .Build();
    }
};
```

---

## ?? 卒業試験

以下ができたら、ECSマスター！

### 必須課題
- [ ] 自分でBehaviourを3つ作る
- [ ] それらを組み合わせたエンティティを作る
- [ ] コンポーネント間で連携させる

### 発展課題
- [ ] 衝突判定システムを作る
- [ ] パーティクルシステムを作る
- [ ] UI表示システムを作る

---

## ?? 参考資料

### プロジェクト内
- `ComponentSamples.h` - コンポーネントの例
- `SampleScenes.h` - シーン作成の例
- `README.md` - 全体ガイド

### 外部資料
- DirectX11 公式ドキュメント
- ECS アーキテクチャ解説

---

**頑張ってください！ ??**

何か分からないことがあれば、コードのコメントを読んでみてください。  
すべてのコードに説明が書いてあります。
