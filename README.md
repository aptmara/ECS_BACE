# Simple ECS + DirectX11 Example

## ファイル構成

このプロジェクトは、可読性を最重視して細かくファイル分割されています。

### ?? ディレクトリ構造

```
ECS_BACE/
├── main.cpp              # エントリーポイント
├── ECS/                  # ECSコアシステム
│   ├── Entity.h          # エンティティID定義
│   ├── Component.h       # コンポーネント基底クラス
│   └── World.h           # ECSワールド管理
├── Components/           # ゲームコンポーネント
│   ├── Transform.h       # 位置・回転・スケール
│   ├── MeshRenderer.h    # メッシュレンダリング
│   └── Rotator.h         # 自動回転Behaviour
├── Graphics/             # グラフィックスシステム
│   ├── GfxDevice.h       # DirectX11デバイス管理
│   ├── Camera.h          # カメラ（ビュー・プロジェクション）
│   └── RenderSystem.h    # レンダリングパイプライン
└── Application/          # アプリケーション層
    └── App.h             # アプリケーションメインクラス
```

### ?? 各ファイルの役割

#### **ECSコアシステム**
- **Entity.h** - エンティティのID構造体を定義
- **Component.h** - `IComponent`基底インターフェースと`Behaviour`基底クラス
- **World.h** - ECSワールド管理（エンティティ作成、コンポーネント追加/削除/取得、Behaviour更新）

#### **コンポーネント**
- **Transform.h** - 位置、回転、スケールを保持
- **MeshRenderer.h** - メッシュレンダリング用の色情報
- **Rotator.h** - 自動回転を行うBehaviour（OnUpdate実装）

#### **グラフィックス**
- **GfxDevice.h** - DirectX11デバイス、スワップチェーン、レンダーターゲット管理
- **Camera.h** - カメラのビュー・プロジェクション行列
- **RenderSystem.h** - シェーダー、パイプライン、ジオメトリ、描画ループ

#### **アプリケーション**
- **App.h** - ウィンドウ作成、初期化、メインループ
- **main.cpp** - WinMainエントリーポイント

### ?? 技術仕様

- **言語**: C++14互換
- **API**: DirectX 11
- **ビルドツール**: Visual Studio 2022
- **アーキテクチャ**: Entity Component System (ECS)

### ?? 設計方針

1. **単一責任の原則** - 各ファイルは1つの明確な役割を持つ
2. **依存関係の明確化** - インクルード関係が追いやすい構造
3. **カテゴリ分け** - 機能ごとにフォルダで整理
4. **コメントの充実** - 各セクションに日本語コメント付き

### ?? ビルド方法

1. Visual Studio 2022でソリューションを開く
2. ビルド構成を選択（Debug/Release, x64推奨）
3. ビルド実行

### ?? 拡張方法

新しいコンポーネントを追加する場合：
1. `Components/`に新しい`.h`ファイルを作成
2. `IComponent`または`Behaviour`を継承
3. 必要に応じて`World::Add<>()`で追加

新しいシステムを追加する場合：
1. 適切なフォルダに`.h`ファイルを作成
2. `App.h`から参照して初期化


朝の有機（無機物の姿）
