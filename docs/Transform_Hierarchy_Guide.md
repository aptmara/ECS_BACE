# Transform階層システム ガイド

## 概要

Transform階層システムは、3Dオブジェクト間の親子関係を管理し、階層的な変換を実現するシステムです。親エンティティが移動・回転・スケールすると、子エンティティも自動的に同じ変換を受けます。

## 主要コンポーネント

### 1. TransformHierarchy コンポーネント

エンティティ間の親子関係を管理するコンポーネントです。

#### 主なメソッド

- `SetParent(Entity parentEntity)` - 親エンティティを設定
- `GetParent()` - 親エンティティを取得
- `HasParent()` - 親を持つか確認
- `ClearParent()` - 親子関係を解除
- `AddChild(Entity childEntity)` - 子エンティティを追加
- `RemoveChild(Entity childEntity)` - 子エンティティを削除
- `GetChildren()` - すべての子エンティティを取得
- `HasChildren()` - 子を持つか確認
- `GetChildCount()` - 子の数を取得

### 2. TransformHierarchySystem

Transform階層を毎フレーム更新するBehaviourシステムです。親のワールド変換行列を計算し、子のローカル変換に適用します。

## 基本的な使用方法

### 1. システムの初期化

```cpp
// TransformHierarchySystemをWorldに追加
Entity hierarchySystem = world.Create()
    .With<TransformHierarchySystem>()
    .Build();
```

### 2. 親子関係の作成

```cpp
// 親エンティティを作成
Entity parent = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
    .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
    .With<TransformHierarchy>()
  .Build();

// 子エンティティを作成（親から右に2単位の位置）
Entity child = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{2, 0, 0})
    .With<MeshRenderer>(DirectX::XMFLOAT3{0, 1, 0})
    .With<TransformHierarchy>()
    .Build();

// 親子関係を設定
auto* childHierarchy = world.TryGet<TransformHierarchy>(child);
auto* parentHierarchy = world.TryGet<TransformHierarchy>(parent);

if (childHierarchy && parentHierarchy) {
    childHierarchy->SetParent(parent);
    parentHierarchy->AddChild(child);
}
```

### 3. 複数の子を持つ階層

```cpp
Entity parent = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
    .With<TransformHierarchy>()
    .Build();

// 3つの子を追加
for (int i = 0; i < 3; ++i) {
    Entity child = world.Create()
        .With<Transform>(
         DirectX::XMFLOAT3{static_cast<float>(i) * 2.0f, 0, 0}
        )
        .With<TransformHierarchy>()
        .Build();
    
    auto* childHierarchy = world.TryGet<TransformHierarchy>(child);
    auto* parentHierarchy = world.TryGet<TransformHierarchy>(parent);
    
    if (childHierarchy && parentHierarchy) {
  childHierarchy->SetParent(parent);
        parentHierarchy->AddChild(child);
    }
}
```

## モデル読み込みシステムとの統合

`ModelLoadingSystem` は、複数のメッシュを持つ3Dモデルをロードする際に、自動的にTransform階層を構築します。

```cpp
// Modelコンポーネントを持つエンティティを作成
Entity modelEntity = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
    .With<Model>("assets/models/character.obj")
    .Build();

// ModelLoadingSystemが自動的に以下を実行:
// 1. 最初のメッシュを親エンティティに設定
// 2. 残りのメッシュを子エンティティとして作成
// 3. 親子関係を自動的に設定
```

## 実装例

### 例1: ロボットの腕

```cpp
// 胴体
Entity body = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
.With<MeshRenderer>(DirectX::XMFLOAT3{0.5f, 0.5f, 0.5f})
    .With<TransformHierarchy>()
    .Build();

// 右肩
Entity rightShoulder = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{1, 0, 0})
    .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
    .With<TransformHierarchy>()
    .Build();

// 右腕
Entity rightArm = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{0, -1, 0})
    .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0.5f, 0.5f})
    .With<TransformHierarchy>()
    .Build();

// 階層構造を設定: body -> rightShoulder -> rightArm
world.Get<TransformHierarchy>(rightShoulder).SetParent(body);
world.Get<TransformHierarchy>(body).AddChild(rightShoulder);

world.Get<TransformHierarchy>(rightArm).SetParent(rightShoulder);
world.Get<TransformHierarchy>(rightShoulder).AddChild(rightArm);

// 胴体を回転すると、肩と腕も一緒に回転する
world.Get<Transform>(body).rotation.y = 45.0f;
```

