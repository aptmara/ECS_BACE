# リアルタイムチャージ取得ガイド

## 概要

GamepadSystemのチャージ&リリースシステムは、**リアルタイムでチャージ状態を取得**できます。スティックを傾けている間、毎フレーム最新のチャージ時間と状態を取得可能です。

## リアルタイム取得API

### チャージ状態の確認

```cpp
// チャージ中かどうか(毎フレーム確認可能)
bool isCharging = GetGamepad().IsLeftStickCharging(0);
```

### チャージ時間の取得

```cpp
// 現在のチャージ時間(秒)をリアルタイム取得
float chargeTime = GetGamepad().GetLeftStickChargeTime(0);

// チャージ中は増加し続ける
// 例: 0.0s → 0.016s → 0.033s → ... → 2.5s → ...
```

### チャージ量の取得

```cpp
// チャージ量(0.0～1.0)をリアルタイム取得
float chargeAmount = GetGamepad().GetLeftStickChargeAmount(0, 3.0f);

// 3秒で100%に達する
// 例: 0.0 → 0.01 → 0.02 → ... → 1.0
```

## 完全なリアルタイム実装例

### リアルタイムゲージ表示

```cpp
DEFINE_BEHAVIOUR(RealtimeChargeGauge,
    int playerIndex = 0;
  float maxChargeTime = 3.0f;
,
    if (!GetGamepad().IsConnected(playerIndex)) return;

    // リアルタイムでチャージ状態を取得
    bool isCharging = GetGamepad().IsLeftStickCharging(playerIndex);
    
    if (isCharging) {
  // 現在のチャージ時間(リアルタイム更新)
        float currentTime = GetGamepad().GetLeftStickChargeTime(playerIndex);
   
  // 現在のチャージ量(リアルタイム更新)
        float currentAmount = GetGamepad().GetLeftStickChargeAmount(playerIndex, maxChargeTime);
        
  // 現在の平均強度(リアルタイム更新)
        float currentIntensity = GetGamepad().GetLeftStickAverageIntensity(playerIndex);
        
   // UIに表示(毎フレーム更新)
        // DrawChargeGauge(currentAmount);
      // DrawChargeTime(currentTime);
        // DrawIntensityMeter(currentIntensity);
    
        // 最大チャージ時のフィードバック
        if (currentAmount >= 1.0f) {
            GetGamepad().SetVibration(playerIndex, 0.3f, 0.3f);
        }
    }
    else {
        GetGamepad().SetVibration(playerIndex, 0.0f, 0.0f);
        // HideChargeGauge();
    }

    // リリース時に発射
    if (GetGamepad().IsLeftStickReleased(playerIndex)) {
  float finalTime = GetGamepad().GetLeftStickChargeTime(playerIndex);
   float finalAmount = GetGamepad().GetLeftStickChargeAmount(playerIndex, maxChargeTime);
        
        // ShootProjectile(finalAmount * 20.0f);
    GetGamepad().SetVibration(playerIndex, 0.8f, 0.8f);
    }
);
```

### チャージ段階システム

```cpp
DEFINE_BEHAVIOUR(ChargeStageSystem,
    int playerIndex = 0;
    int currentStage = 0;  // 0=未チャージ, 1=弱, 2=中, 3=強
,
    if (!GetGamepad().IsConnected(playerIndex)) return;

    if (GetGamepad().IsLeftStickCharging(playerIndex)) {
    // リアルタイムでチャージ時間を取得
        float chargeTime = GetGamepad().GetLeftStickChargeTime(playerIndex);
        
        // チャージ段階を判定(リアルタイム更新)
        int newStage = 0;
        if (chargeTime >= 2.5f) newStage = 3;       // 強チャージ
        else if (chargeTime >= 1.5f) newStage = 2;  // 中チャージ
        else if (chargeTime >= 0.5f) newStage = 1;  // 弱チャージ
        
        // 段階が変わった瞬間にフィードバック
        if (newStage > currentStage) {
     currentStage = newStage;
    GetGamepad().SetVibration(playerIndex, 0.5f, 0.5f);
            
            // PlayStageUpSound(currentStage);
            // ShowStageEffect(currentStage);
        }
    }
    else {
      currentStage = 0;
    }

    // リリース時に段階に応じた攻撃
    if (GetGamepad().IsLeftStickReleased(playerIndex)) {
        switch (currentStage) {
 case 1: /* 弱攻撃 */ break;
        case 2: /* 中攻撃 */ break;
            case 3: /* 強攻撃 */ break;
  }
    }
);
```

### リアルタイムパーティクル生成

