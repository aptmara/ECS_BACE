/**
 * @file RealtimeChargeSamples.h
 * @brief リアルタイムチャージシステムのサンプルコード集
 * @author 山内陽
 * @date 2025
 * @version 6.0
 *
 * @details
 * チャージ状態とチャージ時間をリアルタイムで取得するサンプル実装。
 */

#pragma once
#include "input/GamepadSystem.h"
#include "components/Component.h"
#include "app/DebugLog.h"
#include <DirectXMath.h>
#include <sstream>
#include <cmath>

/**
 * @brief リアルタイムチャージゲージ表示
 *
 * @details
 * スティックを傾けている間、毎フレームチャージ量を取得して表示します。
 * ゲージUI、エフェクト、バイブレーションをリアルタイム更新。
 */
DEFINE_BEHAVIOUR(RealtimeChargeGauge,
    int playerIndex = 0;
    float maxChargeTime = 3.0f;
    float logInterval = 0.5f;
    float logTimer = 0.0f;
,
    if (!GetGamepad().IsConnected(playerIndex)) return;

 // チャージ中の処理(毎フレーム実行)
    if (GetGamepad().IsLeftStickCharging(playerIndex)) {
        // リアルタイムでチャージデータを取得
        float currentTime = GetGamepad().GetLeftStickChargeTime(playerIndex);
        float currentAmount = GetGamepad().GetLeftStickChargeAmount(playerIndex, maxChargeTime);
        float currentIntensity = GetGamepad().GetLeftStickAverageIntensity(playerIndex);

        // ここでUIを更新(毎フレーム)
 // DrawChargeGauge(currentAmount);
        // SetGaugeColor(currentIntensity);

        // 最大チャージでバイブレーション
  if (currentAmount >= 1.0f) {
            GetGamepad().SetVibration(playerIndex, 0.3f, 0.3f);
        }
  else {
  GetGamepad().SetVibration(playerIndex, currentAmount * 0.2f, 0.0f);
        }

        // デバッグログ(0.5秒ごと)
        logTimer += dt;
        if (logTimer >= logInterval) {
  logTimer = 0.0f;
        std::ostringstream oss;
   oss << "[リアルタイム] チャージ: "
             << (currentAmount * 100.0f) << "% ("
          << currentTime << "秒, 強度:"
      << (currentIntensity * 100.0f) << "%)";
    DEBUGLOG(oss.str());
        }
 }
    else {
        logTimer = 0.0f;
     GetGamepad().SetVibration(playerIndex, 0.0f, 0.0f);
      // HideChargeGauge();
    }

    // リリース時の処理(1フレームのみ)
    if (GetGamepad().IsLeftStickReleased(playerIndex)) {
        float finalTime = GetGamepad().GetLeftStickChargeTime(playerIndex);
        float finalAmount = GetGamepad().GetLeftStickChargeAmount(playerIndex, maxChargeTime);

        std::ostringstream oss;
        oss << "[リリース!] 最終チャージ: "
   << (finalAmount * 100.0f) << "% (" << finalTime << "秒)";
        DEBUGLOG(oss.str());

        // 発射処理
        float power = finalAmount * 20.0f;
   // ShootProjectile(power);

     GetGamepad().SetVibration(playerIndex, 0.8f, 0.8f);
    }
);

/**
 * @brief チャージ段階システム(弱→中→強)
 *
 * @details
 * チャージ時間に応じて段階が変化し、リアルタイムでエフェクトを切り替えます。
 */
DEFINE_BEHAVIOUR(ChargeStageSystem,
    int playerIndex = 0;
    int currentStage = 0;
    float weakThreshold = 0.5f;
  float mediumThreshold = 1.5f;
    float strongThreshold = 2.5f;
,
    if (!GetGamepad().IsConnected(playerIndex)) return;

    if (GetGamepad().IsLeftStickCharging(playerIndex)) {
        // リアルタイムでチャージ時間を取得
        float chargeTime = GetGamepad().GetLeftStickChargeTime(playerIndex);

  // 段階判定(リアルタイム更新)
        int newStage = 0;
        if (chargeTime >= strongThreshold) newStage = 3;
        else if (chargeTime >= mediumThreshold) newStage = 2;
        else if (chargeTime >= weakThreshold) newStage = 1;

        // 段階が上がった瞬間の処理
        if (newStage > currentStage) {
            currentStage = newStage;

            std::ostringstream oss;
   const char* stageName = "";
            switch (currentStage) {
    case 1: stageName = "弱チャージ"; break;
           case 2: stageName = "中チャージ"; break;
         case 3: stageName = "強チャージ"; break;
       }
            oss << "[段階UP!] " << stageName << " (時間:" << chargeTime << "秒)";
            DEBUGLOG(oss.str());

 // エフェクト切り替え
      // SwitchChargeEffect(currentStage);

            // バイブレーションフィードバック
   float vibPower = currentStage * 0.25f;
    GetGamepad().SetVibration(playerIndex, vibPower, vibPower);
        }
    }
    else {
      currentStage = 0;
     GetGamepad().SetVibration(playerIndex, 0.0f, 0.0f);
    }

    if (GetGamepad().IsLeftStickReleased(playerIndex)) {
        const char* attackName = "";
     switch (currentStage) {
            case 1: attackName = "弱攻撃"; break;
  case 2: attackName = "中攻撃"; break;
   case 3: attackName = "強攻撃"; break;
   default: attackName = "無攻撃"; break;
        }

        std::ostringstream oss;
        oss << "[発動!] " << attackName;
  DEBUGLOG(oss.str());

 // ExecuteAttack(currentStage);
    }
);

