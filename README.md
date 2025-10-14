# ?? ECSベースのミニゲーム - シューティングゲーム

## ?? 概要

このプロジェクトは、**Entity Component System (ECS)** を使った学習用のシンプルなシューティングゲームです。

- **可読性最重視** - 初心者でもコードが読める
- **シーンマネージャー搭載** - ゲーム画面の切り替え管理
- **シンプルなゲームロジック** - 理解しやすい実装

---

## ?? ゲーム内容

### ルール
- **プレイヤー（緑のキューブ）** を操作して敵を倒す
- **敵（赤のキューブ）** が上から降ってくる
- **弾（黄色のキューブ）** を発射して敵を撃つ
- 敵を倒すと **スコア +10点**

### 操作方法
| キー | 動作 |
|------|------|
| **A** | 左に移動 |
| **D** | 右に移動 |
| **スペース** | 弾を発射 |
| **ESC** | ゲーム終了 |

---

## ?? 実行方法

### ビルドと実行
1. Visual Studioでプロジェクトを開く
2. `F5`キーでビルド＆実行
3. ゲームを楽しむ！

### 動作環境
- **OS**: Windows 10/11
- **DirectX**: DirectX 11対応GPU
- **開発環境**: Visual Studio 2019以降
- **C++標準**: C++14

---

## ?? ファイル構成

### ?? ゲーム関連
```
MiniGame.h         - シューティングゲームの実装
SceneManager.h     - シーン切り替えシステム
App.h              - アプリケーション本体
```

### ??? ECSシステム
```
World.h            - エンティティとコンポーネントの管理
Entity.h           - エンティティの定義
Component.h        - コンポーネントの基底クラス
```

### ?? グラフィックス
```
GfxDevice.h        - DirectX11デバイス管理
RenderSystem.h     - レンダリングシステム
Camera.h           - カメラ制御
DebugDraw.h        - デバッグ描画
```

### ?? コンポーネント
```
Transform.h        - 位置・回転・スケール
MeshRenderer.h     - メッシュ描画
```

### ?? 入力
```
InputSystem.h      - キーボード・マウス入力
```

---

## ?? コードの読み方

### 1. エントリーポイント
**`main.cpp`** から始まります。
```cpp
int WINAPI WinMain(...) {
    App app;
    app.Init(hInstance);
    app.Run();  // ゲームループ開始
}
```

### 2. アプリケーション本体
**`App.h`** がゲーム全体を管理します。
```cpp
// 初期化
bool Init() {
    // ウィンドウ、DirectX、シーンを初期化
}

// メインループ
void Run() {
    while (ゲーム中) {
        入力処理();
        シーン更新();
        描画();
    }
}
```

### 3. ゲームロジック
**`MiniGame.h`** にゲームのルールがあります。
```cpp
class GameScene : public IScene {
    void OnUpdate() {
        プレイヤー移動();
        弾の発射();
        敵の生成();
        衝突判定();
    }
}
```

### 4. コンポーネント
**`MiniGame.h`** 内で定義されています。
```cpp
// プレイヤーの移動
struct PlayerMovement : Behaviour {
    void OnUpdate(World& w, Entity self, float dt) {
        // 移動処理
    }
};

// 弾の移動
struct BulletMovement : Behaviour {
    void OnUpdate(World& w, Entity self, float dt) {
        // 上に進む
    }
};
```

---

## ??? カスタマイズ方法

### ゲームバランスを変える

#### 敵の出現速度を変える
**`MiniGame.h`** の `UpdateEnemySpawning()` 内:
```cpp
// 1秒ごとに敵を生成 → 0.5秒に変更
if (enemySpawnTimer_ >= 0.5f) {  // 1.0f から変更
```

#### プレイヤーの移動速度を変える
**`MiniGame.h`** の `PlayerMovement`:
```cpp
float speed = 8.0f;  // この値を変える
```

#### 弾の速度を変える
**`MiniGame.h`** の `BulletMovement`:
```cpp
float speed = 15.0f;  // この値を変える
```

#### 弾の連射速度を変える
**`MiniGame.h`** の `UpdateShooting()` 内:
```cpp
shootCooldown_ = 0.2f;  // クールダウン時間（秒）
```

### 新しい機能を追加する

#### 例: 体力システムを追加
```cpp
// 1. コンポーネントを定義
struct Health : IComponent {
    int hp = 3;
};

// 2. プレイヤーに追加
playerEntity_ = world.Create()
    .With<Transform>(...)
    .With<MeshRenderer>(...)
    .With<Player>()
    .With<Health>()  // ← 追加
    .Build();

// 3. 衝突時にダメージ処理
auto* health = world.TryGet<Health>(playerEntity_);
if (health) {
    health->hp -= 1;
    if (health->hp <= 0) {
        // ゲームオーバー処理
    }
}
```

---

## ?? 学習リソース

### ECSについて学ぶ
- **Entity**: ゲームオブジェクトのID
- **Component**: データの塊（位置、見た目など）
- **System**: コンポーネントを処理するロジック

### このプロジェクトのECS
```
Entity (エンティティ)
  └─ Component (コンポーネント)
       ├─ Transform (位置・回転・スケール)
       ├─ MeshRenderer (見た目)
       ├─ Behaviour (動き)
       │   ├─ PlayerMovement
       │   ├─ BulletMovement
       │   └─ EnemyMovement
       └─ Tag (目印)
           ├─ Player
           ├─ Enemy
           └─ Bullet
```

---

## ?? デバッグ方法

### デバッグ描画
`_DEBUG`モードでは、グリッドと座標軸が表示されます。

### ブレークポイント
Visual Studioで以下の場所にブレークポイントを設定すると便利:
- `GameScene::OnUpdate()` - 毎フレームの処理
- `CheckCollisions()` - 衝突判定
- `UpdateShooting()` - 弾の発射

---

## ?? トラブルシューティング

### ビルドエラーが出る
- DirectX SDKがインストールされているか確認
- Windows SDK 10が必要

### ゲームが起動しない
- DirectX 11対応のGPUが必要
- グラフィックスドライバーを最新に更新

### 動作が重い
- `_DEBUG`モードではなく`Release`モードでビルド

---

## ?? ライセンス

このプロジェクトは学習用です。自由に改変・再配布できます。

---

## ?? 作成者

**山内陽**  
バージョン: v5.0 - ミニゲーム実装版（可読性最重視）

---

## ?? 次のステップ

このプロジェクトを理解したら、次に挑戦してみよう：

1. **パワーアップアイテムの追加**
2. **複数の敵タイプ**
3. **タイトル画面とゲームオーバー画面の実装**
4. **ハイスコアの保存**
5. **効果音とBGMの追加**

頑張ってください！ ??