### 例2: 太陽系シミュレーション

```cpp
// 太陽
Entity sun = world.Create()
  .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
    .With<MeshRenderer>(DirectX::XMFLOAT3{1, 1, 0})
    .With<TransformHierarchy>()
    .Build();

// 地球（太陽の周りを公転）
Entity earth = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{10, 0, 0})
    .With<MeshRenderer>(DirectX::XMFLOAT3{0, 0, 1})
    .With<TransformHierarchy>()
    .Build();

// 月（地球の周りを公転）
Entity moon = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{2, 0, 0})
    .With<MeshRenderer>(DirectX::XMFLOAT3{0.7f, 0.7f, 0.7f})
    .With<TransformHierarchy>()
    .Build();

// 階層構造を設定
world.Get<TransformHierarchy>(earth).SetParent(sun);
world.Get<TransformHierarchy>(sun).AddChild(earth);

world.Get<TransformHierarchy>(moon).SetParent(earth);
world.Get<TransformHierarchy>(earth).AddChild(moon);

// 太陽を回転させると、地球と月も一緒に回転する
world.Get<Transform>(sun).rotation.y += 10.0f * deltaTime;
```

## 動作の仕組み

### 更新順序

1. `TransformHierarchySystem` が毎フレーム実行される
2. ルートノード（親を持たないエンティティ）から処理開始
3. 親のローカル変換行列を計算（位置・回転・スケール）
4. 子のローカル変換を親のワールド変換に適用
5. 子の子（孫）に対して再帰的に処理

### 座標系

- **ローカル座標**: 親エンティティを基準とした相対座標
- **ワールド座標**: 絶対座標系での位置

子のTransformは常にワールド座標で保存されますが、親が変換されると自動的に再計算されます。

## 注意事項

### 1. 循環参照の防止

親子関係を設定する際、循環参照（A→B→A）を作成しないように注意してください。システムは循環参照を検出して無視しますが、意図しない動作を引き起こす可能性があります。

```cpp
// ? 悪い例: 循環参照
world.Get<TransformHierarchy>(child).SetParent(parent);
world.Get<TransformHierarchy>(parent).SetParent(child);  // 循環参照!
```

### 2. 親子関係の同期

親子関係を設定する際は、必ず両方向で設定してください：

```cpp
// ? 良い例
childHierarchy->SetParent(parent);      // 子 → 親
parentHierarchy->AddChild(child);  // 親 → 子
```

### 3. エンティティの削除

親エンティティを削除する場合、子エンティティは自動的には削除されません。必要に応じて手動で削除してください：

```cpp
// 親と子をすべて削除
auto* hierarchy = world.TryGet<TransformHierarchy>(parent);
if (hierarchy) {
 for (const Entity& child : hierarchy->GetChildren()) {
   world.DestroyEntity(child);
    }
}
world.DestroyEntity(parent);
```

## パフォーマンス最適化

### 1. 深い階層を避ける

階層が深すぎると、再帰的な処理によりパフォーマンスが低下します。可能な限りフラットな階層構造を維持してください。

### 2. 静的オブジェクトの最適化

変換が変わらないオブジェクトの場合、TransformHierarchySystemを無効にすることで最適化できます。

## デバッグ方法

階層構造をデバッグする際は、以下のヘルパー関数を使用できます：

```cpp
void PrintHierarchy(World& world, Entity entity, int depth = 0) {
    auto* hierarchy = world.TryGet<TransformHierarchy>(entity);
    auto* transform = world.TryGet<Transform>(entity);
    
    if (!hierarchy || !transform) return;
    
    std::string indent(depth * 2, ' ');
  printf("%sEntity %u: pos=(%.1f, %.1f, %.1f)\n",
        indent.c_str(), entity.id,
     transform->position.x,
        transform->position.y,
        transform->position.z);
    
    for (const Entity& child : hierarchy->GetChildren()) {
        PrintHierarchy(world, child, depth + 1);
    }
}
```

## まとめ

Transform階層システムを使用することで、複雑な3Dシーンの管理が簡単になります。親子関係を正しく設定し、システムが自動的にワールド座標を更新することで、直感的なオブジェクト配置が可能になります。

---

**作成者**: 山内陽  
**バージョン**: 6.0  
**最終更新**: 2025
