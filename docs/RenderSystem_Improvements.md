# RenderSystem.h 改善ドキュメント

## バージョン: 7.0
**作成日**: 2025  
**作成者**: 山内陽

---

## 課題分析

### 現在の実装の問題点

#### 1. **MeshRendererの未実装** ??
- `RenderMeshRenderers()`関数が空実装
- デバッグメッセージを出力するだけで実際の描画をしていない
- これが**プレイヤーやエンティティが表示されない主な原因**

#### 2. **エラーハンドリング不足**
```cpp
// 現在の問題
gfx.Dev()->CreateInputLayout(...); // 戻り値チェックなし
gfx.Dev()->CreateBuffer(...);   // 戻り値チェックなし
```

#### 3. **初期化状態の管理不足**
- `initialized_`フラグがない
- 複数回`Init()`を呼べてしまう
- 未初期化状態で`Render()`を呼べる

#### 4. **コードの重複**
- `ModelComponent`と`MeshRenderer`で同じ行列変換コード
- テクスチャ設定コードが重複

#### 5. **統計情報の欠如**
- 描画オブジェクト数の追跡なし
- パフォーマンスメトリクスなし

#### 6. **ドキュメント不足**
- Doxygenコメントがない
- 関数の目的が不明確

---

## 改善提案

### 最優先: MeshRendererの実装

```cpp
void RenderMeshRenderers(World& w, GfxDevice& gfx, const Camera& cam, TextureManager& texMgr) {
    // 基本形状メッシュのキャッシュを使用
    w.ForEach<Transform, MeshRenderer>([&](Entity e, Transform& t, MeshRenderer& mr) {
  // 1. メッシュデータの取得
        auto* meshData = GetMeshData(mr.meshType);
        if (!meshData) return;
  
        // 2. ワールド行列の計算
        DirectX::XMMATRIX worldMatrix = CalculateWorldMatrix(t);
        
        // 3. 定数バッファの更新
        UpdateVSConstants(gfx, worldMatrix, cam, mr.uvOffset, mr.uvScale);
        UpdatePSConstants(gfx, mr.color, mr.texture, 32.0f);
        
     // 4. テクスチャの設定
        SetTextures(gfx, texMgr, mr.texture);
        
        // 5. 描画
        DrawMesh(gfx, meshData);
        
        stats_.meshesRendered++;
    });
}
```

### エラーハンドリング強化

```cpp
bool CreateInputLayout(GfxDevice& gfx) {
    D3D11_INPUT_ELEMENT_DESC il[] = { /* ... */ };
    
    HRESULT hr = gfx.Dev()->CreateInputLayout(il, 5, vsBlob_->GetBufferPointer(), 
             vsBlob_->GetBufferSize(), layout_.GetAddressOf());
    if (FAILED(hr)) {
  DEBUGLOG_ERROR("[RenderSystem] 入力レイアウトの作成失敗 (HRESULT: 0x" + 
           std::to_string(hr) + ")");
        return false;
    }
    
    return true;
}
```

### 初期化状態の管理

```cpp
struct RenderSystem {
    bool Init() {
        if (initialized_) {
 DEBUGLOG_WARNING("[RenderSystem] 既に初期化されています");
  return true;
  }
   
        // 初期化処理...
        
        initialized_ = true;
        return true;
    }
    
    void Render(World& w, const Camera& cam) {
    if (!initialized_) {
            DEBUGLOG_WARNING("[RenderSystem] 初期化されていません");
            return;
        }
        
   // 描画処理...
    }
    
private:
    bool initialized_ = false;
};
```

### 統計情報の追加

```cpp
struct Statistics {
    size_t modelsRendered = 0;
    size_t meshesRendered = 0;
    size_t totalDrawCalls = 0;
    
    void Reset() {
        modelsRendered = 0;
        meshesRendered = 0;
        totalDrawCalls = 0;
    }
};

// 使用例
const auto& stats = renderSystem.GetStatistics();
printf("描画: Models=%zu, Meshes=%zu, Calls=%zu\n", 
  stats.modelsRendered, stats.meshesRendered, stats.totalDrawCalls);
```

---

## 基本形状メッシュの実装

### メッシュキャッシュ構造

```cpp
struct MeshData {
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
  UINT indexCount = 0;
};

std::unordered_map<int, std::unique_ptr<MeshData>> meshCache_;
```

### Cube メッシュ

```cpp
bool CreateCubeMesh(GfxDevice& gfx) {
    const float size = 0.5f;
    
    Vertex vertices[] = {
  // Front face
        {{-size, -size, size}, {0, 1}, {0, 0, 1}, {1, 0, 0}, {0, 1, 0}},
        {{size, -size, size}, {1, 1}, {0, 0, 1}, {1, 0, 0}, {0, 1, 0}},
    {{size, size, size}, {1, 0}, {0, 0, 1}, {1, 0, 0}, {0, 1, 0}},
        {{-size, size, size}, {0, 0}, {0, 0, 1}, {1, 0, 0}, {0, 1, 0}},
        // ... 他の面
    };
    
    uint16_t indices[] = {0, 1, 2, 0, 2, 3, /* ... */};
    
    return CreateMeshBuffers(gfx, vertices, indices, MeshType::Cube);
}
```

