# チャージ&リリースシステム - ゲームメインシステム

## 概要

**チャージ&リリースシステム**は、スティックを傾けてチャージし、ニュートラルに戻すことで解放する、ゲームのメインシステムです。簡単に実装できるよう、専用APIを提供しています。

## 基本的な使い方

### チャージ検出

```cpp
// スティックがチャージ中かどうか
if (GetGamepad().IsLeftStickCharging(0)) {
    // チャージ中の処理
}
```

### リリース検出

```cpp
// スティックがリリースされた瞬間
if (GetGamepad().IsLeftStickReleased(0)) {
    // 解放時の処理
}
```

### チャージ量の取得

```cpp
// チャージ量を0.0～1.0で取得
float chargeAmount = GetGamepad().GetLeftStickChargeAmount(0, 3.0f);  // 3秒で最大
```

## 完全な実装例

### 基本的な射撃システム

```cpp
DEFINE_BEHAVIOUR(ChargeShootController,
    int playerIndex = 0;
    float maxChargeTime = 3.0f;
    float maxPower = 20.0f;
    float minPower = 5.0f;
,
    if (!GetGamepad().IsConnected(playerIndex)) return;

    // チャージ中
    if (GetGamepad().IsLeftStickCharging(playerIndex)) {
   float charge = GetGamepad().GetLeftStickChargeAmount(playerIndex, maxChargeTime);
        
        // UIにチャージゲージを表示
 // DrawChargeGauge(charge);
        
     // 最大チャージでバイブレーション
        if (charge >= 1.0f) {
 GetGamepad().SetVibration(playerIndex, 0.3f, 0.3f);
        }
    }
    else {
        GetGamepad().SetVibration(playerIndex, 0.0f, 0.0f);
    }

    // リリース時に発射
    if (GetGamepad().IsLeftStickReleased(playerIndex)) {
     float charge = GetGamepad().GetLeftStickChargeAmount(playerIndex, maxChargeTime);
        float power = minPower + (maxPower - minPower) * charge;
        
   // 弾を発射
        // world.EnqueueSpawn<Projectile>(transform->position, power);
      
        // 発射エフェクト
        GetGamepad().SetVibration(playerIndex, 0.8f, 0.8f);
    }
);
```

## 利用可能なAPI

### チャージ状態の取得

| API | 説明 | 戻り値 |
|-----|------|--------|
| `IsLeftStickCharging(index)` | 左スティックがチャージ中か | bool |
| `IsRightStickCharging(index)` | 右スティックがチャージ中か | bool |
| `IsLeftStickReleased(index)` | 左スティックがリリースされたか | bool |
| `IsRightStickReleased(index)` | 右スティックがリリースされたか | bool |

### チャージデータの取得

| API | 説明 | 戻り値 |
|-----|------|--------|
| `GetLeftStickChargeTime(index)` | 左スティックのチャージ時間(秒) | float |
| `GetRightStickChargeTime(index)` | 右スティックのチャージ時間(秒) | float |
| `GetLeftStickChargeAmount(index, maxTime)` | 左スティックのチャージ量(0.0～1.0) | float |
| `GetRightStickChargeAmount(index, maxTime)` | 右スティックのチャージ量(0.0～1.0) | float |
| `GetLeftStickAverageIntensity(index)` | 左スティックの平均入力強度 | float |
| `GetRightStickAverageIntensity(index)` | 右スティックの平均入力強度 | float |

## 高度な使用例

### ダブルスティック同時チャージ

```cpp
DEFINE_BEHAVIOUR(DualChargeController,
    int playerIndex = 0;
    float syncWindow = 0.2f;  // 同時判定の時間窓
    float lastLeftRelease = -999.0f;
float lastRightRelease = -999.0f;
    float totalTime = 0.0f;
,
    totalTime += dt;

    // 左リリース
    if (GetGamepad().IsLeftStickReleased(playerIndex)) {
      lastLeftRelease = totalTime;
     
        // 右も最近リリースされていたら同時攻撃
        if ((totalTime - lastRightRelease) < syncWindow) {
    float leftCharge = GetGamepad().GetLeftStickChargeAmount(playerIndex, 2.0f);
     float rightCharge = GetGamepad().GetRightStickChargeAmount(playerIndex, 2.0f);
     float combinedPower = (leftCharge + rightCharge) * 15.0f;
     
            // 強力な攻撃を発動
          // ExecuteSpecialAttack(combinedPower);
        }
    }

    if (GetGamepad().IsRightStickReleased(playerIndex)) {
        lastRightRelease = totalTime;
    }
);
```

### タイミング判定システム