```cpp
DEFINE_BEHAVIOUR(ChargeParticleEmitter,
    int playerIndex = 0;
    float particleSpawnTimer = 0.0f;
    float particleInterval = 0.1f;  // パーティクル生成間隔
,
    if (!GetGamepad().IsConnected(playerIndex)) return;

    if (GetGamepad().IsLeftStickCharging(playerIndex)) {
        // リアルタイムでチャージ量を取得
        float chargeAmount = GetGamepad().GetLeftStickChargeAmount(playerIndex, 3.0f);
 float intensity = GetGamepad().GetLeftStickAverageIntensity(playerIndex);
        
      // チャージ量に応じてパーティクル生成頻度を変更
  float adjustedInterval = particleInterval / (1.0f + chargeAmount * 3.0f);
 
    particleSpawnTimer += dt;
  if (particleSpawnTimer >= adjustedInterval) {
            particleSpawnTimer = 0.0f;
            
            // パーティクル生成(チャージ量で色や大きさを変更)
          // SpawnChargeParticle(chargeAmount, intensity);
        }
        
        // バイブレーションもチャージ量に応じて変化
  GetGamepad().SetVibration(playerIndex, chargeAmount * 0.3f, chargeAmount * 0.3f);
    }
    else {
        particleSpawnTimer = 0.0f;
    GetGamepad().SetVibration(playerIndex, 0.0f, 0.0f);
    }
);
```

### リアルタイム音響フィードバック

```cpp
DEFINE_BEHAVIOUR(ChargeAudioController,
  int playerIndex = 0;
    float previousChargeAmount = 0.0f;
,
    if (!GetGamepad().IsConnected(playerIndex)) return;

    if (GetGamepad().IsLeftStickCharging(playerIndex)) {
        // リアルタイムでチャージ量を取得
        float currentAmount = GetGamepad().GetLeftStickChargeAmount(playerIndex, 3.0f);
        
    // チャージ量に応じてピッチを変化させる
        float pitch = 1.0f + currentAmount * 0.5f;  // 1.0 ～ 1.5
        // SetChargeLoopPitch(pitch);
     
        // チャージ量の変化に応じてボリュームも変化
float volume = 0.3f + currentAmount * 0.7f;  // 0.3 ～ 1.0
    // SetChargeLoopVolume(volume);
        
        // 25%、50%、75%、100%でワンショットSE
        if (previousChargeAmount < 0.25f && currentAmount >= 0.25f) {
        // PlayChargeMilestoneSound(1);
        }
        else if (previousChargeAmount < 0.50f && currentAmount >= 0.50f) {
    // PlayChargeMilestoneSound(2);
        }
        else if (previousChargeAmount < 0.75f && currentAmount >= 0.75f) {
// PlayChargeMilestoneSound(3);
     }
        else if (previousChargeAmount < 1.00f && currentAmount >= 1.00f) {
      // PlayChargeMilestoneSound(4);
            GetGamepad().SetVibration(playerIndex, 0.5f, 0.5f);
 }
     
        previousChargeAmount = currentAmount;
    }
  else {
        previousChargeAmount = 0.0f;
        // StopChargeLoop();
    }

  if (GetGamepad().IsLeftStickReleased(playerIndex)) {
        float finalAmount = GetGamepad().GetLeftStickChargeAmount(playerIndex, 3.0f);
  // PlayReleaseSound(finalAmount);
    }
);
```

## データ更新タイミング

### チャージ中

```
フレーム1: IsCharging=true,  Time=0.016s, Amount=0.005
フレーム2: IsCharging=true,  Time=0.032s, Amount=0.011
フレーム3: IsCharging=true,  Time=0.048s, Amount=0.016
...
フレーム60: IsCharging=true, Time=1.000s, Amount=0.333
...
```

### リリース時

```
フレーム180: IsCharging=true,  Released=false, Time=3.000s, Amount=1.000
フレーム181: IsCharging=false, Released=true,  Time=3.000s, Amount=1.000 ← リリース検出
フレーム182: IsCharging=false, Released=false, Time=0.000s, Amount=0.000 ← リセット
```

## ベストプラクティス

### ? 推奨される使い方

```cpp
// チャージ中のリアルタイム更新
if (GetGamepad().IsLeftStickCharging(0)) {
    float current = GetGamepad().GetLeftStickChargeTime(0);  // 毎フレーム最新値
    UpdateUI(current);
}

// リリース時の最終値取得
if (GetGamepad().IsLeftStickReleased(0)) {
    float final = GetGamepad().GetLeftStickChargeTime(0);    // リリース時の値
    Execute(final);
}
```

### ? 避けるべき使い方

```cpp
// チャージ状態を確認せずに時間を取得
float time = GetGamepad().GetLeftStickChargeTime(0);  // チャージ中でなければ0.0

// リリース後のフレームで値を取得
if (GetGamepad().IsLeftStickReleased(0)) {
    // 何もしない
}
// 次のフレーム
float time = GetGamepad().GetLeftStickChargeTime(0);  // 既に0.0にリセット済み
```

## パフォーマンスノート

- すべてのAPI呼び出しは**O(1)**の定数時間
- 毎フレーム呼び出しても問題なし
- 内部計算は `Update()` で完結しており、getter関数は値を返すのみ

## まとめ

| API | リアルタイム | リリース時 |
|-----|------------|----------|
| `IsLeftStickCharging()` | ? 毎フレーム更新 | ? false |
| `GetLeftStickChargeTime()` | ? 毎フレーム増加 | ? 最終値を1フレーム保持 |
| `GetLeftStickChargeAmount()` | ? 毎フレーム増加 | ? 最終値を1フレーム保持 |
| `IsLeftStickReleased()` | ? false | ? 1フレームのみtrue |

これにより、チャージ中のゲージ表示、段階的なエフェクト、音響フィードバックなど、リアルタイムな演出が簡単に実装できます!