### Sphere メッシュ (パラメトリック生成)

```cpp
bool CreateSphereMesh(GfxDevice& gfx) {
    const int segments = 32;
    const int rings = 16;
    const float radius = 0.5f;
    
    std::vector<Vertex> vertices;
 std::vector<uint16_t> indices;
    
    // 球面座標系で頂点を生成
    for (int ring = 0; ring <= rings; ++ring) {
        float phi = DirectX::XM_PI * ring / rings;
        for (int seg = 0; seg <= segments; ++seg) {
    float theta = 2.0f * DirectX::XM_PI * seg / segments;
        
            Vertex v;
    v.pos.x = radius * sinf(phi) * cosf(theta);
      v.pos.y = radius * cosf(phi);
      v.pos.z = radius * sinf(phi) * sinf(theta);
            // ...
          vertices.push_back(v);
        }
    }
    
    // インデックス生成...
    
    return CreateMeshBuffers(gfx, vertices, indices, MeshType::Sphere);
}
```

---

## パフォーマンス最適化

### 1. ステートの最適化

```cpp
void SetupPipeline(GfxDevice& gfx) {
    // 一度だけ設定（フレーム毎ではなく）
    static bool initialized = false;
    if (!initialized) {
        gfx.Ctx()->IASetInputLayout(layout_.Get());
gfx.Ctx()->VSSetShader(vs_.Get(), nullptr, 0);
  gfx.Ctx()->PSSetShader(ps_.Get(), nullptr, 0);
   // ...
        initialized = true;
    }
    
    // フレーム毎に設定が必要なもの
    gfx.Ctx()->VSSetConstantBuffers(0, 1, vsCb_.GetAddressOf());
    // ...
}
```

### 2. 定数バッファの更新最適化

```cpp
// 変更されていない場合はスキップ
if (lastLightUpdate_ != currentFrame_) {
    UpdateLightConstants(w, cam, gfx);
    lastLightUpdate_ = currentFrame_;
}
```

---

## 使用例

### 基本的な使い方

```cpp
// 初期化
RenderSystem renderer;
if (!renderer.Init()) {
    return false;
}

// メインループ
while (running) {
    // Worldに描画可能なエンティティを追加
    Entity cube = world.Create()
        .With<Transform>(DirectX::XMFLOAT3{0, 0, 5})
        .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})  // 赤いキューブ
        .Build();
    
    // 描画
    renderer.Render(world, camera);
    
    // 統計情報の表示（オプション）
    const auto& stats = renderer.GetStatistics();
    if (stats.totalDrawCalls > 1000) {
        DEBUGLOG_WARNING("描画コールが多すぎます: " + std::to_string(stats.totalDrawCalls));
    }
}

// シャットダウン
renderer.Shutdown();
```

### 各種形状の描画

```cpp
// キューブ
Entity cube = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{0, 1, 5})
    .With<MeshRenderer>()  // デフォルトはCube
    .Build();

// 球体
MeshRenderer sphereRenderer;
sphereRenderer.meshType = MeshType::Sphere;
sphereRenderer.color = DirectX::XMFLOAT3{0, 1, 0};  // 緑

Entity sphere = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{2, 1, 5})
    .With<MeshRenderer>(sphereRenderer)
.Build();

// 円柱
MeshRenderer cylinderRenderer;
cylinderRenderer.meshType = MeshType::Cylinder;
cylinderRenderer.color = DirectX::XMFLOAT3{0, 0, 1};  // 青

Entity cylinder = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{-2, 1, 5})
    .With<MeshRenderer>(cylinderRenderer)
    .Build();
```

---

## 実装上の注意点

### C++14準拠

プロジェクトはC++14を使用しているため、以下の機能は使用できません：

- ? `std::optional` (C++17)
- ? `if constexpr` (C++17)
- ? 構造化束縛 (C++17)
- ? `auto` 型推論
- ? ラムダ式
- ? `std::unique_ptr`, `std::shared_ptr`

### マルチスレッド

現在の実装はシングルスレッド専用です。マルチスレッド環境で使用する場合は、以下の対策が必要：

1. `meshCache_`へのアクセスを`std::mutex`で保護
2. DirectX11コンテキストの同期
3. 統計情報の更新をアトミックに

---

## まとめ

### 実装すべき優先順位

1. ? **最優先**: `RenderMeshRenderers()`の実装
2. ? エラーハンドリングの強化
3. ? 初期化状態の管理
4. ? 統計情報の追加
5. ? ドキュメントの充実

### 期待される効果

- ?? **MeshRenderer**が正しく描画される
- ??? **エラー**が適切にログ出力される
- ?? **パフォーマンス**が可視化される
- ?? **保守性**が向上する
- ?? **デバッグ**が容易になる

---

**作成者**: 山内陽  
**バージョン**: 7.0  
**日付**: 2025
