# GamepadSystem - XInput & DirectInput 統合入力システム

## 概要

`GamepadSystem` は、XInput と DirectInput を抽象化したゲームパッド入力管理システムです。最大4つのゲームパッドを同時にサポートし、XInputデバイスを優先的に使用します。

## 特徴

- **XInput と DirectInput の統合**: XInputデバイスを優先し、非XInputデバイスはDirectInputで処理
- **最大4プレイヤー対応**: 同時に4つのゲームパッドをサポート
- **自動デバイス検出**: 接続されたゲームパッドを自動で認識
- **デッドゾーン処理**: スティックとトリガーに適切なデッドゾーンを適用
- **振動サポート**: XInputデバイスの振動機能をサポート
- **C++14準拠**: プロジェクトのコーディング規約に完全準拠

## 使用方法

### 初期化とシャットダウン

```cpp
#include "input/GamepadSystem.h"

// 初期化
GamepadSystem& gamepad = GetGamepad();
gamepad.Init();

// メインループ
while (running) {
    gamepad.Update();
    
    // ゲーム処理
}

// シャットダウン
gamepad.Shutdown();
```

### ボタン入力の取得

```cpp
// プレイヤー0のAボタンが押されているか
if (GetGamepad().GetButton(0, GamepadSystem::Button_A)) {
 // 押され続けている間の処理
}

// Aボタンが押された瞬間
if (GetGamepad().GetButtonDown(0, GamepadSystem::Button_A)) {
    // ジャンプなど、1回だけ実行したい処理
}

// Aボタンが離された瞬間
if (GetGamepad().GetButtonUp(0, GamepadSystem::Button_A)) {
    // ボタンを離した時の処理
}
```

### スティック入力の取得

```cpp
// 左スティックの値を取得(-1.0 ～ +1.0)
float leftX = GetGamepad().GetLeftStickX(0);
float leftY = GetGamepad().GetLeftStickY(0);

// プレイヤーを移動
transform->position.x += leftX * moveSpeed * deltaTime;
transform->position.z += leftY * moveSpeed * deltaTime;

// 右スティックでカメラ回転
float rightX = GetGamepad().GetRightStickX(0);
float rightY = GetGamepad().GetRightStickY(0);

cameraRotation.y += rightX * sensitivity * deltaTime;
cameraRotation.x += rightY * sensitivity * deltaTime;
```

### トリガー入力の取得

```cpp
// 左トリガーと右トリガーの値(0.0 ～ 1.0)
float leftTrigger = GetGamepad().GetLeftTrigger(0);
float rightTrigger = GetGamepad().GetRightTrigger(0);

// ダッシュ判定
if (rightTrigger > 0.5f) {
    currentSpeed = dashSpeed;
}

// アクセル/ブレーキ
vehicleSpeed += rightTrigger * acceleration;
vehicleSpeed -= leftTrigger * braking;
```

### 振動(バイブレーション)

```cpp
// 振動を設定(0.0 ～ 1.0)
// 左モーター: 低周波振動
// 右モーター: 高周波振動
GetGamepad().SetVibration(0, 0.5f, 0.5f);

// 振動を停止
GetGamepad().SetVibration(0, 0.0f, 0.0f);
```

### 接続確認

```cpp
// ゲームパッドが接続されているか確認
if (GetGamepad().IsConnected(0)) {
    // プレイヤー0のゲームパッドが接続中
}
```

## ECSでの使用例

### プレイヤーコントローラー

```cpp
#include "samples/GamepadSample.h"

// プレイヤーエンティティ作成
Entity player = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
    .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
    .With<GamepadPlayerController>(0)  // プレイヤー0
    .Build();
```

### カメラコントローラー

```cpp
Entity camera = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{0, 2, -5})
  .With<GamepadCameraController>(0)
    .Build();
```

### デバッグ表示

```cpp
Entity debugMonitor = world.Create()
    .With<GamepadDebugDisplay>(0)
    .Build();
```

## ボタンマッピング(Xbox配置)

| ボタン識別子 | Xbox | PlayStation |
|----------|------|-------------|
| `Button_A` | A | ×(バツ) |
| `Button_B` | B | ○(マル) |
| `Button_X` | X | □(四角) |
| `Button_Y` | Y | △(三角) |
| `Button_LB` | LB | L1 |
| `Button_RB` | RB | R1 |
| `Button_Back` | Back/View | Share |
| `Button_Start` | Start/Menu | Options |
| `Button_LS` | 左スティック押込 | L3 |
| `Button_RS` | 右スティック押込 | R3 |
| `Button_DPad_Up` | 十字キー上 | 十字キー上 |
| `Button_DPad_Down` | 十字キー下 | 十字キー下 |
| `Button_DPad_Left` | 十字キー左 | 十字キー左 |
| `Button_DPad_Right` | 十字キー右 | 十字キー右 |

## デッドゾーン設定

- **左スティック**: 7849 / 32767 ? 0.24
- **右スティック**: 8689 / 32767 ? 0.27
- **トリガー**: 30 / 255 ? 0.12

これらの値はXInputの標準的なデッドゾーンに準拠しています。

## 技術仕様

### 対応デバイス

- **XInput**: Xbox 360/One/Series コントローラー
- **DirectInput**: レガシーゲームパッド、フライトスティック、その他

### XInputとDirectInputの優先順位

1. 最初にXInputデバイス(スロット0-3)をチェック
2. XInputで認識されないデバイスをDirectInputで列挙
3. 各フレームでXInputの接続状態を監視
4. DirectInputデバイスは初期化時に検出

### スレッドセーフティ

- `Update()` はメインスレッドから毎フレーム1回呼び出すことを想定
- WMIによるXInputデバイス判定は初期化時のみ実行

## デバッグログ

`_DEBUG` ビルドでは、以下の情報がログ出力されます:

- ゲームパッド接続/切断イベント
- デバイス初期化エラー
- DirectInputデバイス登録情報

```cpp
#ifdef _DEBUG
DEBUGLOG_CATEGORY(DebugLog::Category::Input, "XInputデバイス接続: Index=0");
#endif
```

## 注意事項

### XInputデバイスの重複回避

DirectInputでもXInputデバイスが列挙される場合があるため、`IsXInputDevice()` 関数でWMIを使用してデバイスを判定し、重複を回避しています。

### 協調レベル

DirectInputデバイスは `DISCL_BACKGROUND | DISCL_NONEXCLUSIVE` で初期化されるため、ウィンドウがフォーカスを失っても入力を受け付けます。

### 振動機能

振動はXInputデバイスのみサポートします。DirectInputデバイスで `SetVibration()` を呼び出しても何も起きません。

## ファイル構成

```
include/input/
  └── GamepadSystem.h       # ヘッダーファイル

src/input/
  └── GamepadSystem.cpp     # 実装ファイル

include/samples/
  └── GamepadSample.h       # 使用例とサンプルコンポーネント
```

## ライセンス

このコードはHEW_GAMEプロジェクトの一部です。

## 作成者

山内陽 - 2025

## バージョン履歴

- **v6.0** (2025): 初版リリース
  - XInput と DirectInput の統合
  - 最大4プレイヤー対応
  - ECS Behaviourサンプル追加