/**
 * @brief リアルタイムパーティクルエミッター
 *
 * @details
 * チャージ量に応じてパーティクル生成頻度と見た目を変化させます。
 */
DEFINE_BEHAVIOUR(ChargeParticleEmitter,
    int playerIndex = 0;
    float maxChargeTime = 3.0f;
    float particleTimer = 0.0f;
    float baseInterval = 0.1f;
,
    if (!GetGamepad().IsConnected(playerIndex)) return;

    if (GetGamepad().IsLeftStickCharging(playerIndex)) {
     // リアルタイムでチャージデータを取得
        float chargeAmount = GetGamepad().GetLeftStickChargeAmount(playerIndex, maxChargeTime);
      float intensity = GetGamepad().GetLeftStickAverageIntensity(playerIndex);

        // チャージ量で生成頻度を変化(0%=10fps, 100%=40fps)
        float interval = baseInterval / (1.0f + chargeAmount * 3.0f);

        particleTimer += dt;
        if (particleTimer >= interval) {
     particleTimer -= interval;

   // パーティクル生成(チャージ量で色・サイズ・速度を変更)
    // SpawnChargeParticle(chargeAmount, intensity);

   // 25%ごとにログ出力
      int percentage = static_cast<int>(chargeAmount * 100.0f);
        if (percentage % 25 == 0 && percentage > 0) {
         std::ostringstream oss;
     oss << "[パーティクル] " << percentage << "%チャージ";
     // DEBUGLOG(oss.str());  // 頻繁すぎるのでコメントアウト
         }
        }

        // バイブレーションもチャージ量で変化
        GetGamepad().SetVibration(playerIndex, chargeAmount * 0.3f, 0.0f);
    }
    else {
        particleTimer = 0.0f;
        GetGamepad().SetVibration(playerIndex, 0.0f, 0.0f);
    }
);

/**
 * @brief チャージ音響コントローラー
 *
 * @details
 * チャージ量に応じて音のピッチとボリュームをリアルタイム変化。
 * マイルストーン(25%, 50%, 75%, 100%)でSE再生。
 */
DEFINE_BEHAVIOUR(ChargeAudioController,
    int playerIndex = 0;
    float maxChargeTime = 3.0f;
    float previousAmount = 0.0f;
    bool milestone25 = false;
    bool milestone50 = false;
    bool milestone75 = false;
    bool milestone100 = false;
,
    if (!GetGamepad().IsConnected(playerIndex)) return;

    if (GetGamepad().IsLeftStickCharging(playerIndex)) {
        // リアルタイムでチャージ量を取得
        float currentAmount = GetGamepad().GetLeftStickChargeAmount(playerIndex, maxChargeTime);

        // 音のピッチを変化(1.0 → 1.5)
        float pitch = 1.0f + currentAmount * 0.5f;
        // SetChargeLoopPitch(pitch);

        // 音量を変化(0.3 → 1.0)
        float volume = 0.3f + currentAmount * 0.7f;
     // SetChargeLoopVolume(volume);

        // マイルストーン検出(25%刻み)
      if (!milestone25 && currentAmount >= 0.25f) {
            milestone25 = true;
      DEBUGLOG("[音響] 25%チャージ到達!");
 // PlayChargeMilestone(1);
  GetGamepad().SetVibration(playerIndex, 0.2f, 0.2f);
        }
    if (!milestone50 && currentAmount >= 0.50f) {
   milestone50 = true;
        DEBUGLOG("[音響] 50%チャージ到達!");
   // PlayChargeMilestone(2);
       GetGamepad().SetVibration(playerIndex, 0.4f, 0.4f);
        }
        if (!milestone75 && currentAmount >= 0.75f) {
            milestone75 = true;
 DEBUGLOG("[音響] 75%チャージ到達!");
         // PlayChargeMilestone(3);
  GetGamepad().SetVibration(playerIndex, 0.6f, 0.6f);
        }
 if (!milestone100 && currentAmount >= 1.00f) {
 milestone100 = true;
    DEBUGLOG("[音響] 100%チャージ到達!");
            // PlayChargeMilestone(4);
 GetGamepad().SetVibration(playerIndex, 0.8f, 0.8f);
 }

  previousAmount = currentAmount;
    }
    else {
        // リセット
        previousAmount = 0.0f;
 milestone25 = false;
        milestone50 = false;
        milestone75 = false;
   milestone100 = false;
        // StopChargeLoop();
  GetGamepad().SetVibration(playerIndex, 0.0f, 0.0f);
    }

 if (GetGamepad().IsLeftStickReleased(playerIndex)) {
   float finalAmount = GetGamepad().GetLeftStickChargeAmount(playerIndex, maxChargeTime);
        // PlayReleaseSound(finalAmount);
    }
);

