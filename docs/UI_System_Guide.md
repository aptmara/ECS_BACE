/**
 * @file UIDocumentation.md
 * @brief UIシステムの使用ガイド
 * @author 山内陽
 * @date 2025
 */

# UIシステム使用ガイド

## 概要

本プロジェクトのUIシステムは、ECSアーキテクチャに基づいて設計されており、Direct2D/DirectWriteを使用してテキストとUI要素を描画します。

## アーキテクチャ

### コンポーネント構成

```
UICanvas (ルート)
  └─ UITransform (位置・サイズ)
      ├─ UIPanel (背景パネル)
      ├─ UIText (テキストラベル)
      └─ UIButton (ボタン) + UIText
```

### システム構成

- **TextSystem**: DirectWriteによるテキスト描画エンジン
- **UIRenderSystem**: UI要素の描画処理 (Behaviour)
- **UIInteractionSystem**: マウス入力の処理 (Behaviour)

## 基本的な使い方

### 1. TextSystemの初期化

```cpp
#include "graphics/TextSystem.h"

TextSystem textSystem;
if (!textSystem.Init(gfx)) {
    DEBUGLOG_ERROR("TextSystem initialization failed");
 return;
}
```

### 2. フォントフォーマットの作成

```cpp
TextSystem::TextFormat titleFormat;
titleFormat.fontSize = 48.0f;
titleFormat.fontFamily = L"メイリオ";
titleFormat.alignment = DWRITE_TEXT_ALIGNMENT_CENTER;
titleFormat.paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
textSystem.CreateTextFormat("title", titleFormat);

TextSystem::TextFormat buttonFormat;
buttonFormat.fontSize = 24.0f;
buttonFormat.alignment = DWRITE_TEXT_ALIGNMENT_CENTER;
textSystem.CreateTextFormat("button", buttonFormat);
```

### 3. UIキャンバスの作成

```cpp
Entity canvas = world.Create()
    .With<UICanvas>()
    .Build();
```

### 4. UIシステムの作成

```cpp
Entity uiRenderSystem = world.Create()
    .With<UIRenderSystem>()
    .Build();

auto* renderSys = world.TryGet<UIRenderSystem>(uiRenderSystem);
if (renderSys) {
    renderSys->SetTextSystem(&textSystem);
    renderSys->SetScreenSize(1280.0f, 720.0f);
}

Entity uiInteractionSystem = world.Create()
    .With<UIInteractionSystem>()
    .Build();

auto* interactionSys = world.TryGet<UIInteractionSystem>(uiInteractionSystem);
if (interactionSys) {
    interactionSys->SetScreenSize(1280.0f, 720.0f);
}
```

### 5. UIUpdateループ

```cpp
void OnUpdate(World& world, InputSystem& input, float deltaTime) override {
    world.ForEach<UIInteractionSystem>([&](Entity e, UIInteractionSystem& sys) {
        if (!sys.input_) {
   sys.input_ = &input;
     }
    });
    
    world.Tick(deltaTime);
}
```

## UI要素の作成例

### テキストラベル

```cpp
UITransform transform;
transform.position = {100.0f, 50.0f};
transform.size = {400.0f, 50.0f};
transform.anchor = {0.0f, 0.0f};  // 左上基準
transform.pivot = {0.0f, 0.0f};

UIText text{L"Hello World"};
text.color = {1.0f, 1.0f, 1.0f, 1.0f};
text.formatId = "title";

Entity label = world.Create()
  .With<UITransform>(transform)
  .With<UIText>(text)
    .Build();
```

### ボタン

```cpp
UITransform transform;
transform.position = {0.0f, 0.0f};
transform.size = {200.0f, 60.0f};
transform.anchor = {0.5f, 0.5f};  // 画面中央基準
transform.pivot = {0.5f, 0.5f};

UIButton button;
button.onClick = []() {
    DEBUGLOG("Button clicked!");
};
button.normalColor = {0.2f, 0.3f, 0.5f, 1.0f};
button.hoverColor = {0.3f, 0.4f, 0.6f, 1.0f};
button.pressedColor = {0.15f, 0.25f, 0.45f, 1.0f};

UIText buttonText{L"Click Me"};
buttonText.color = {1.0f, 1.0f, 1.0f, 1.0f};
buttonText.formatId = "button";

Entity btn = world.Create()
    .With<UITransform>(transform)
    .With<UIButton>(button)
    .With<UIText>(buttonText)
    .Build();
```

### パネル

```cpp
UITransform transform;
transform.position = {0.0f, 0.0f};
transform.size = {300.0f, 400.0f};
transform.anchor = {0.5f, 0.5f};
transform.pivot = {0.5f, 0.5f};

UIPanel panel;
panel.color = {0.15f, 0.15f, 0.2f, 0.9f};

Entity panelEntity = world.Create()
    .With<UITransform>(transform)
    .With<UIPanel>(panel)
    .Build();
```

## UITransformの座標系

### アンカー (Anchor)

画面上の基準点を指定します。

- `{0.0f, 0.0f}`: 左上
- `{0.5f, 0.5f}`: 中央
- `{1.0f, 1.0f}`: 右下

### ピボット (Pivot)

UI要素自身の基準点を指定します。

