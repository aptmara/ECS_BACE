<div align="center">

# 🎮 HEW_ECS

### Entity Component System ゲーム開発フレームワーク

[![C++14](https://img.shields.io/badge/C++-14-blue.svg?style=flat&logo=c%2B%2B)](https://isocpp.org/)
[![DirectX](https://img.shields.io/badge/DirectX-11-green.svg?style=flat)](https://docs.microsoft.com/en-us/windows/win32/directx)
[![Platform](https://img.shields.io/badge/Platform-Windows-lightgrey.svg?style=flat)](https://www.microsoft.com/windows)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg?style=flat)](LICENSE)
[![Build](https://img.shields.io/badge/Build-Passing-brightgreen.svg?style=flat)](https://github.com/aptma-sHEWTeam/HEW_ECS)

**ECS（Entity Component System）アーキテクチャ**を活用した柔軟で拡張性の高いゲーム開発プロジェクト

[特徴](#-主な特徴) • [クイックスタート](#-クイックスタート) • [ドキュメント](#-目次) • [サンプル](#-実践例)

</div>

---

## 📋 目次

<details open>
<summary><b>クリックして展開/折りたたみ</b></summary>

- [🎯 プロジェクト概要](#-プロジェクト概要)
- [✨ 主な特徴](#-主な特徴)
- [🚀 クイックスタート](#-クイックスタート)
- [🛠 環境構築](#-環境構築)
- [📚 ECSアーキテクチャ入門](#-ecsアーキテクチャ入門)
- [🔧 コンポーネントの作り方](#-コンポーネントの作り方)
- [🎮 エンティティの作成](#-エンティティの作成)
- [💡 実践例](#-実践例)
- [📝 コーディング規約](#-コーディング規約)
- [👥 チーム開発ルール](#-チーム開発ルール)
- [📖 参考資料](#-参考資料)

</details>

---

## 🎯 プロジェクト概要

<table>
<tr>
<td width="50%">

**プロジェクト情報**

| 項目 | 内容 |
|------|------|
| **名称** | HEW_ECS (ECS_BACE) |
| **目的** | ECSを活用したチームゲーム開発 |
| **言語** | C++14 |
| **プラットフォーム** | Windows (DirectX 11) |
| **アーキテクチャ** | Entity Component System |

</td>
<td width="50%">

**プロジェクト統計**

```
📦 コンポーネント数:    15+
🎯 エンティティ管理:    動的
🔄 フレーム管理:       自動
📊 メモリ管理:         スマートポインタ
```

</td>
</tr>
</table>

---

## ✨ 主な特徴

<div align="center">

| 🎨 柔軟な設計 | ♻️ 再利用性 | 🔧 保守性 | 📈 拡張性 |
|:---:|:---:|:---:|:---:|
| コンポーネントの<br>組み合わせで機能実装 | 汎用コンポーネントを<br>複数のエンティティで共有 | 責任の分離により<br>バグ特定が容易 | 新しいコンポーネント追加で<br>機能拡張が可能 |

</div>

### 🌟 ECSの利点

```mermaid
graph LR
    A[Entity<br>識別子] --> B[Transform<br>位置・回転]
    A --> C[MeshRenderer<br>描画]
    A --> D[Rotator<br>回転動作]
    A --> E[PlayerTag<br>識別]
    
    style A fill:#4CAF50
    style B fill:#2196F3
    style C fill:#2196F3
    style D fill:#FF9800
    style E fill:#9C27B0
```

---

## 🚀 クイックスタート

### ⚡ 30秒で始める

```cpp
// 1️⃣ ワールドを作成
World world;

// 2️⃣ エンティティを作成（ビルダーパターン）
Entity player = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
    .With<MeshRenderer>(DirectX::XMFLOAT3{0, 1, 0})
    .With<Rotator>(45.0f)
    .Build();

// 3️⃣ 毎フレーム更新
world.Tick(deltaTime);
```

> 💡 **たったこれだけ！** 回転する緑のキューブが画面に表示されます。

---

## 🛠 環境構築

### 📋 必要な環境

<table>
<tr>
<td align="center" width="25%">

**💻 OS**

Windows 10/11

</td>
<td align="center" width="25%">

**🔨 IDE**

Visual Studio<br>2019/2022

</td>
<td align="center" width="25%">

**📦 SDK**

Windows SDK<br>(DirectX 11)

</td>
<td align="center" width="25%">

**📚 C++**

C++14

</td>
</tr>
</table>

### 📥 インストール手順

```bash
# 1️⃣ リポジトリをクローン
git clone https://github.com/aptma-sHEWTeam/HEW_ECS.git

# 2️⃣ ディレクトリに移動
cd HEW_ECS

# 3️⃣ Visual Studioでソリューションを開く
start ECS_BACE.sln
```

### ▶️ ビルド & 実行

| 操作 | ショートカット |
|------|---------------|
| ビルド | `F7` または `Ctrl+Shift+B` |
| 実行（デバッグ） | `F5` |
| 実行（非デバッグ） | `Ctrl+F5` |

---

## 📚 ECSアーキテクチャ入門

<div align="center">

### 🏗️ ECSの3要素

```
┌─────────────┐     ┌─────────────────┐     ┌──────────────┐
│   Entity    │────▶│   Component     │────▶│   System     │
│  (識別子)    │     │ (データ・動作)   │     │  (処理)      │
└─────────────┘     └─────────────────┘     └──────────────┘
      ID                データ保持               ロジック実行
```

</div>

### 1️⃣ Entity（エンティティ）

> **一意なID（識別子）のみを持つオブジェクト**

```cpp
struct Entity {
    uint32_t id;   // エンティティID
    uint32_t gen;  // 世代番号（削除時に使用）
};
```

<details>
<summary>📖 詳細を見る</summary>

- ✅ データやロジックは一切持たない
- ✅ コンポーネントの「入れ物」として機能
- ✅ 例: プレイヤー、敵、弾丸、アイテムなど

**特徴**
- **軽量**: IDと世代番号のみ
- **安全**: 世代番号で古いハンドルを無効化
- **柔軟**: コンポーネントの組み合わせで機能定義

</details>

---

### 2️⃣ Component（コンポーネント）

> **データまたは動作を表す部品**

#### 📦 データコンポーネント（IComponent継承）

データのみを保持し、ロジックは含まない

```cpp
struct Transform : IComponent {
    DirectX::XMFLOAT3 position{0, 0, 5};  // 位置
    DirectX::XMFLOAT3 rotation{0, 0, 0};  // 回転
    DirectX::XMFLOAT3 scale{1, 1, 1};     // スケール
};
```

<details>
<summary>📦 その他のデータコンポーネント例</summary>

```cpp
// 体力管理
struct Health : IComponent {
    float current = 100.0f;
    float max = 100.0f;
};

// 速度ベクトル
struct Velocity : IComponent {
    DirectX::XMFLOAT3 velocity{0, 0, 0};
};

// スコア
DEFINE_DATA_COMPONENT(Score,
    int points = 0;
);
```

</details>

#### ⚙️ Behaviourコンポーネント（Behaviour継承）

毎フレーム更新されるロジックを持つ

```cpp
struct Rotator : Behaviour {
    float speedDegY = 45.0f;  // 回転速度（度/秒）
    
    void OnUpdate(World& w, Entity self, float dt) override {
        auto* t = w.TryGet<Transform>(self);
        if (t) {
            t->rotation.y += speedDegY * dt;
        }
    }
};
```

<details>
<summary>⚙️ その他のBehaviour例</summary>

```cpp
// 上下に跳ねる動き
struct Bouncer : Behaviour {
    float speed = 2.0f;
    float amplitude = 2.0f;
    
    void OnUpdate(World& w, Entity self, float dt) override {
        time += dt * speed;
        auto* t = w.TryGet<Transform>(self);
        if (t) {
            t->position.y = startY + sinf(time) * amplitude;
        }
    }
};

// 前進移動
struct MoveForward : Behaviour {
    float speed = 5.0f;
    
    void OnUpdate(World& w, Entity self, float dt) override {
        auto* t = w.TryGet<Transform>(self);
        if (t) {
            t->position.z += speed * dt;
        }
    }
};
```

</details>

---

### 3️⃣ System（システム）

> **コンポーネントに対する処理ロジック**

#### 方法1: Behaviourパターン（推奨）

```cpp
struct MyBehaviour : Behaviour {
    void OnStart(World& w, Entity self) override {
        // 初回起動時に1度だけ実行
    }
    
    void OnUpdate(World& w, Entity self, float dt) override {
        // 毎フレーム実行される処理
    }
};
```

#### 方法2: ForEachパターン

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

## 🔧 コンポーネントの作り方

### 方法1: 構造体で定義（基本）

<table>
<tr>
<td width="50%">

**📦 データコンポーネント**

```cpp
struct Health : IComponent {
    float current = 100.0f;
    float max = 100.0f;
    
    void TakeDamage(float dmg) {
        current -= dmg;
        if (current < 0.0f) 
            current = 0.0f;
    }
    
    bool IsDead() const {
        return current <= 0.0f;
    }
};
```

</td>
<td width="50%">

**⚙️ Behaviourコンポーネント**

```cpp
struct Bouncer : Behaviour {
    float speed = 2.0f;
    float amplitude = 2.0f;
    float time = 0.0f;
    
    void OnUpdate(World& w, 
                  Entity self, 
                  float dt) override {
        time += dt * speed;
        auto* t = w.TryGet<Transform>(self);
        if (t) {
            t->position.y = 
                startY + sinf(time) * amplitude;
        }
    }
};
```

</td>
</tr>
</table>

---

### 方法2: マクロで定義（簡潔）

#### 🎯 DEFINE_DATA_COMPONENT

```cpp
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

#### 🎯 DEFINE_BEHAVIOUR

```cpp
DEFINE_BEHAVIOUR(SpinAndColor,
    // 🔹 メンバ変数
    float rotSpeed = 90.0f;
    float colorSpeed = 1.0f;
    float time = 0.0f;
,
    // 🔹 OnUpdate内の処理
    time += dt * colorSpeed;
    
    auto* t = w.TryGet<Transform>(self);
    if (t) {
        t->rotation.y += rotSpeed * dt;
    }
    
    auto* mr = w.TryGet<MeshRenderer>(self);
    if (mr) {
        float hue = fmodf(time, 1.0f);
        mr->color.x = sinf(hue * 6.28f) * 0.5f + 0.5f;
        mr->color.y = cosf(hue * 6.28f) * 0.5f + 0.5f;
    }
);
```

---

### 方法3: タグコンポーネント（マーカー）

エンティティの種類を識別するための空のコンポーネント

```cpp
struct PlayerTag : IComponent {};
struct EnemyTag : IComponent {};
struct BulletTag : IComponent {};
```

**使用例：**

```cpp
// プレイヤーだけを処理
world.ForEach<PlayerTag, Transform>([](Entity e, PlayerTag& tag, Transform& t) {
    // プレイヤー限定の処理
});
```

---

## 🎮 エンティティの作成

### 方法1: ビルダーパターン（推奨） ⭐

**メソッドチェーンで直感的にコンポーネントを追加**

```cpp
Entity player = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
    .With<MeshRenderer>(DirectX::XMFLOAT3{0, 1, 0})
    .With<Rotator>(45.0f)
    .With<PlayerTag>()
    .Build();  // ← Build()は省略可能
```

> 💡 **Tip**: `.Build()`は省略可能（暗黙的にEntity型に変換）

```cpp
// これでもOK！
Entity player = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
    .With<MeshRenderer>(DirectX::XMFLOAT3{0, 1, 0});
```

---

### 方法2: 従来の方法

```cpp
// ステップ1: エンティティを作成
Entity enemy = world.CreateEntity();

// ステップ2: コンポーネントを個別に追加
world.Add<Transform>(enemy, Transform{});
world.Add<MeshRenderer>(enemy, MeshRenderer{DirectX::XMFLOAT3{1, 0, 0}});
world.Add<EnemyTag>(enemy, EnemyTag{});
```

---

### 方法3: 遅延スポーン（並列処理対応） 🔒

```cpp
// スポーン要求をキューに追加（スレッドセーフ）
world.EnqueueSpawn(World::Cause::Spawner, [](Entity e) {
    // 生成後の初期化（メインスレッドで実行される）
    // ここでコンポーネントを追加
});
```

---

## 💡 実践例

### 例1: シンプルな回転キューブ 🎲

```cpp
Entity CreateRotatingCube(World& world) {
    return world.Create()
        .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
        .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})  // 赤色
        .With<Rotator>(90.0f)  // 90度/秒で回転
        .Build();
}
```

<div align="center">

**実行結果**

🔴 ← 回転する赤いキューブ

</div>

---

### 例2: プレイヤーキャラクター 🎮

```cpp
struct PlayerMovement : Behaviour {
    InputSystem* input_ = nullptr;
    float speed = 5.0f;
    
    void OnUpdate(World& w, Entity self, float dt) override {
        auto* t = w.TryGet<Transform>(self);
        if (!t || !input_) return;
        
        if (input_->GetKey('W')) t->position.z += speed * dt;  // 前進
        if (input_->GetKey('S')) t->position.z -= speed * dt;  // 後退
        if (input_->GetKey('A')) t->position.x -= speed * dt;  // 左移動
        if (input_->GetKey('D')) t->position.x += speed * dt;  // 右移動
    }
};

Entity CreatePlayer(World& world, InputSystem* input) {
    Entity player = world.Create()
        .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
        .With<MeshRenderer>(DirectX::XMFLOAT3{0, 1, 0})  // 緑色
        .With<PlayerTag>()
        .Build();
    
    // PlayerMovementは後から追加し、inputを設定
    auto& movement = world.Add<PlayerMovement>(player);
    movement.input_ = input;
    
    return player;
}
```

<div align="center">

**操作方法**

`W` / `A` / `S` / `D` キーで移動

</div>

---

### 例3: 時間経過で消える弾丸 💥

```cpp
struct LifeTime : Behaviour {
    float remainingTime = 3.0f;
    
    void OnUpdate(World& w, Entity self, float dt) override {
        remainingTime -= dt;
        if (remainingTime <= 0.0f) {
            w.DestroyEntityWithCause(self, World::Cause::LifetimeExpired);
        }
    }
};

Entity CreateBullet(World& world, const DirectX::XMFLOAT3& pos) {
    return world.Create()
        .With<Transform>(pos)
        .With<MeshRenderer>(DirectX::XMFLOAT3{1, 1, 0})  // 黄色
        .With<MoveForward>(10.0f)  // 前進速度
        .With<LifeTime>(3.0f)      // 3秒後に削除
        .With<BulletTag>()
        .Build();
}
```

<div align="center">

**動作**

🟡 → → → 💨 (3秒後に消滅)

</div>

---

### 例4: コンポーネントの取得と変更 🔍

```cpp
// ✅ 安全な取得（TryGet推奨）
auto* transform = world.TryGet<Transform>(entity);
if (transform) {
    transform->position.x += 1.0f;
}

// ✅ 存在確認
if (world.Has<Health>(entity)) {
    auto* health = world.TryGet<Health>(entity);
    health->TakeDamage(10.0f);
}

// ✅ 複数コンポーネントの取得
auto* t = world.TryGet<Transform>(entity);
auto* mr = world.TryGet<MeshRenderer>(entity);
if (t && mr) {
    // 両方存在する場合の処理
}
```

---

### 例5: ForEachでの一括処理 🔄

```cpp
// 単一コンポーネント
world.ForEach<Transform>([](Entity e, Transform& t) {
    t.position.y += 0.1f;  // 全エンティティを上に移動
});

// 複数コンポーネント
world.ForEach<Transform, Velocity>([dt](Entity e, Transform& t, Velocity& v) {
    t.position.x += v.velocity.x * dt;
    t.position.y += v.velocity.y * dt;
    t.position.z += v.velocity.z * dt;
});
```

---

### 例6: エンティティの削除 🗑️

```cpp
// 即座に削除（フレーム終了時に実際に削除される）
world.DestroyEntity(entity);

// 原因付きで削除（デバッグログに記録される）
world.DestroyEntityWithCause(entity, World::Cause::Collision);

// コンポーネントのみ削除
world.Remove<Health>(entity);
```

---

## 📝 コーディング規約

### 🏷️ 命名規約

<div align="center">

| 要素 | 規約 | 例 |
|:----:|:----:|:--:|
| **クラス名** | PascalCase | `Transform`, `MeshRenderer` |
| **関数名** | PascalCase | `CreateEntity`, `TryGet` |
| **変数名** | camelCase | `deltaTime`, `entityId` |
| **メンバ変数** | camelCase + `_` | `world_`, `nextId_` |
| **定数** | UPPER_SNAKE_CASE | `MAX_ENTITIES` |

</div>

---

### 📚 C++14準拠

```cpp
// ✅ 正しい: C++14互換
std::vector<Entity> entities;
std::unique_ptr<Transform> transform;

// ❌ 間違い: C++17機能は使用不可
std::optional<Transform> GetTransform(Entity e);  // C++17
if constexpr (condition) { }                      // C++17
std::filesystem::path p;                          // C++17
```

---

### 🎨 DirectXMath の使用

```cpp
// ✅ 推奨: XMFLOAT3でデータ保持
struct Transform : IComponent {
    DirectX::XMFLOAT3 position{0, 0, 0};
};

// ✅ 計算時はXMVECTORを使用（SIMD最適化）
DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&transform.position);
DirectX::XMVECTOR dir = DirectX::XMLoadFloat3(&direction);
DirectX::XMVECTOR result = DirectX::XMVectorAdd(pos, dir);
DirectX::XMStoreFloat3(&transform.position, result);
```

---

### 📄 コメント規約（Doxygen形式）

```cpp
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
 * 注意点や制限事項を記述します。
 * 
 * @note 補足情報
 * @warning 警告事項
 * @author 山内陽
 */
ReturnType FunctionName(Type input, Type& output, Type& inout);
```

---

## 👥 チーム開発ルール

### 📂 ファイル編集の優先順位

#### 🔒 コアシステム（触らない）

> ⚠️ **注意**: 以下のファイルは**変更する場合はチーム全体で相談**

```
include/ecs/World.h           # ECSコアシステム
include/ecs/Entity.h          # エンティティ定義
include/components/Component.h # コンポーネント基底クラス
include/components/Transform.h # Transform定義
```

#### ✅ 自由に編集可能

```
include/scenes/              # ゲームシーンの実装
include/components/Custom*.h # カスタムコンポーネント
src/                        # 実装ファイル
```

#### ⚠️ 要相談

```
include/graphics/  # グラフィックスシステム
include/input/     # 入力システム
include/app/       # アプリケーション基盤
```

---

### 🔀 Git/GitHubのルール

---

## 🌟 GitHub初心者ガイド

<div align="center">

### 🤔 GitHubって何？

**GitHub**は、プログラムのソースコードを保存・管理・共有できるオンラインサービスです。  
チームで開発する際に、**誰がいつどこを変更したか**を記録し、**複数人が同時に作業しても混乱しない**ようにしてくれます。

</div>

---

### 📖 重要な用語集（初心者向け）

<table>
<tr>
<th width="25%">用語</th>
<th width="75%">説明</th>
</tr>
<tr>
<td><b>🗂️ リポジトリ<br>(Repository)</b></td>
<td>プロジェクトのファイルを保存する場所。<b>プロジェクトフォルダ</b>のようなもの。<br>例: <code>HEW_ECS</code> がこのプロジェクトのリポジトリ</td>
</tr>
<tr>
<td><b>📥 クローン<br>(Clone)</b></td>
<td>GitHub上のリポジトリを<b>自分のパソコンにコピー</b>すること。<br>ダウンロードのようなもの（ただし履歴情報も含む）</td>
</tr>
<tr>
<td><b>💾 コミット<br>(Commit)</b></td>
<td>変更を<b>記録する</b>こと。セーブポイントのようなもの。<br>「何を変更したか」のメッセージも記録する</td>
</tr>
<tr>
<td><b>📤 プッシュ<br>(Push)</b></td>
<td>自分のパソコンの変更を<b>GitHub（サーバー）にアップロード</b>すること</td>
</tr>
<tr>
<td><b>📥 プル<br>(Pull)</b></td>
<td>GitHub（サーバー）の最新の変更を<b>自分のパソコンにダウンロード</b>すること</td>
</tr>
<tr>
<td><b>🏠 master/main<br>ブランチ</b></td>
<td>プロジェクトの<b>本番用</b>のブランチ。完成したコードが入る。<br>ペアプログラミングでは、このブランチで直接作業します</td>
</tr>
</table>

---

### 🚀 完全ステップバイステップガイド

---

## ステップ1️⃣: Gitのインストール

### Windowsの場合

1. **Git for Windows**をダウンロード  
   👉 https://git-scm.com/download/win

2. インストーラーを実行（基本的に全て「Next」で進めてOK）

3. インストール確認  
   ```bash
   # コマンドプロンプトまたはPowerShellを開く
   git --version
   ```
   
   ✅ `git version 2.x.x` のように表示されればOK

---

## ステップ2️⃣: GitHubアカウントの設定

### 初回のみ必要な設定

```bash
# 1. ユーザー名を設定（GitHubのユーザー名を入力）
git config --global user.name "あなたのGitHubユーザー名"

# 2. メールアドレスを設定（GitHubのメールアドレスを入力）
git config --global user.email "your.email@example.com"

# 3. 設定確認
git config --list
```

**例:**
```bash
git config --global user.name "YamaYamamoto"
git config --global user.email "yama@example.com"
```

---

## ステップ3️⃣: リポジトリをクローン（初回のみ）

### 3-1. クローンする場所を決める

```bash
# 例: デスクトップに移動
cd Desktop

# または Documents フォルダに移動
cd Documents

# 現在のフォルダを確認
pwd
```

### 3-2. GitHubからクローン

```bash
# HEW_ECSリポジトリをクローン
git clone https://github.com/aptma-sHEWTeam/HEW_ECS.git

# クローンしたフォルダに移動
cd HEW_ECS
```

<div align="center">

✅ **成功すると**:

```
Cloning into 'HEW_ECS'...
remote: Enumerating objects: 1234, done.
remote: Counting objects: 100% (1234/1234), done.
Unpacking objects: 100% (1234/1234), done.

````````

## 🆘 トラブルシューティング

### ❓ よくあるエラーと解決方法

<details>
<summary><b>エラー: "fatal: not a git repository"</b></summary>

**原因**: Gitリポジトリではないフォルダで実行した

**解決策**:
```bash
# HEW_ECSフォルダに移動しているか確認
cd HEW_ECS

# または、再度クローン
git clone https://github.com/aptma-sHEWTeam/HEW_ECS.git
cd HEW_ECS
```

</details>

<details>
<summary><b>エラー: "error: Your local changes would be overwritten"</b></summary>

**原因**: 変更をコミットせずにプルしようとした

**解決策**:
```bash
# 変更をコミット
git add .
git commit -m "💾 Save work in progress"

# 再度プル
git pull origin master
```

</details>

<details>
<summary><b>エラー: "rejected - non-fast-forward"</b></summary>

**原因**: GitHub上に自分の持っていない変更がある

**解決策**:
```bash
# 1. 最新の変更を取得
git pull origin master

# 2. コンフリクトがあれば解決←わかんなければ私まで

# 3. 再度プッシュ
git push origin master
```

</details>

<details>
<summary><b>問題: "間違えたファイルをコミットしてしまった"</b></summary>

**解決策（プッシュ前）**:
```bash
# 最後のコミットを取り消し（変更は残る）
git reset --soft HEAD~1

# 正しいファイルのみ追加
git add 正しいファイル
git commit -m "✨ 正しいコミット"
```

**解決策（プッシュ済み）**:
```bash
# 新しいコミットで修正
git rm 間違えたファイル
git commit -m "🔥 Remove accidentally committed file"
git push origin master
```

</details>

<details>
<summary><b>問題: "前のバージョンに戻したい※誰かに相談してほしい"</b></summary>

**解決策**:
```bash
# 1. コミット履歴を確認
git log --oneline

# 例:
# def5678 ✨ Add player shoot
# abc1234 ✨ Add player jump
# 9876543 🐛 Fix collision bug

# 2. 戻りたいコミットのハッシュをコピー（例: abc1234）

# 3. そのコミットに戻る（変更は破棄される）
git reset --hard abc1234

# 4. 強制的にプッシュ（⚠️注意: 全員に必ず伝える）
git push origin master --force
```

⚠️ **注意**: `--force`を使うと履歴が書き換わるため、必ず誰かに確認してください。

</details>

---

## 📋 コマンド一覧

<table>
<tr>
<th width="40%">コマンド</th>
<th width="60%">説明</th>
</tr>
<tr>
<td><code>git status</code></td>
<td>現在の状態を確認（変更されたファイルなど）</td>
</tr>
<tr>
<td><code>git add .</code></td>
<td>全ての変更をステージング</td>
</tr>
<tr>
<td><code>git add ファイル名</code></td>
<td>特定のファイルをステージング</td>
</tr>
<tr>
<td><code>git commit -m "メッセージ"</code></td>
<td>変更をコミット</td>
</tr>
<tr>
<td><code>git pull origin master</code></td>
<td>GitHub上の最新を取得</td>
</tr>
<tr>
<td><code>git push origin master</code></td>
<td>GitHubにプッシュ</td>
</tr>
<tr>
<td><code>git log</code></td>
<td>コミット履歴を表示</td>
</tr>
<tr>
<td><code>git log --oneline</code></td>
<td>コミット履歴を1行で表示</td>
</tr>
<tr>
<td><code>git diff</code></td>
<td>変更内容を表示</td>
</tr>
<tr>
<td><code>git reset --soft HEAD~1</code></td>
<td>最後のコミットを取り消し（変更は残す）</td>
</tr>
<tr>
<td><code>git reset --hard HEAD~1</code></td>
<td>最後のコミットを取り消し（変更も破棄）</td>
</tr>
</table>

---

## 🎓 さらに学習するには

### 📚 推奨リソース

| リソース | URL |
|---------|-----|
| **Git公式ドキュメント（日本語）** | https://git-scm.com/book/ja/v2 |
| **GitHub公式ガイド** | https://docs.github.com/ja |
| **サル先生のGit入門** | https://backlog.com/ja/git-tutorial/ |
| **わかばちゃんと学ぶ Git使い方入門←オヌヌメ** | （書籍・Kindle版あり） |

---

## ✅ チェックリスト（作業前）

```
開発開始前に必ず確認:

□ 最新のmasterブランチを取得した（git pull origin master）
□ Visual Studioでビルドが通る
□ ペアの人に作業内容を伝えた
□ コンフリクトがないことを確認した
```

---

## ✅ チェックリスト（作業後）

```
masterにプッシュする前に必ず確認:

□ ビルドエラーがない（Debug/Release両方）
□ コミットメッセージが明確
□ 不要なファイル（.obj, .exe など）をコミットしていない
□ コメントアウトされたコードを削除した
□ デバッグ用のprintfを削除した
□ 他のメンバーの作業に影響しない
□ README更新が必要な場合は更新した
□ ペアの人にレビューしてもらった
□ プッシュ前に再度 git pull した