/**
 * @brief リアルタイムチャージデバッグ表示
 *
 * @details
 * すべてのチャージデータをリアルタイムでログ出力(デバッグ用)。
 */
DEFINE_BEHAVIOUR(ChargeDebugMonitor,
    int playerIndex = 0;
    float maxChargeTime = 3.0f;
    float updateInterval = 0.1f;
    float updateTimer = 0.0f;
,
    if (!GetGamepad().IsConnected(playerIndex)) return;

    updateTimer += dt;

    if (GetGamepad().IsLeftStickCharging(playerIndex)) {
        if (updateTimer >= updateInterval) {
            updateTimer = 0.0f;

   // すべてのデータをリアルタイム取得
            float chargeTime = GetGamepad().GetLeftStickChargeTime(playerIndex);
  float chargeAmount = GetGamepad().GetLeftStickChargeAmount(playerIndex, maxChargeTime);
        float intensity = GetGamepad().GetLeftStickAverageIntensity(playerIndex);
         float stickX = GetGamepad().GetLeftStickX(playerIndex);
        float stickY = GetGamepad().GetLeftStickY(playerIndex);

        std::ostringstream oss;
        oss << "[デバッグ] 時間:" << chargeTime << "s"
 << ", 量:" << (chargeAmount * 100.0f) << "%"
     << ", 強度:" << (intensity * 100.0f) << "%"
  << ", スティック:(" << stickX << ", " << stickY << ")";
DEBUGLOG(oss.str());
        }
    }

    if (GetGamepad().IsLeftStickReleased(playerIndex)) {
        float finalTime = GetGamepad().GetLeftStickChargeTime(playerIndex);
    float finalAmount = GetGamepad().GetLeftStickChargeAmount(playerIndex, maxChargeTime);
        float finalIntensity = GetGamepad().GetLeftStickAverageIntensity(playerIndex);

     std::ostringstream oss;
        oss << "[リリース!] 最終データ - "
       << "時間:" << finalTime << "s, "
            << "量:" << (finalAmount * 100.0f) << "%, "
         << "強度:" << (finalIntensity * 100.0f) << "%";
        DEBUGLOG(oss.str());
    }
);

/**
 * @brief 両スティックリアルタイム監視
 *
 * @details
 * 左右両方のスティックのチャージ状態をリアルタイムで監視。
 * 同時チャージ検出などに使用。
 */
DEFINE_BEHAVIOUR(DualStickRealtimeMonitor,
    int playerIndex = 0;
    float maxChargeTime = 2.0f;
    float logInterval = 0.5f;
    float logTimer = 0.0f;
,
    if (!GetGamepad().IsConnected(playerIndex)) return;

    bool leftCharging = GetGamepad().IsLeftStickCharging(playerIndex);
    bool rightCharging = GetGamepad().IsRightStickCharging(playerIndex);

    if (leftCharging || rightCharging) {
        logTimer += dt;
        if (logTimer >= logInterval) {
            logTimer = 0.0f;

            std::ostringstream oss;
         oss << "[両スティック] ";

    if (leftCharging) {
    float leftTime = GetGamepad().GetLeftStickChargeTime(playerIndex);
     float leftAmount = GetGamepad().GetLeftStickChargeAmount(playerIndex, maxChargeTime);
            oss << "左:" << (leftAmount * 100.0f) << "% ";
            }
       else {
       oss << "左:-- ";
     }

            if (rightCharging) {
 float rightTime = GetGamepad().GetRightStickChargeTime(playerIndex);
   float rightAmount = GetGamepad().GetRightStickChargeAmount(playerIndex, maxChargeTime);
    oss << "右:" << (rightAmount * 100.0f) << "%";
            }
            else {
     oss << "右:--";
     }

            // 両方チャージ中なら特別な表示
            if (leftCharging && rightCharging) {
      oss << " [同時チャージ中!]";
      GetGamepad().SetVibration(playerIndex, 0.3f, 0.3f);
            }

        DEBUGLOG(oss.str());
        }
 }
    else {
   logTimer = 0.0f;
   GetGamepad().SetVibration(playerIndex, 0.0f, 0.0f);
  }
);
