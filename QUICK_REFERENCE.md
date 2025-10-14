# ?? ECS + DirectX11 プロジェクト - クイックリファレンス

**初学者向けコンポーネント指向プログラミング教材 v4.0**

---

## ?? 5分で始める

### 1. ビルド＆実行
```
Visual Studio 2022で開く → F5キー → 実行！
```

### 2. 最初に読むファイル
1. **README.md** - プロジェクト全体の説明
2. **LEARNING_GUIDE.md** - ステップバイステップ学習
3. **App.h** - メインコード

---

## ?? ファイル一覧（重要度順）

### ??? 必読ファイル
| ファイル | 説明 | 行数 |
|---------|------|------|
| `README.md` | プロジェクト全体ガイド | 500行 |
| `LEARNING_GUIDE.md` | 段階的学習ガイド | 800行 |
| `App.h` | メインアプリケーション | 450行 |

### ?? 重要ファイル
| ファイル | 説明 | 行数 |
|---------|------|------|
| `Entity.h` | エンティティの定義 | 50行 |
| `Component.h` | コンポーネント基底 | 100行 |
| `World.h` | ECSワールド管理 | 200行 |
| `Transform.h` | 位置・回転・スケール | 30行 |
| `MeshRenderer.h` | 描画設定 | 40行 |
| `Rotator.h` | 自動回転 | 50行 |

### ? 学習用サンプル
| ファイル | 説明 | 行数 |
|---------|------|------|
| `ComponentSamples.h` | コンポーネント集 | 500行 |
| `SampleScenes.h` | シーン作成例 | 400行 |

### システムファイル（読まなくてOK）
- `GfxDevice.h` - DirectX11管理
- `RenderSystem.h` - 描画システム
- `InputSystem.h` - 入力システム
- `TextureManager.h` - テクスチャ管理
- `Camera.h` - カメラ
- `DebugDraw.h` - デバッグ描画
- `Animation.h` - アニメーション
- `VideoPlayer.h` - 動画再生

---

## ?? 重要な概念（3つだけ覚える）

### 1. Entity（エンティティ）
```cpp
Entity player = world.CreateEntity();
```
- ゲーム世界の「物」を表すID番号
- それ自体には機能がない

### 2. Component（コンポーネント）
```cpp
struct Transform : IComponent {
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 rotation;
    DirectX::XMFLOAT3 scale;
};
```
- エンティティに付ける「部品」
- 2種類: データコンポーネント / Behaviourコンポーネント

### 3. World（ワールド）
```cpp
World world;
world.Add<Transform>(entity, Transform{...});
auto* t = world.TryGet<Transform>(entity);
```
- 全エンティティとコンポーネントを管理
- 追加・削除・取得を行う

---

## ?? よくあるコードパターン

### パターン1: エンティティ作成
```cpp
// ビルダーパターン（推奨）
Entity cube = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
    .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
    .With<Rotator>(45.0f)
    .Build();
```

### パターン2: コンポーネント取得＆変更
```cpp
auto* transform = world.TryGet<Transform>(entity);
if (transform) {
    transform->position.y += 1.0f;
}
```

### パターン3: 全エンティティに処理
```cpp
world.ForEach<Transform>([](Entity e, Transform& t) {
    t.position.y += 0.01f;
});
```

### パターン4: カスタムBehaviour作成
```cpp
struct MyBehaviour : Behaviour {
    void OnUpdate(World& w, Entity self, float dt) override {
        auto* t = w.TryGet<Transform>(self);
        if (t) {
            t->rotation.y += 90.0f * dt;
        }
    }
};
```

---

## ?? よくある操作

### キューブの色を変える
```cpp
// App.h の CreateSimpleRotatingCube() で
.With<MeshRenderer>(DirectX::XMFLOAT3{1.0f, 0.0f, 0.0f}) // 赤
//                                      ↑    ↑    ↑
//                                      R    G    B (0.0 ～ 1.0)
```

### キューブの位置を変える
```cpp
.With<Transform>(
    DirectX::XMFLOAT3{-2.0f, 0.0f, 0.0f},  // X, Y, Z
    //                 ↑     ↑     ↑
    //                左右   上下  前後
```

### 回転速度を変える
```cpp
.With<Rotator>(45.0f)  // 毎秒45度
//             ↑
//          この数値を変える
```

---

## ?? 学習ロードマップ

### レベル1: 基本理解（1時間）
- [ ] README.md を読む
- [ ] LEARNING_GUIDE.md のレッスン1-3
- [ ] App.h のコードを眺める

