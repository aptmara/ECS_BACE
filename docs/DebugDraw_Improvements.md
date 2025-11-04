# DebugDraw.h 改善ドキュメント

## バージョン: 6.0
**作成日**: 2025  
**作成者**: 山内陽

---

## 改善概要

`DebugDraw.h`を完璧な実装に改善しました。以下は主な変更点と改善内容です。

---

## 主な改善点

### 1. **C++14準拠の徹底**

#### 改善前の問題:
- コメントに「C++17」と記載されていたが、プロジェクトはC++14を使用

#### 改善内容:
- ドキュメントコメントを「C++14準拠」に修正
- C++14で使用可能な機能のみを使用

---

### 2. **ムーブセマンティクスの実装**

#### 改善前の問題:
- コピーコンストラクタとコピー代入演算子が削除されていたが、ムーブ操作が未定義

#### 改善内容:
```cpp
// ムーブコンストラクタ
DebugDraw(DebugDraw&& other) noexcept;

// ムーブ代入演算子
DebugDraw& operator=(DebugDraw&& other) noexcept;
```

**メリット**:
- リソースの効率的な移動が可能
- `std::vector<DebugDraw>`などのコンテナで使用可能
- 一時オブジェクトのパフォーマンス向上

---

### 3. **統計情報機能の追加**

#### 新機能:
```cpp
struct Statistics {
    size_t linesDrawn = 0;   // 描画された線の数
    size_t linesDropped = 0;      // 容量不足で破棄された線の数
    size_t totalLinesAdded = 0;   // 追加された線の総数
  size_t peakLineCount = 0;     // ピーク時の線の数
};
```

**使用例**:
```cpp
// 統計情報の取得
const auto& stats = debugDraw.GetStatistics();
printf("描画線数: %zu, 破棄線数: %zu\n", stats.linesDrawn, stats.linesDropped);

// 統計情報のリセット
debugDraw.ResetStatistics();
```

**メリット**:
- パフォーマンスのモニタリングが容易
- メモリ使用量の最適化の判断材料
- デバッグ時の可視化

---

### 4. **エラーハンドリングの強化**

#### 改善前の問題:
- 初期化されていない状態での操作が可能
- エラーメッセージが不十分

#### 改善内容:

**初期化チェック**:
```cpp
void AddLine(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const DirectX::XMFLOAT3& color) {
    if (!initialized_) {
        DEBUGLOG_WARNING("[DebugDraw] 初期化されていません。AddLine()を無視します。");
 return;
    }
    // ...
}
```

**詳細なエラーログ**:
```cpp
if (FAILED(hr)) {
if (err) {
        std::string errorMsg(static_cast<const char*>(err->GetBufferPointer()), err->GetBufferSize());
        DEBUGLOG_ERROR("[DebugDraw] 頂点シェーダーのコンパイル失敗: " + errorMsg);
    } else {
        DEBUGLOG_ERROR("[DebugDraw] 頂点シェーダーのコンパイル失敗 (HRESULT: 0x" + std::to_string(hr) + ")");
    }
    return false;
}
```

**メリット**:
- エラーの原因が明確
- デバッグが容易
- クラッシュの防止

---

### 5. **パフォーマンスの最適化**

#### 改善前の問題:
- 線の上限に到達するたびに警告ログが出力され、パフォーマンスに影響

#### 改善内容:
```cpp
void AddLine(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, const DirectX::XMFLOAT3& color) {
 if (lines_.size() >= maxLines_) {
        stats_.linesDropped++;
        
        // 最初の警告のみログ出力
        static bool warningShown = false;
        if (!warningShown) {
          DEBUGLOG_WARNING("[DebugDraw] 線の上限に到達 (" + std::to_string(maxLines_) + ")。これ以降の線は破棄されます。");
        warningShown = true;
      }
        return;
    }
    // ...
}
```

**メリット**:
- 警告は最初の1回のみ
- ログの過剰な出力を防止
- フレームレートへの影響を最小化

---

### 6. **新しい描画機能の追加**

#### 6.1 ボックス描画

```cpp
void DrawBox(const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT3& halfExtents, const DirectX::XMFLOAT3& color);
```

