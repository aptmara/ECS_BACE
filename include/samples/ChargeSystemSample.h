/**
 * @file ChargeSystemSample.h
 * @brief チャージ&リリースシステムのサンプルコード
 * @author 山内陽
 * @date 2025
 * @version 6.0
 * 
 * @details
 * ゲームのメインシステムである「スティックを傾けてチャージ→離して解放」
 * の実装例を示します。
 */

#pragma once
#include "input/GamepadSystem.h"
#include "components/Component.h"
#include "app/DebugLog.h"
#include <DirectXMath.h>
#include <sstream>
#include <cmath>

/**
 * @brief 基本的なチャージ射撃システム
 * 
 * @details
 * 左スティックを傾けてチャージ、離すと発射します。
 * チャージ時間に応じて射撃パワーが変化します。
 */
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
        
 std::ostringstream oss;
        oss << "発射! パワー: " << power;
        DEBUGLOG(oss.str());
        
    GetGamepad().SetVibration(playerIndex, 0.8f, 0.8f);
    }
);

/**
 * @brief ダブルスティックチャージ
 * 
 * @details
 * 左右両方を同時にリリースすると強力な攻撃。
 */
DEFINE_BEHAVIOUR(DualChargeController,
  int playerIndex = 0;
    float maxChargeTime = 2.0f;
    float syncWindow = 0.2f;
    float lastLeftRelease = -999.0f;
    float lastRightRelease = -999.0f;
    float totalTime = 0.0f;
,
    if (!GetGamepad().IsConnected(playerIndex)) return;
    totalTime += dt;

    if (GetGamepad().IsLeftStickReleased(playerIndex)) {
      lastLeftRelease = totalTime;
      
        if ((totalTime - lastRightRelease) < syncWindow) {
            float leftCharge = GetGamepad().GetLeftStickChargeAmount(playerIndex, maxChargeTime);
      float rightCharge = GetGamepad().GetRightStickChargeAmount(playerIndex, maxChargeTime);
 float power = (leftCharge + rightCharge) * 15.0f;

            std::ostringstream oss;
            oss << "同時攻撃! パワー: " << power;
   DEBUGLOG(oss.str());
            
            GetGamepad().SetVibration(playerIndex, 1.0f, 1.0f);
        }
    }

    if (GetGamepad().IsRightStickReleased(playerIndex)) {
        lastRightRelease = totalTime;
    }
);

/**
 * @brief タイミング判定システム
 * 
 * @details
 * 特定のタイミングでリリースするとボーナス。
 */
DEFINE_BEHAVIOUR(ChargeTimingController,
    int playerIndex = 0;
    float perfectTime = 1.5f;
    float goodWindow = 0.2f;
    float greatWindow = 0.1f;
,
    if (!GetGamepad().IsConnected(playerIndex)) return;

    if (GetGamepad().IsLeftStickReleased(playerIndex)) {
        float chargeTime = GetGamepad().GetLeftStickChargeTime(playerIndex);
        float diff = fabsf(chargeTime - perfectTime);
        
        const char* judgment = "MISS";
        if (diff < greatWindow) {
          judgment = "PERFECT";
            GetGamepad().SetVibration(playerIndex, 1.0f, 1.0f);
        }
  else if (diff < goodWindow) {
            judgment = "GOOD";
      GetGamepad().SetVibration(playerIndex, 0.5f, 0.5f);
   }
        
        std::ostringstream oss;
        oss << "判定: " << judgment;
        DEBUGLOG(oss.str());
    }
);