### レベル2: 手を動かす（2時間）
- [ ] 色を変えてみる
- [ ] 位置を変えてみる
- [ ] 回転速度を変えてみる
- [ ] 新しいキューブを追加

### レベル3: サンプル活用（3時間）
- [ ] SampleScenes.h を使う
- [ ] ComponentSamples.h のコンポーネントを試す
- [ ] サンプルを改造する

### レベル4: 自作開始（5時間）
- [ ] 自分でBehaviourを作る
- [ ] 複数のコンポーネントを組み合わせる
- [ ] オリジナルのゲーム要素を作る

### レベル5: 応用（10時間以上）
- [ ] 衝突判定システム
- [ ] パーティクルシステム
- [ ] UIシステム
- [ ] ミニゲームを完成させる

---

## ?? トラブルシューティング

### Q: ビルドエラーが出る
**A:** DirectX11 SDKがインストールされているか確認

### Q: 何も表示されない
**A:** MeshRendererコンポーネントが付いているか確認

### Q: TryGetがnullptrを返す
**A:** そのエンティティにそのコンポーネントが付いていない

### Q: コンポーネントが動かない
**A:** Behaviourの場合、world.Tick(dt)が呼ばれているか確認

---

## ?? プロジェクト統計

| 項目 | 数値 |
|------|------|
| 総コード行数 | 約3000行 |
| ヘッダーファイル数 | 20個 |
| サンプルコンポーネント数 | 15個 |
| サンプルシーン数 | 9個 |
| レッスン数 | 9個 |
| ドキュメント行数 | 約2000行 |

---

## ?? カラーコード参考

```cpp
// 基本色
DirectX::XMFLOAT3{1, 0, 0}  // 赤
DirectX::XMFLOAT3{0, 1, 0}  // 緑
DirectX::XMFLOAT3{0, 0, 1}  // 青
DirectX::XMFLOAT3{1, 1, 0}  // 黄
DirectX::XMFLOAT3{1, 0, 1}  // マゼンタ
DirectX::XMFLOAT3{0, 1, 1}  // シアン
DirectX::XMFLOAT3{1, 1, 1}  // 白
DirectX::XMFLOAT3{0, 0, 0}  // 黒

// パステルカラー
DirectX::XMFLOAT3{1.0f, 0.7f, 0.7f}  // ピンク
DirectX::XMFLOAT3{0.7f, 1.0f, 0.7f}  // ライトグリーン
DirectX::XMFLOAT3{0.7f, 0.7f, 1.0f}  // ライトブルー
```

---

## ?? ドキュメント一覧

| ファイル | 内容 |
|---------|------|
| `README.md` | プロジェクト全体ガイド |
| `LEARNING_GUIDE.md` | ステップバイステップ学習 |
| `CHANGELOG_v4.md` | v4.0の変更点 |
| `CHANGELOG_v3.md` | v3.0の変更点（旧版） |
| `QUICK_REFERENCE.md` | このファイル |

---

## ?? 次にやること（優先度順）

### 今すぐできる
1. F5キーでビルド＆実行
2. App.h でキューブの色を変える
3. 新しいキューブを1つ追加

### 今日中にやる
1. LEARNING_GUIDE.md のレッスン1-3
2. SampleScenes.h を試す
3. 自分でBehaviourを1つ作る

### 今週中にやる
1. ComponentSamples.h の全サンプルを試す
2. オリジナルのコンポーネントを5つ作る
3. ミニゲームのプロトタイプを作る

---

## ?? キーボードショートカット

| キー | 動作 |
|------|------|
| **F5** | ビルド＆実行 |
| **Ctrl + F5** | デバッグなしで実行 |
| **F9** | ブレークポイント設定 |
| **F10** | ステップオーバー |
| **F11** | ステップイン |
| **ESC** | アプリ終了（実行中） |
| **WASD** | カメラ回転（実行中） |
| **Space** | 動画切替（実行中） |

---

## ?? 最後に

このプロジェクトは、**初学者が挫折せずにコンポーネント指向を学べる**ことを最優先に作られています。

### 学習のコツ
- ? 小さく始める（1つのキューブから）
- ? すぐに試す（コードを書いたらF5）
- ? 失敗を恐れない（壊れても大丈夫）
- ? サンプルをコピペして改造
- ? わからなければコメントを読む

### サポート
- コード内のコメントを読む
- README.md のFAQを見る
- LEARNING_GUIDE.md で復習

---

**楽しんで学んでください！ ??**

---

**作成者: 山内陽**  
**バージョン: v4.0 - 可読性特化版**  
**最終更新: 2024年**
