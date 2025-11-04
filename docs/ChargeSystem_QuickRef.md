# チャージシステム クイックリファレンス

## 基本的な使い方

### チャージ中かどうか確認

```cpp
if (GetGamepad().IsLeftStickCharging(0)) {
    // チャージ中の処理(毎フレーム実行)
}
```

### チャージ時間を取得(リアルタイム)

```cpp
float time = GetGamepad().GetLeftStickChargeTime(0);  // 秒単位
```

### チャージ量を取得(リアルタイム、0.0～1.0)

```cpp
float amount = GetGamepad().GetLeftStickChargeAmount(0, 3.0f);  // 3秒で100%
```

### リリースを検出

```cpp
if (GetGamepad().IsLeftStickReleased(0)) {
    // リリースされた瞬間の処理(1フレームのみ)
    float finalTime = GetGamepad().GetLeftStickChargeTime(0);
    float finalAmount = GetGamepad().GetLeftStickChargeAmount(0, 3.0f);
}
```

## 完全な実装例

```cpp
DEFINE_BEHAVIOUR(MyChargeSystem,
    int playerIndex = 0;
 float maxChargeTime = 3.0f;
,
    // チャージ中: リアルタイムでUI更新
    if (GetGamepad().IsLeftStickCharging(playerIndex)) {
   float amount = GetGamepad().GetLeftStickChargeAmount(playerIndex, maxChargeTime);
        // DrawChargeGauge(amount);  // 毎フレーム更新
        
        if (amount >= 1.0f) {
 GetGamepad().SetVibration(playerIndex, 0.3f, 0.3f);
        }
    }
    else {
    GetGamepad().SetVibration(playerIndex, 0.0f, 0.0f);
    }

    // リリース時: 発射
    if (GetGamepad().IsLeftStickReleased(playerIndex)) {
        float amount = GetGamepad().GetLeftStickChargeAmount(playerIndex, maxChargeTime);
        float power = amount * 20.0f;
        // ShootProjectile(power);
    }
);
```

## 利用可能なAPI一覧

| API | 戻り値 | リアルタイム | 説明 |
|-----|--------|------------|------|
| `IsLeftStickCharging(index)` | bool | ? | チャージ中か |
| `GetLeftStickChargeTime(index)` | float | ? | チャージ時間(秒) |
| `GetLeftStickChargeAmount(index, maxTime)` | float | ? | チャージ量(0.0～1.0) |
| `GetLeftStickAverageIntensity(index)` | float | ? | 平均入力強度 |
| `IsLeftStickReleased(index)` | bool | ? | リリースされたか(1フレームのみ) |

※右スティックも同様のAPIが利用可能

## よくある使い方

### パターン1: ゲージ表示

```cpp
if (GetGamepad().IsLeftStickCharging(0)) {
    float amount = GetGamepad().GetLeftStickChargeAmount(0, 3.0f);
    DrawChargeGauge(amount);  // 0.0～1.0をゲージに反映
}
```

### パターン2: 段階的なエフェクト

```cpp
float time = GetGamepad().GetLeftStickChargeTime(0);
if (time >= 2.5f) {
    ShowMaxChargeEffect();
}
else if (time >= 1.5f) {
    ShowMediumChargeEffect();
}
```

### パターン3: 音のピッチ変化

```cpp
if (GetGamepad().IsLeftStickCharging(0)) {
    float amount = GetGamepad().GetLeftStickChargeAmount(0, 3.0f);
    float pitch = 1.0f + amount * 0.5f;  // 1.0～1.5
    SetSoundPitch(pitch);
}
```

## サンプルコード

- `include/samples/ChargeSystemSample.h` - 基本例
- `include/samples/RealtimeChargeSamples.h` - リアルタイム更新例

## ドキュメント

- `docs/ChargeSystem_Guide.md` - 詳細ガイド
- `docs/RealtimeCharge_Guide.md` - リアルタイム取得ガイド
- `docs/GamepadSystem_README.md` - システム全体のドキュメント