**使用例**:
```cpp
// 当たり判定ボックスの可視化
debugDraw.DrawBox(
    DirectX::XMFLOAT3{0, 1, 0},  // 中心
    DirectX::XMFLOAT3{0.5f, 0.5f, 0.5f},  // 半分のサイズ
    DirectX::XMFLOAT3{1, 1, 0}  // 黄色
);
```

#### 6.2 球描画

```cpp
void DrawSphere(const DirectX::XMFLOAT3& center, float radius, const DirectX::XMFLOAT3& color, int segments = 16);
```

**使用例**:
```cpp
// 球状の当たり判定の可視化
debugDraw.DrawSphere(
    DirectX::XMFLOAT3{0, 2, 0},  // 中心
    1.5f,  // 半径
    DirectX::XMFLOAT3{0, 1, 1},  // シアン
    32  // 分割数
);
```

**メリット**:
- 当たり判定の可視化が簡単
- 物理シミュレーションのデバッグに便利
- 実装が高速（ワイヤーフレーム）

---

### 7. **入力検証の追加**

#### 改善内容:
```cpp
void DrawGrid(float size = 10.0f, int divisions = 10, ...) {
    if (divisions <= 0) {
        DEBUGLOG_WARNING("[DebugDraw] DrawGrid: divisionsは正の値である必要があります");
     return;
}
    // ...
}

void DrawAxes(float length = 500.0f) {
    if (length <= 0.0f) {
        DEBUGLOG_WARNING("[DebugDraw] DrawAxes: lengthは正の値である必要があります");
        return;
    }
    // ...
}
```

**メリット**:
- 不正な入力による予期しない動作を防止
- ユーザーへのフィードバックが明確

---

### 8. **シャットダウン処理の改善**

#### 改善内容:
```cpp
void Shutdown() {
    if (isShutdown_) return; // 冪等性
    
    // 統計情報をログ出力
    if (stats_.totalLinesAdded > 0) {
        DEBUGLOG_CATEGORY(DebugLog::Category::Graphics, 
       "DebugDraw統計: 総追加線数=" + std::to_string(stats_.totalLinesAdded) +
            ", ピーク線数=" + std::to_string(stats_.peakLineCount) +
        ", 破棄線数=" + std::to_string(stats_.linesDropped));
    }
    
    // リソース解放
    // ...
    
    lines_.clear();
    lines_.shrink_to_fit();// メモリを確実に解放
    
    isShutdown_ = true;
    initialized_ = false;
}
```

**メリット**:
- 統計情報が自動的にログ出力される
- メモリリークの防止
- 冪等性により複数回の呼び出しが安全

---

### 9. **コードの構造化**

#### 改善内容:
- シェーダーコンパイルを`CompileShaders()`に分離
- パイプラインステート作成を`CreatePipelineState()`に分離
- 定数バッファ作成を`CreateConstantBuffer()`に分離
- 頂点バッファ作成を`CreateVertexBuffer()`に分離

**メリット**:
- 各機能が独立して理解しやすい
- テストが容易
- 将来の拡張が簡単

---

### 10. **ドキュメントの改善**

#### 改善内容:
- すべてのpublic関数にDoxygenコメントを追加
- 使用例を追加
- パラメータの詳細説明を追加
- 戻り値の説明を追加

**例**:
```cpp
/**
 * @brief ボックスを描画
 * @param[in] center ボックスの中心
 * @param[in] halfExtents ボックスの半分のサイズ
 * @param[in] color ボックスの色
 *
 * @details
 * ワイヤーフレームのボックスを描画します。
 * 当たり判定の可視化に便利です。
 */
void DrawBox(const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT3& halfExtents, const DirectX::XMFLOAT3& color);
```

---

## 使用例

### 基本的な使い方