```cpp
DEFINE_BEHAVIOUR(ChargeTimingController,
    int playerIndex = 0;
    float perfectTiming = 1.5f;      // 完璧なタイミング(秒)
    float goodWindow = 0.2f;         // GOOD判定の窓
  float greatWindow = 0.1f; // GREAT判定の窓
,
    if (GetGamepad().IsLeftStickReleased(playerIndex)) {
        float chargeTime = GetGamepad().GetLeftStickChargeTime(playerIndex);
      float diff = fabsf(chargeTime - perfectTiming);
        
        if (diff < greatWindow) {
       // PERFECT! ボーナス x2.0
  GetGamepad().SetVibration(playerIndex, 1.0f, 1.0f);
        }
        else if (diff < goodWindow) {
            // GOOD! ボーナス x1.5
 GetGamepad().SetVibration(playerIndex, 0.5f, 0.5f);
        }
 else {
        // MISS...
        }
    }
);
```

### 入力強度を考慮したシステム

```cpp
if (GetGamepad().IsLeftStickReleased(0)) {
    float chargeAmount = GetGamepad().GetLeftStickChargeAmount(0, 3.0f);
    float avgIntensity = GetGamepad().GetLeftStickAverageIntensity(0);
    
    // チャージ時間と入力強度の両方を考慮
    float finalPower = chargeAmount * (0.5f + avgIntensity * 0.5f) * 20.0f;
    
 // スティックを強く倒し続けた場合、より強力になる
    // ShootProjectile(finalPower);
}
```

## 技術仕様

### チャージ検出閾値

- **デフォルト閾値**: 0.1 (スティックの傾きが10%以上でチャージと判定)
- **定数**: `GamepadSystem::CHARGE_DETECTION_THRESHOLD`

### 計測データ

チャージ中、以下のデータが自動的に記録されます:

1. **チャージ時間**: スティックを傾け始めてからの経過時間(秒)
2. **入力強度の累積**: スティックの傾き具合の合計
3. **サンプル数**: 計測フレーム数

### リリース判定

- **前フレーム**: チャージ中
- **このフレーム**: ニュートラル(閾値以下)

この条件を満たした時、`IsLeftStickReleased()` が **1フレームだけ** `true` を返します。

### データのリセット

チャージデータは以下のタイミングでリセットされます:

1. スティックがニュートラルに戻って **2フレーム経過後**
2. 新たにチャージを開始した時

これにより、リリース直後のフレームでも正確なチャージ量を取得できます。

## デバッグとチューニング

### チャージ情報のログ出力

```cpp
if (GetGamepad().IsLeftStickCharging(0)) {
  float time = GetGamepad().GetLeftStickChargeTime(0);
    float amount = GetGamepad().GetLeftStickChargeAmount(0, 3.0f);
    float intensity = GetGamepad().GetLeftStickAverageIntensity(0);
 
    std::ostringstream oss;
    oss << "チャージ: " << (amount * 100.0f) << "% "
        << "(" << time << "秒, 強度:" << (intensity * 100.0f) << "%)";
 DEBUGLOG(oss.str());
}
```

### 推奨パラメータ

| ゲームジャンル | 最大チャージ時間 | 完璧なタイミング | 用途 |
|--------------|-----------------|-----------------|------|
| アクション | 1.0～2.0秒 | 0.8～1.2秒 | 素早い操作 |
| パズル | 2.0～3.0秒 | 1.5～2.5秒 | じっくり考える |
| リズムゲーム | 0.5～1.5秒 | 1.0秒 | 音楽に合わせる |
| シューティング | 1.5～3.0秒 | なし | チャージ量重視 |

## ベストプラクティス

### ? 推奨

```cpp
// リリース時にチャージ量を取得
if (GetGamepad().IsLeftStickReleased(0)) {
    float charge = GetGamepad().GetLeftStickChargeAmount(0, 3.0f);
    Shoot(charge);
}

// チャージ中にリアルタイム表示
if (GetGamepad().IsLeftStickCharging(0)) {
  float charge = GetGamepad().GetLeftStickChargeAmount(0, 3.0f);
    UpdateChargeGaugeUI(charge);
}
```

### ? 非推奨

```cpp
// リリース後にチャージ量を取得しようとする(0になっている)
if (GetGamepad().IsLeftStickReleased(0)) {
    // 何もしない
}
// 次のフレーム
float charge = GetGamepad().GetLeftStickChargeAmount(0, 3.0f);  // 0.0!
```

## トラブルシューティング

### Q: チャージ量が常に0になる

A: `IsLeftStickReleased()` が `true` の **同じフレーム** で `GetLeftStickChargeAmount()` を呼び出してください。

### Q: 微妙なスティック操作でもチャージが始まる

A: `CHARGE_DETECTION_THRESHOLD` の値を大きくします(デフォルト0.1→0.2など)。

### Q: リリース判定が2回発生する

A: リリースは1フレームのみ `true` を返すので、フラグで管理してください。

## サンプルコード

完全な実装例は以下のファイルを参照してください:

- `include/samples/ChargeSystemSample.h` - 基本的な使用例
- `include/samples/GamepadSample.h` - 応用例

## 作成者

山内陽 - 2025
