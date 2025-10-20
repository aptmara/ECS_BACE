# 🎓 ECSアーキテクチャ学習プロジェクト

<div align="center">

![C++](https://img.shields.io/badge/C++-14-blue.svg)
![DirectX](https://img.shields.io/badge/DirectX-11-green.svg)
![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg)
![License](https://img.shields.io/badge/License-Free-brightgreen.svg)
![Version](https://img.shields.io/badge/Version-5.0-orange.svg)

**Entity Component System (ECS) を実践的に学ぶための教育プロジェクト**

[🚀 クイックスタート](#-実行方法) | [📖 学習ガイド](#-学習ガイド) | [🎮 サンプルゲーム](#-学習用ミニゲーム) | [📁 構成](#-プロジェクト構成)

</div>

---

## 📖 概要

<table>
<tr>
<td width="50%">

### 🎯 このプロジェクトで学べること

```
┌─────────────────────────────────┐
│   🏗️ ECS アーキテクチャ           │
├─────────────────────────────────┤
│  ✓ Entity（エンティティ）        │
│  ✓ Component（コンポーネント）   │
│  ✓ System（システム）            │
│  ✓ データ指向設計                │
└─────────────────────────────────┘
```

</td>
<td width="50%">

### 💡 プロジェクトの特徴

- 🔍 **可読性最重視** - 初心者に優しいコード
- 📚 **段階的学習** - レベル1〜9のサンプル
- 🎮 **実践例付き** - ミニゲームで体験
- 🛠️ **すぐ使える** - コピペ可能なコンポーネント集

</td>
</tr>
</table>

---

## 🎮 学習用ミニゲーム

<div align="center">

```
╔══════════════════════════════════════════╗
║   🎯 シンプルシューティングゲーム         ║
╠══════════════════════════════════════════╣
║                                          ║
║     🟢 プレイヤー（緑のキューブ）         ║
║        ↓  ↓  ↓                          ║
║     🟡 🟡 🟡  弾（黄色）                 ║
║        ↓  ↓  ↓                          ║
║     🔴 🔴 🔴  敵（赤）                   ║
║                                          ║
║  スコアを稼いでECSを体験しよう！          ║
║                                          ║
╚══════════════════════════════════════════╝
```

</div>

### 🎮 操作方法

<table align="center">
<tr>
<th>キー</th>
<th>動作</th>
<th>説明</th>
</tr>
<tr>
<td align="center"><kbd>A</kbd></td>
<td>← 左移動</td>
<td>プレイヤーを左に移動</td>
</tr>
<tr>
<td align="center"><kbd>D</kbd></td>
<td>→ 右移動</td>
<td>プレイヤーを右に移動</td>
</tr>
<tr>
<td align="center"><kbd>Space</kbd></td>
<td>🔫 発射</td>
<td>弾を発射して敵を撃つ</td>
</tr>
<tr>
<td align="center"><kbd>ESC</kbd></td>
<td>❌ 終了</td>
<td>ゲームを終了</td>
</tr>
</table>

### 🎯 ゲームルール

<div align="center">

| 🎯 目標 | 📊 スコア | ⚡ 難易度 |
|:---:|:---:|:---:|
| 敵を倒す | +10点 | ⭐⭐☆☆☆ |

</div>

---

## 🚀 実行方法

### 📋 必要な環境

<table>
<tr>
<td align="center">🖥️</td>
<td><b>OS</b></td>
<td>Windows 10/11</td>
</tr>
<tr>
<td align="center">🎨</td>
<td><b>GPU</b></td>
<td>DirectX 11 対応</td>
</tr>
<tr>
<td align="center">🔧</td>
<td><b>開発環境</b></td>
<td>Visual Studio 2019 以降</td>
</tr>
<tr>
<td align="center">⚙️</td>
<td><b>C++ 標準</b></td>
<td>C++14</td>
</tr>
</table>

### 🎬 起動手順

```
1️⃣  Visual Studio でプロジェクトを開く
      ↓
2️⃣  F5 キーを押してビルド＆実行
      ↓
3️⃣  ゲームで遊びながらECSを体験！
```

---

## 📁 プロジェクト構成

<details open>
<summary><b>🔵 ECSコアシステム</b>（基礎を学ぶ）</summary>

```
📂 include/ecs/
├── 🌍 World.h          ← ECSワールド管理（中核！）
│                         - エンティティ作成/破棄
│                         - コンポーネント追加/削除/取得
│                         - Behaviour更新ループ
│
├── 🆔 Entity.h         ← エンティティ定義
│                         - 一意なID
│                         - オブジェクトの識別子
│
└── 🏗️ EntityBuilder    ← ビルダーパターン実装
                          - メソッドチェーン
                          - 流暢なAPI
```

```
📂 include/components/
├── 📦 Component.h      ← コンポーネント基底クラス
│                         - IComponent（データ型）
│                         - Behaviour（動作型）
│
├── 📍 Transform.h      ← 位置・回転・スケール
├── 🎨 MeshRenderer.h   ← メッシュ描画
└── 🔄 Rotator.h        ← 回転コンポーネント（サンプル）
```

</details>

<details open>
<summary><b>📚 学習用サンプル</b>（コピペして使える）</summary>

```
📂 include/samples/
│
├── 🧩 ComponentSamples.h
│   ├── 💚 Health           ← 体力システム
│   ├── 🏃 Velocity         ← 速度システム
│   ├── 🎾 Bouncer          ← 上下に跳ねる
│   ├── ➡️  MoveForward      ← 前進する
│   ├── 💓 PulseScale       ← 拡大縮小
│   ├── 🌈 ColorCycle       ← 色を変化
│   ├── 🚶 RandomWalk       ← ランダム移動
│   └── ⏱️  LifeTime         ← 時間経過で削除
│
└── 🎬 SampleScenes.h
    ├── 📝 レベル1: 最もシンプルなエンティティ
    ├── 🔄 レベル2: 動きのあるエンティティ
    ├── 🎨 レベル3: カスタムBehaviour
    ├── 🎭 レベル4: 複数Behaviourの組み合わせ
    ├── 🔧 レベル5: 従来の方法
    ├── ✏️  レベル6: コンポーネントの変更
    ├── 🔍 レベル7: 全エンティティ処理
    ├── 🏗️  レベル8: デモシーン作成
    └── 🎓 レベル9: 練習問題の解答例
```

</details>

<details>
<summary><b>🎮 実践例（ミニゲーム）</b></summary>

```
📂 include/scenes/
│
├── 🎬 SceneManager.h    ← シーン切り替えシステム
│                          - IScene インターフェース
│                          - シーン登録・切り替え
│
└── 🎯 MiniGame.h        ← シューティングゲーム実装
    ├── 🟢 PlayerMovement    ← プレイヤー移動
    ├── 🟡 BulletMovement    ← 弾の移動
    ├── 🔴 EnemyMovement     ← 敵の移動
    ├── 🎯 UpdateShooting    ← 弾発射システム
    ├── 👾 UpdateEnemySpawning ← 敵生成システム
    └── 💥 CheckCollisions   ← 衝突判定システム
```

</details>

<details>
<summary><b>🎨 グラフィックス・入力</b></summary>

```
📂 include/graphics/
├── 🖥️  GfxDevice.h       ← DirectX11デバイス管理
├── 🎨 RenderSystem.h    ← レンダリングシステム
├── 📷 Camera.h          ← カメラ制御
└── 🐛 DebugDraw.h       ← デバッグ描画

📂 include/input/
└── 🎮 InputSystem.h     ← キーボード・マウス入力
```

</details>

<details>
<summary><b>🏗️ アプリケーション</b></summary>

```
📂 include/app/
└── 🚀 App.h             ← メインアプリケーション

📂 src/
└── 🎯 main.cpp          ← エントリーポイント
```

</details>

---

## 📖 学習ガイド

### 🗺️ 学習ロードマップ

```
┌────────────────────────────────────────────────────────┐
│                  ECS学習の旅 🗺️                         │
└────────────────────────────────────────────────────────┘
      ↓
┌─────────────────────┐
│  STEP 1: ECS基礎    │  ← ここからスタート！
│  📖 World.h         │
│  📖 Entity.h        │
│  ⏱️  30分           │
└─────────────────────┘
      ↓
┌─────────────────────┐
│  STEP 2: サンプル   │  ← 実例で理解
│  📚 SampleScenes.h  │
│  🎯 レベル1〜9      │
│  ⏱️  1時間          │
└─────────────────────┘
      ↓
┌─────────────────────┐
│  STEP 3: 部品集     │  ← 再利用可能な部品
│  🧩 ComponentSamples│
│  💡 8種類のコンポ   │
│  ⏱️  1時間          │
└─────────────────────┘
      ↓
┌─────────────────────┐
│  STEP 4: 実践例     │  ← ゲームでの使い方
│  🎮 MiniGame.h      │
│  🎯 完成したゲーム  │
│  ⏱️  2時間          │
└─────────────────────┘
      ↓
┌─────────────────────┐
│  STEP 5: 独自開発   │  ← オリジナルを作る！
│  🚀 あなたのゲーム  │
│  💪 実力アップ！    │
│  ⏱️  ∞              │
└─────────────────────┘
```

---

### 🎯 STEP 1: ECS基礎を理解する

<div align="center">

**⏱️ 所要時間: 30分 | 📖 推奨ファイル: `World.h`, `Entity.h`**

</div>

#### ECSの3つの要素

<table>
<tr>
<th width="33%">🆔 Entity</th>
<th width="33%">📦 Component</th>
<th width="33%">⚙️ System</th>
</tr>
<tr>
<td>

```cpp
// 一意なID
Entity player{ 1 };
```
オブジェクトの識別子

</td>
<td>

```cpp
// データの塊
struct Transform {
  XMFLOAT3 pos;
  XMFLOAT3 rot;
  XMFLOAT3 scale;
};
```
データと機能

</td>
<td>

```cpp
// 処理ロジック
world.ForEach<T>(
  [](Entity e, T& c) {
    // 処理
  }
);
```
コンポーネントを処理

</td>
</tr>
</table>

#### 🏗️ アーキテクチャ図

```
┌─────────────────────────────────────────────────────┐
│                   🌍 World (世界)                    │
│         すべてのEntityとComponentを管理              │
└─────────────────────────────────────────────────────┘
                        │
        ┌───────────────┼───────────────┐
        │               │               │
   ┌────▼────┐    ┌────▼────┐    ┌────▼────┐
   │Entity #1│    │Entity #2│    │Entity #3│
   │ (Player)│    │ (Enemy) │    │ (Bullet)│
   └─────────┘    └─────────┘    └─────────┘
        │               │               │
   ┌────┴────┐     ┌────┴────┐     ┌────┴────┐
   │📍Transform│     │📍Transform│     │📍Transform│
   │🎨Renderer │     │🎨Renderer │     │🎨Renderer │
   │🎮Movement │     │👾AI      │     │🚀Speed   │
   │💚Health  │     │💚Health  │     │⏱️LifeTime│
   └─────────┘     └─────────┘     └─────────┘
```

#### 💻 基本的なコード例

```cpp
// ✨ ビルダーパターン（推奨）
Entity player = world.Create()
    .With<Transform>(XMFLOAT3{0, 0, 0})
    .With<MeshRenderer>(XMFLOAT3{0, 1, 0})  // 緑色
    .With<Player>()
    .Build();

// 🔍 コンポーネントの取得
auto* transform = world.TryGet<Transform>(player);
if (transform) {
    transform->position.x += 1.0f;
}

// 🔄 全エンティティを処理
world.ForEach<Transform>([](Entity e, Transform& t) {
    t.position.y += 0.01f;  // 全部を上に移動
});

// ⏱️ 毎フレーム更新
world.Tick(deltaTime);
```

---

### 🎯 STEP 2: サンプルコードを読む

<div align="center">

**⏱️ 所要時間: 1時間 | 📖 推奨ファイル: `SampleScenes.h`**

</div>

#### 📊 レベル別学習マップ

<table>
<tr>
<td align="center">

**🌱 初級**<br/>
レベル1〜3

</td>
<td align="center">

**🌿 中級**<br/>
レベル4〜6

</td>
<td align="center">

**🌳 上級**<br/>
レベル7〜9

</td>
</tr>
<tr>
<td>

- シンプルなエンティティ
- 動きのあるエンティティ
- カスタムBehaviour

</td>
<td>

- 複数Behaviour
- 従来の方法
- 動的な変更

</td>
<td>

- 全エンティティ処理
- デモシーン作成
- 練習問題

</td>
</tr>
</table>

#### 📝 レベル1の例: 最もシンプルなキューブ

```cpp
Entity cube = world.Create()
    .With<Transform>(
        XMFLOAT3{0.0f, 0.0f, 0.0f},  // 📍 位置
        XMFLOAT3{0.0f, 0.0f, 0.0f},  // 🔄 回転
        XMFLOAT3{1.0f, 1.0f, 1.0f}   // 📏 スケール
    )
    .With<MeshRenderer>(XMFLOAT3{1.0f, 0.0f, 0.0f})  // 🎨 赤色
    .Build();
```

---

### 🎯 STEP 3: カスタムコンポーネントを作る

<div align="center">

**⏱️ 所要時間: 1時間 | 📖 推奨ファイル: `ComponentSamples.h`**

</div>

#### 📦 2種類のコンポーネント

<table>
<tr>
<th width="50%">💾 データ型（状態のみ）</th>
<th width="50%">⚡ Behaviour型（動作あり）</th>
</tr>
<tr>
<td valign="top">

```cpp
struct Health : IComponent {
    float current = 100.0f;
    float max = 100.0f;
    
    void TakeDamage(float dmg) {
        current -= dmg;
        if (current < 0) 
            current = 0;
    }
};
```

**特徴:**
- ✅ データのみ保持
- ✅ 他から操作される
- ✅ メモリ効率的

</td>
<td valign="top">

```cpp
struct Bouncer : Behaviour {
    float speed = 2.0f;
    float time = 0.0f;
    
    void OnUpdate(World& w, 
                  Entity self, 
                  float dt) override {
        time += dt * speed;
        auto* t = w.TryGet<Transform>(self);
        if (t) {
            t->position.y = sin(time);
        }
    }
};
```

**特徴:**
- ✅ 毎フレーム自動更新
- ✅ 自律的な動作
- ✅ ゲームロジック実装

</td>
</tr>
</table>

#### 🧩 利用可能なコンポーネント一覧

<div align="center">

| アイコン | 名前 | 説明 | 使用例 |
|:---:|:---:|:---:|:---:|
| 💚 | Health | 体力管理 | RPG、シューティング |
| 🏃 | Velocity | 速度ベクトル | 物理演算 |
| 🎾 | Bouncer | 上下に跳ねる | プラットフォーム |
| ➡️ | MoveForward | 前進する | 弾、敵 |
| 💓 | PulseScale | 大きさ変化 | アイテム、エフェクト |
| 🌈 | ColorCycle | 色を変化 | 演出、UI |
| 🚶 | RandomWalk | ランダム移動 | NPC、敵AI |
| ⏱️ | LifeTime | 時間で削除 | エフェクト、弾 |

</div>

---

### 🎯 STEP 4: 実践例を読む

<div align="center">

**⏱️ 所要時間: 2時間 | 📖 推奨ファイル: `MiniGame.h`**

</div>

#### 🎮 ゲームの構造

```
┌────────────────────────────────────────┐
│         GameScene (ゲームシーン)         │
├────────────────────────────────────────┤
│                                        │
│  🟢 Player System                      │
│     └─ PlayerMovement (移動)          │
│                                        │
│  🟡 Bullet System                      │
│     ├─ BulletMovement (飛行)          │
│     └─ UpdateShooting (発射)          │
│                                        │
│  🔴 Enemy System                       │
│     ├─ EnemyMovement (降下)           │
│     └─ UpdateEnemySpawning (生成)     │
│                                        │
│  💥 Collision System                   │
│     └─ CheckCollisions (衝突判定)     │
│                                        │
│  📊 Score System                       │
│     └─ スコア管理                      │
│                                        │
└────────────────────────────────────────┘
```

#### 💡 実装例: プレイヤー移動

```cpp
struct PlayerMovement : Behaviour {
    float speed = 8.0f;
    InputSystem* input_;
    
    void OnUpdate(World& w, Entity self, float dt) override {
        auto* t = w.TryGet<Transform>(self);
        if (!t) return;
        
        // ⌨️ キーボード入力で移動
        if (input_->GetKey('A')) t->position.x -= speed * dt;
        if (input_->GetKey('D')) t->position.x += speed * dt;
        
        // 🚧 画面外に出ないように制限
        if (t->position.x < -10.0f) t->position.x = -10.0f;
        if (t->position.x > 10.0f) t->position.x = 10.0f;
    }
};
```

---

## 🔧 カスタマイズ例

### 🎨 簡単なカスタマイズ

<table>
<tr>
<th>変更内容</th>
<th>場所</th>
<th>変更例</th>
</tr>
<tr>
<td>🏃 プレイヤー速度</td>
<td><code>MiniGame.h</code></td>
<td>

```cpp
float speed = 8.0f;
// ↓ 速くする
float speed = 12.0f;
```

</td>
</tr>
<tr>
<td>👾 敵の出現間隔</td>
<td><code>MiniGame.h</code></td>
<td>

```cpp
if (timer >= 1.0f)
// ↓ 速く出現
if (timer >= 0.5f)
```

</td>
</tr>
<tr>
<td>🔫 弾の速度</td>
<td><code>MiniGame.h</code></td>
<td>

```cpp
float speed = 15.0f;
// ↓ 速い弾
float speed = 25.0f;
```

</td>
</tr>
</table>

### 🚀 機能追加の例

<details>
<summary><b>💚 体力システムを追加する</b></summary>

```cpp
// 1️⃣ コンポーネント定義
struct Health : IComponent {
    int hp = 3;
    int maxHp = 3;
};

// 2️⃣ プレイヤーに追加
playerEntity_ = world.Create()
    .With<Transform>(XMFLOAT3{0, -5, 0})
    .With<MeshRenderer>(XMFLOAT3{0, 1, 0})
    .With<Player>()
    .With<Health>()  // ← 追加
    .Build();

// 3️⃣ 衝突時にダメージ
void OnEnemyHit(Entity player) {
    auto* health = world.TryGet<Health>(player);
    if (health) {
        health->hp--;
        if (health->hp <= 0) {
            // 💀 ゲームオーバー処理
            SceneManager::LoadScene("GameOver");
        }
    }
}
```

</details>

<details>
<summary><b>⚡ パワーアップアイテムを追加する</b></summary>

```cpp
// 1️⃣ パワーアップタグ
struct PowerUp : IComponent {
    enum Type { SPEED, RAPID_FIRE, SHIELD };
    Type type = SPEED;
};

// 2️⃣ アイテム生成
Entity CreatePowerUp(World& world, PowerUp::Type type) {
    return world.Create()
        .With<Transform>(XMFLOAT3{0, 5, 0})
        .With<MeshRenderer>(XMFLOAT3{1, 1, 0})  // 黄色
        .With<PowerUp>(type)
        .With<MoveForward>(-2.0f)  // 下に落ちる
        .Build();
}

// 3️⃣ 取得処理
void CheckPowerUpCollision() {
    world.ForEach<PowerUp>([&](Entity item, PowerUp& pu) {
        if (CollidesWith(playerEntity_, item)) {
            ApplyPowerUp(pu.type);
            world.DestroyEntity(item);
        }
    });
}
```

</details>

---

## 🎓 ECSアーキテクチャの理解

### 🆚 従来のOOP vs ECS

<table>
<tr>
<th width="50%">🏛️ 従来のOOP（継承ベース）</th>
<th width="50%">🎯 ECS（コンポーネントベース）</th>
</tr>
<tr>
<td valign="top">

```cpp
class GameObject {
    Transform transform;
    Renderer renderer;
};

class Player : GameObject {
    void Move() { ... }
    void Shoot() { ... }
};

class Enemy : GameObject {
    void AI() { ... }
    void Attack() { ... }
};
```

**❌ 問題点:**
- 継承の深さが増える
- 機能の再利用が困難
- メモリレイアウトが非効率

</td>
<td valign="top">

```cpp
// 🟢 プレイヤー
Entity player = world.Create()
    .With<Transform>()
    .With<Renderer>()
    .With<PlayerMovement>()
    .With<Shooter>()
    .Build();

// 🔴 敵
Entity enemy = world.Create()
    .With<Transform>()
    .With<Renderer>()
    .With<EnemyAI>()
    .With<Attacker>()
    .Build();
```

**✅ メリット:**
- 柔軟な組み合わせ
- 機能の再利用が容易
- データ指向で高速

</td>
</tr>
</table>

### 💎 ECSのメリット

<div align="center">

```
┌─────────────────────────────────────────────┐
│          🌟 ECSの5大メリット 🌟              │
├─────────────────────────────────────────────┤
│                                             │
│  1️⃣  柔軟性        コンポーネントの自由な組合せ  │
│  2️⃣  再利用性      同じコンポーネントを再利用   │
│  3️⃣  保守性        機能が独立していて変更容易   │
│  4️⃣  パフォーマンス  データ指向で高速処理       │
│  5️⃣  拡張性        新機能の追加が簡単         │
│                                             │
└─────────────────────────────────────────────┘
```

</div>

---

## 🐛 デバッグ方法

### 🔍 デバッグ機能

<table>
<tr>
<td width="50%">

#### 📊 デバッグ描画

`_DEBUG` モードで有効:

```
      Y軸（緑）
       ↑
       │
       │
       └────→ X軸（赤）
      ╱
     ╱
    ↙ Z軸（青）

   ┌─┬─┬─┬─┐
   │ │ │ │ │  グリッド
   ├─┼─┼─┼─┤
   │ │🟢│ │ │  オブジェクト
   ├─┼─┼─┼─┤
```

</td>
<td width="50%">

#### 🎯 推奨ブレークポイント

| 場所 | タイミング |
|:---|:---|
| `GameScene::OnUpdate()` | 毎フレーム |
| `World::Tick()` | Behaviour更新 |
| `CheckCollisions()` | 衝突判定 |
| `UpdateShooting()` | 弾発射 |

</td>
</tr>
</table>

### 💡 デバッグのコツ

```cpp
// ✅ エンティティの存在確認
if (!world.IsAlive(entity)) {
    // エンティティが削除されている！
}

// ✅ コンポーネントの確認
auto* transform = world.TryGet<Transform>(entity);
if (!transform) {
    // Transformコンポーネントがない！
}

// ✅ 値の確認
if (transform) {
    float x = transform->position.x;  // ← ブレークポイント
    float y = transform->position.y;
    float z = transform->position.z;
}
```

---

## ❓ トラブルシューティング

<table>
<tr>
<th>😱 問題</th>
<th>💡 解決方法</th>
</tr>
<tr>
<td>

**ビルドエラー**
```
error C2039: 'XMFLOAT3': ...
```

</td>
<td>

- Windows SDK 10 をインストール
- プロジェクト設定で C++14 を確認
- インクルードパスを確認

</td>
</tr>
<tr>
<td>

**起動しない**
```
アプリケーションエラー
```

</td>
<td>

- DirectX 11 対応GPUを確認
- グラフィックスドライバー更新
- Visual C++ 再頒布可能パッケージ

</td>
</tr>
<tr>
<td>

**動作が重い**

FPS が低い

</td>
<td>

- `Release` モードでビルド
- デバッグ描画を無効化
- V-Sync 設定を確認

</td>
</tr>
</table>

---

## 🎯 次のステップ

### 🌱 レベル1: ゲームの改造

<table>
<tr>
<td align="center">⭐</td>
<td><b>パワーアップアイテム</b></td>
<td>スピードアップ、連射など</td>
</tr>
<tr>
<td align="center">⭐⭐</td>
<td><b>複数の敵タイプ</b></td>
<td>速い敵、硬い敵、ボス</td>
</tr>
<tr>
<td align="center">⭐⭐⭐</td>
<td><b>エフェクト追加</b></td>
<td>爆発、パーティクル</td>
</tr>
</table>

### 🌿 レベル2: システムの拡張

<table>
<tr>
<td align="center">⭐⭐</td>
<td><b>タイトル画面</b></td>
<td>シーン管理を活用</td>
</tr>
<tr>
<td align="center">⭐⭐</td>
<td><b>ハイスコア保存</b></td>
<td>ファイルI/O実装</td>
</tr>
<tr>
<td align="center">⭐⭐⭐</td>
<td><b>サウンドシステム</b></td>
<td>BGM、効果音</td>
</tr>
</table>

### 🌳 レベル3: ECSの深堀り

<table>
<tr>
<td align="center">⭐⭐⭐</td>
<td><b>アーキタイプECS</b></td>
<td>より効率的な実装</td>
</tr>
<tr>
<td align="center">⭐⭐⭐⭐</td>
<td><b>マルチスレッド</b></td>
<td>並列処理で高速化</td>
</tr>
<tr>
<td align="center">⭐⭐⭐⭐⭐</td>
<td><b>データ指向最適化</b></td>
<td>キャッシュフレンドリー</td>
</tr>
</table>

### 🚀 レベル4: 独自プロジェクト

<div align="center">

```
🎮 学んだECSで、オリジナルゲームを作ろう！

  ┌─────────┐   ┌─────────┐   ┌─────────┐
  │  RPG    │   │ パズル  │   │  STG    │
  └─────────┘   └─────────┘   └─────────┘
  ┌─────────┐   ┌─────────┐   ┌─────────┐
  │ アクション│   │ タワーD │   │  ?????  │
  └─────────┘   └─────────┘   └─────────┘

         あなたのアイデアを形にしよう！ 💡
```

</div>

---

## 📚 参考リソース

### 🔗 推奨リンク

<table>
<tr>
<td>📖 <b>Unity ECS</b></td>
<td><a href="https://docs.unity3d.com/Packages/com.unity.entities@latest">公式ドキュメント</a></td>
</tr>
<tr>
<td>📖 <b>Game Programming Patterns</b></td>
<td>Component パターン解説</td>
</tr>
<tr>
<td>🎥 <b>YouTube</b></td>
<td>ECS アーキテクチャ解説動画</td>
</tr>
<tr>
<td>💬 <b>Discord/Reddit</b></td>
<td>ゲーム開発コミュニティ</td>
</tr>
</table>

---

## 📄 ライセンス

<div align="center">

```
┌─────────────────────────────────────┐
│     📜 このプロジェクトは          │
│     学習用として自由に使えます      │
│                                     │
│  ✅ 改変 OK                         │
│  ✅ 再配布 OK                       │
│  ✅ 商用利用 OK                     │
│                                     │
│  自由に学んで、作って、シェアしよう！ │
└─────────────────────────────────────┘
```

</div>

---

## 👤 作成者

<div align="center">

**山内陽** 🧑‍💻

### 📊 バージョン履歴

| バージョン | リリース日 | 内容 |
|:---:|:---:|:---|
| v5.0 | 2025 | ✨ ミニゲーム実装版（可読性最重視） |
| v4.0 | 2025 | 📚 段階的学習用サンプル集 |
| v3.0 | 2025 | 🏗️ ECSコアシステム |
| v2.0 | 2025 | 🎨 DirectX11統合 |
| v1.0 | 2025 | 🎉 初期リリース |

</div>

---

## 🙏 謝辞

<div align="center">

このプロジェクトは、**Entity Component System** アーキテクチャの
教育目的で作成されました。

ECSの概念を実践的に学べるよう、
**シンプルさ**と**可読性**を最優先に設計しています。

---

### 🌟 Happy Learning! 🎓

質問やフィードバックがあれば、
Issue や Pull Request でお気軽にどうぞ！

[![GitHub](https://img.shields.io/badge/GitHub-ECS__BACE-blue?logo=github)](https://github.com/aptmara/ECS_BACE)

---

**Let's Build Amazing Games with ECS! 🎮✨**

</div>