- `{0.0f, 0.0f}`: UI要素の左上
- `{0.5f, 0.5f}`: UI要素の中央
- `{1.0f, 1.0f}`: UI要素の右下

### 実際のスクリーン座標の計算

```
screenX = (screenWidth * anchor.x) + position.x - (size.x * pivot.x)
screenY = (screenHeight * anchor.y) + position.y - (size.y * pivot.y)
```

### 配置例

#### 左上隅に配置
```cpp
transform.anchor = {0.0f, 0.0f};  // 左上基準
transform.pivot = {0.0f, 0.0f};
transform.position = {10.0f, 10.0f};  // 左上から10pxオフセット
```

#### 画面中央に配置
```cpp
transform.anchor = {0.5f, 0.5f};  // 中央基準
transform.pivot = {0.5f, 0.5f};
transform.position = {0.0f, 0.0f};  // オフセットなし
```

#### 右下隅に配置
```cpp
transform.anchor = {1.0f, 1.0f};  // 右下基準
transform.pivot = {1.0f, 1.0f};
transform.position = {-10.0f, -10.0f};  // 右下から10px内側
```

## ボタンの状態

UIButtonは4つの状態を持ちます:

1. **Normal**: 通常状態
2. **Hovered**: マウスカーソルが上に乗っている状態
3. **Pressed**: クリック中
4. **Disabled**: 無効状態

各状態に対して異なる色を設定できます:

```cpp
button.normalColor = {0.2f, 0.2f, 0.2f, 1.0f};
button.hoverColor = {0.3f, 0.3f, 0.3f, 1.0f};
button.pressedColor = {0.15f, 0.15f, 0.15f, 1.0f};
button.disabledColor = {0.1f, 0.1f, 0.1f, 0.5f};

button.enabled = false;  // 無効化
```

## テキストフォーマット

DirectWriteのテキストフォーマットオプション:

### 水平方向の配置 (alignment)
- `DWRITE_TEXT_ALIGNMENT_LEADING`: 左揃え
- `DWRITE_TEXT_ALIGNMENT_CENTER`: 中央揃え
- `DWRITE_TEXT_ALIGNMENT_TRAILING`: 右揃え

### 垂直方向の配置 (paragraphAlignment)
- `DWRITE_PARAGRAPH_ALIGNMENT_NEAR`: 上揃え
- `DWRITE_PARAGRAPH_ALIGNMENT_CENTER`: 中央揃え
- `DWRITE_PARAGRAPH_ALIGNMENT_FAR`: 下揃え

### フォントの太さ (weight)
- `DWRITE_FONT_WEIGHT_NORMAL`: 通常
- `DWRITE_FONT_WEIGHT_BOLD`: 太字
- `DWRITE_FONT_WEIGHT_LIGHT`: 細字

### フォントスタイル (style)
- `DWRITE_FONT_STYLE_NORMAL`: 標準
- `DWRITE_FONT_STYLE_ITALIC`: イタリック

## サンプルシーン

完全な実装例は `include/samples/UISceneSample.h` を参照してください。

```cpp
#include "samples/UISceneSample.h"

UITestScene uiScene;
sceneManager.ChangeScene(&uiScene);
```

## 注意事項

### TextSystemのライフサイクル

- `TextSystem`はシーンごとに初期化・シャットダウンする必要があります
- `OnEnter()`で`Init()`、`OnExit()`で`Shutdown()`を呼び出してください

### パフォーマンス

- テキストフォーマットはキャッシュされます
- ブラシ(色)もハッシュベースでキャッシュされます
- 大量のUI要素を動的に生成・破棄する場合はプーリングを検討してください

### Direct2DとDirect3Dの統合

- TextSystemは同じスワップチェインを使用します
- 描画順序:
  1. `GfxDevice::BeginFrame()` - 3D描画クリア
  2. 3D描画 (MeshRenderer等)
  3. `TextSystem::BeginDraw()` - 2D描画開始
  4. UI描画 (TextSystem::DrawText)
  5. `TextSystem::EndDraw()` - 2D描画終了
  6. `GfxDevice::EndFrame()` - Present

## トラブルシューティング

### テキストが表示されない

1. TextSystemが初期化されているか確認
2. TextFormatが作成されているか確認
3. UIRenderSystemにTextSystemが設定されているか確認
4. BeginDraw()/EndDraw()が呼ばれているか確認

### ボタンがクリックできない

1. UIInteractionSystemにInputSystemが設定されているか確認
2. UIButtonのenabledがtrueか確認
3. UITransformの座標が正しいか確認 (Contains()でデバッグ)

### 座標がずれる

1. アンカーとピボットの設定を確認
2. SetScreenSize()で正しい画面サイズが設定されているか確認
3. GetScreenPosition()の計算結果をデバッグ出力

## 拡張可能性

本UIシステムは以下の拡張に対応できます:

- **画像描画**: UIImageコンポーネントを追加
- **スライダー**: UISliderコンポーネントを追加
- **テキスト入力**: UITextInputコンポーネントを追加
- **スクロールビュー**: UIScrollViewコンポーネントを追加
- **アニメーション**: UIAnimationシステムを追加

拡張時はECSの原則を守り、Component/Systemの分離を維持してください。