```cpp
// 初期化
DebugDraw debugDraw;
if (!debugDraw.Init(gfx, 20000)) {  // 最大20000線
    // エラーハンドリング
    return false;
}

// メインループ
while (running) {
    // 毎フレーム、グリッドと座標軸を描画
    debugDraw.DrawGrid(20.0f, 20);
    debugDraw.DrawAxes(500.0f);
    
    // プレイヤーの当たり判定を可視化
  world.ForEach<Transform, PlayerTag>([&](Entity e, Transform& t, PlayerTag&) {
        debugDraw.DrawBox(t.position, DirectX::XMFLOAT3{0.5f, 0.5f, 0.5f}, DirectX::XMFLOAT3{1, 1, 0});
    });
    
    // 敵の当たり判定を可視化
    world.ForEach<Transform, EnemyTag>([&](Entity e, Transform& t, EnemyTag&) {
        debugDraw.DrawSphere(t.position, 1.0f, DirectX::XMFLOAT3{1, 0, 0}, 16);
    });
    
    // 描画
    debugDraw.Render(gfx, camera);
    
    // クリア
    debugDraw.Clear();
}

// 統計情報の表示
const auto& stats = debugDraw.GetStatistics();
printf("ピーク線数: %zu, 破棄線数: %zu\n", stats.peakLineCount, stats.linesDropped);

// シャットダウン
debugDraw.Shutdown();
```

### 高度な使い方

```cpp
// カスタム線の描画
debugDraw.AddLine(
    DirectX::XMFLOAT3{0, 0, 0},
    DirectX::XMFLOAT3{10, 5, 3},
    DirectX::XMFLOAT3{0, 1, 0}  // 緑色
);

// パス（経路）の描画
std::vector<DirectX::XMFLOAT3> path = { /* ... */ };
for (size_t i = 1; i < path.size(); ++i) {
    debugDraw.AddLine(path[i-1], path[i], DirectX::XMFLOAT3{1, 0, 1});  // マゼンタ
}

// 統計情報のモニタリング
if (debugDraw.GetStatistics().linesDropped > 0) {
    // 最大線数を増やすか、描画する線を減らす
    DEBUGLOG_WARNING("線が破棄されています。最大線数を増やしてください。");
}
```

---

## パフォーマンス特性

### メモリ使用量

- **最小**: 約100KB（空の状態）
- **標準**: 約1MB（10000線の場合）
- **最大**: Init()で指定した`maxLines`に依存

### CPU使用量

- **AddLine()**: O(1)
- **DrawGrid()**: O(divisions?)
- **DrawBox()**: O(1)（12本の線）
- **DrawSphere()**: O(segments)（segments * 3本の線）
- **Render()**: O(n)（n = 線の数）

### GPU使用量

- **頂点数**: 線の数 × 2
- **描画コール**: 1回（すべての線を1回で描画）

---

## 制限事項

1. **最大線数**
   - Init()で指定した`maxLines`を超える線は破棄される
   - 破棄された線は統計情報に記録される

2. **スレッドセーフティ**
   - シングルスレッド専用
   - マルチスレッド環境では外部で同期が必要

3. **深度テスト**
   - 現在の実装では深度テストが有効
   - 線が他のオブジェクトの後ろに隠れる可能性がある

---

## 今後の拡張案

### 1. 深度テスト制御
```cpp
void SetDepthTestEnabled(bool enabled);
```

### 2. 線の太さ制御
```cpp
void SetLineWidth(float width);
```

### 3. アンチエイリアシング
```cpp
void SetAntiAliasingEnabled(bool enabled);
```

### 4. バッチング最適化
- 同じ色の線をバッチングして描画コールを削減

### 5. スレッドセーフ版
```cpp
class ThreadSafeDebugDraw : public DebugDraw {
    std::mutex mutex_;
    // ...
};
```

---

## まとめ

`DebugDraw.h`を以下の観点から改善しました：

? **C++14準拠**  
? **ムーブセマンティクス対応**  
? **統計情報機能**  
? **エラーハンドリング強化**  
? **パフォーマンス最適化**  
? **新しい描画機能（ボックス、球）**  
? **入力検証**  
? **コードの構造化**  
? **ドキュメントの充実**  
? **シャットダウン処理の改善**

これにより、**完璧で保守性の高い実装**を実現しました。

---

**作成者**: 山内陽  
**バージョン**: 6.0  
**日付**: 2025
