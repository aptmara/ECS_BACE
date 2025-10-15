# ヘッダーインクルード簡略化ガイド

## 方法1: PCH（プリコンパイル済みヘッダー）【推奨】

### 利点
- ? **ビルド高速化**: 一度コンパイルしたヘッダーを再利用
- ? **コードが簡潔**: 1行で全部インクルード
- ? **プロジェクト全体で統一**: すべてのファイルで同じ環境

### 設定方法
`docs/PCH_SETUP.md` を参照

### 使い方
```cpp
#include "pch.h"

// これだけで以下がすべて使える:
// - Entity, World
// - Transform, MeshRenderer, Rotator
// - InputSystem, SceneManager
// - ComponentSamples
// - Animation
```

## 方法2: AllComponents.h【PCH代替】

### 利点
- ? **設定不要**: すぐ使える
- ? **ファイル単位で選択可**: 必要なファイルだけで使える

### 使い方
```cpp
#include "AllComponents.h"

// PCHと同じように使える
Entity e = world.Create()
    .With<Transform>()
    .With<MeshRenderer>()
    .Build();
```

## 使い分け

| 状況 | おすすめ |
|------|----------|
| Visual Studio でプロジェクト開発 | **PCH** |
| 単一ファイルだけで試したい | **AllComponents.h** |
| 他のコンパイラを使う | **AllComponents.h** |
| ビルド速度を最優先 | **PCH** |

## 従来の方法（個別インクルード）

もちろん、個別にインクルードすることもできます：

```cpp
#include "components/Transform.h"
#include "components/MeshRenderer.h"
#include "ecs/World.h"
// 必要なものだけ
```

### いつ使う？
- 依存関係を明確にしたい場合
- ヘッダーファイル（.h）内で最小限のインクルード
- ライブラリとして公開する場合

## 新しいコンポーネント追加時

### PCH使用時
1. コンポーネント定義（例: `MyComponent.h`）
2. `pch.h` に追加（オプション、よく使う場合のみ）
3. `#include "pch.h"` で使える

### AllComponents.h使用時
1. コンポーネント定義（例: `MyComponent.h`）
2. `AllComponents.h` に追加
3. `#include "AllComponents.h"` で使える

### 個別インクルード時
1. コンポーネント定義（例: `MyComponent.h`）
2. 使いたいファイルで `#include "MyComponent.h"`

## サンプル比較

### Before（煩雑）
```cpp
#include "scenes/SceneManager.h"
#include "components/Transform.h"
#include "components/MeshRenderer.h"
#include "components/Rotator.h"
#include "components/Component.h"
#include "ecs/World.h"
#include "ecs/Entity.h"
#include <cstdlib>
#include <ctime>
#include <vector>

struct MyComponent : IComponent {
    // ...
};
```

### After（簡潔）
```cpp
#include "pch.h"  // または AllComponents.h

struct MyComponent : IComponent {
    // ...
};
```

## パフォーマンス比較

| 方法 | 初回ビルド | 2回目以降 | コード量 |
|------|-----------|----------|---------|
| 個別インクルード | 遅い | 遅い | 多い |
| AllComponents.h | 遅い | 遅い | 少ない |
| **PCH** | **やや遅い** | **速い** | **最少** |

## トラブルシューティング

### Q: PCH設定後、ビルドエラーが出る
A: すべての.cppファイルの先頭に `#include "pch.h"` を追加してください

### Q: 特定のファイルだけPCHを使いたくない
A: そのファイルのプロパティでPCHを「使用しない」に設定

### Q: pch.h を変更したら全ファイル再ビルドされる
A: 正常な動作です。pch.h は頻繁に変更しないようにしましょう
