/**
 * @file GamepadSample.h
 * @brief ゲームパッド入力システムの使用例
 * @author 山内陽
 * @date 2025
 * @version 6.0
 * 
 * @details
 * GamepadSystemの使用方法を示すサンプルコードです。
 */

#pragma once
#include "input/GamepadSystem.h"
#include "components/Component.h"
#include "app/DebugLog.h"
#include <DirectXMath.h>
#include <sstream>
#include <cmath>

/**
 * @brief ゲームパッドでプレイヤーを操作するBehaviour
 * 
 * @details
 * ゲームパッドの左スティックで移動、Aボタンでジャンプ、
 * 右トリガーでダッシュを行います。
 * 
 * @par 使用例
 * @code
 * Entity player = world.Create()
 *     .With<Transform>(DirectX::XMFLOAT3{0,0,0})
 *     .With<GamepadPlayerController>(0) // プレイヤー0
 *     .Build();
 * @endcode
 */
DEFINE_BEHAVIOUR(GamepadPlayerController,
    int playerIndex = 0;    ///< ゲームパッドインデックス(0-3)
    float moveSpeed = 5.0f;     ///< 移動速度
    float dashSpeed = 10.0f;    ///< ダッシュ速度
    float rotateSpeed = 180.0f; ///< 回転速度(度/秒)
    bool isJumping = false;     ///< ジャンプ中フラグ
,
  // Transformを取得
    auto* transform = w.TryGet<Transform>(self);
    if (!transform) return;

    // ゲームパッド接続確認
    if (!GetGamepad().IsConnected(playerIndex)) {
        // 切断中
        return;
    }

    // 左スティックで移動
    float leftX = GetGamepad().GetLeftStickX(playerIndex);
    float leftY = GetGamepad().GetLeftStickY(playerIndex);

    // 移動速度を決定(右トリガーでダッシュ)
  float currentSpeed = moveSpeed;
    float rightTrigger = GetGamepad().GetRightTrigger(playerIndex);
    if (rightTrigger > 0.5f) {
        currentSpeed = dashSpeed;
    }

    // 移動を適用
    transform->position.x += leftX * currentSpeed * dt;
    transform->position.z += leftY * currentSpeed * dt;

    // 移動方向に回転
    if (leftX != 0.0f || leftY != 0.0f) {
      float targetAngle = atan2f(leftX, leftY);
        float currentAngle = transform->rotation.y;
        
        // 角度差を計算(-π ～ +π)
        float diff = targetAngle - currentAngle;
     while (diff > DirectX::XM_PI) diff -= DirectX::XM_2PI;
        while (diff < -DirectX::XM_PI) diff += DirectX::XM_2PI;

        // 滑らかに回転
    float maxRotate = rotateSpeed * dt * DirectX::XM_PI / 180.0f;
        if (fabsf(diff) < maxRotate) {
      transform->rotation.y = targetAngle;
    } else {
  transform->rotation.y += (diff > 0 ? maxRotate : -maxRotate);
    }
    }

    // Aボタンでジャンプ
 if (GetGamepad().GetButtonDown(playerIndex, GamepadSystem::Button_A)) {
   if (!isJumping) {
       transform->position.y += 2.0f; // 簡易ジャンプ
isJumping = true;
  }
    }

    // 地面に着地したらジャンプフラグリセット(簡易実装)
    if (transform->position.y <= 0.0f) {
  transform->position.y = 0.0f;
      isJumping = false;
    } else if (isJumping) {
 // 重力適用
    transform->position.y -= 9.8f * dt;
    }

    // Bボタンでバイブレーション
    if (GetGamepad().GetButton(playerIndex, GamepadSystem::Button_B)) {
   GetGamepad().SetVibration(playerIndex, 0.5f, 0.5f);
    } else {
        GetGamepad().SetVibration(playerIndex, 0.0f, 0.0f);
    }
);

/**
 * @brief ゲームパッドで視点を操作するBehaviour
 * 
 * @details
 * ゲームパッドの右スティックでカメラの回転を制御します。
 * 
 * @par 使用例
 * @code
 * Entity camera = world.Create()
 *   .With<Transform>(DirectX::XMFLOAT3{0,2,-5})
 *     .With<GamepadCameraController>(0)
 *     .Build();
 * @endcode
 */
DEFINE_BEHAVIOUR(GamepadCameraController,
    int playerIndex = 0;     ///< ゲームパッドインデックス
    float sensitivity = 2.0f;       ///< 感度
    float minPitch = -80.0f;        ///< 最小ピッチ角(度)
    float maxPitch = 80.0f;         ///< 最大ピッチ角(度)
,
    auto* transform = w.TryGet<Transform>(self);
    if (!transform) return;

    if (!GetGamepad().IsConnected(playerIndex)) return;

    // 右スティックで視点回転
    float rightX = GetGamepad().GetRightStickX(playerIndex);
    float rightY = GetGamepad().GetRightStickY(playerIndex);

    // ヨー(Y軸回転)
    transform->rotation.y += rightX * sensitivity * dt;

    // ピッチ(X軸回転)を制限付きで更新
    float pitchDelta = rightY * sensitivity * dt;
    float newPitch = transform->rotation.x + pitchDelta;
    
    float minPitchRad = minPitch * DirectX::XM_PI / 180.0f;
    float maxPitchRad = maxPitch * DirectX::XM_PI / 180.0f;
    
    if (newPitch < minPitchRad) newPitch = minPitchRad;
    if (newPitch > maxPitchRad) newPitch = maxPitchRad;
    
    transform->rotation.x = newPitch;
);

/**
 * @brief ゲームパッドの入力状態を表示するBehaviour
 * 
 * @details
 * デバッグ用:ゲームパッドの全ボタンとスティックの状態をログ出力します。
 */
DEFINE_BEHAVIOUR(GamepadDebugDisplay,
    int playerIndex = 0;
    float logInterval = 1.0f;  ///< ログ出力間隔(秒)
    float timer = 0.0f;
,
    timer += dt;
    
    if (timer >= logInterval) {
     timer = 0.0f;

        if (!GetGamepad().IsConnected(playerIndex)) {
     DEBUGLOG("ゲームパッド未接続");
         return;
        }

 // ボタン状態をログ出力
        if (GetGamepad().GetButton(playerIndex, GamepadSystem::Button_A)) {
          DEBUGLOG("Aボタン押下中");
        }
  if (GetGamepad().GetButtonDown(playerIndex, GamepadSystem::Button_B)) {
     DEBUGLOG("Bボタン押された");
  }

        // スティック値をログ出力
        float lx = GetGamepad().GetLeftStickX(playerIndex);
        float ly = GetGamepad().GetLeftStickY(playerIndex);
        if (lx != 0.0f || ly != 0.0f) {
       std::ostringstream oss;
oss << "左スティック: X=" << lx << ", Y=" << ly;
DEBUGLOG(oss.str());
        }

      // トリガー値をログ出力
        float lt = GetGamepad().GetLeftTrigger(playerIndex);
        float rt = GetGamepad().GetRightTrigger(playerIndex);
      if (lt > 0.0f || rt > 0.0f) {
        std::ostringstream oss;
         oss << "トリガー: L=" << lt << ", R=" << rt;
      DEBUGLOG(oss.str());
        }
    }
);

/**
 * @brief 複数プレイヤー用のゲームパッド接続監視
 * 
 * @details
 * 4人までのプレイヤーのゲームパッド接続状態を監視し、
 * 接続/切断時にログを出力します。
 */
DEFINE_DATA_COMPONENT(GamepadConnectionMonitor,
    bool wasConnected[4] = {false, false, false, false};

    void Update() {
        for (int i = 0; i < 4; ++i) {
         bool isNowConnected = GetGamepad().IsConnected(i);
       
     if (isNowConnected && !wasConnected[i]) {
     // 接続された
        std::ostringstream oss;
oss << "プレイヤー" << i << "のゲームパッド接続";
                DEBUGLOG(oss.str());
  wasConnected[i] = true;
   }
       else if (!isNowConnected && wasConnected[i]) {
         // 切断された
       std::ostringstream oss;
         oss << "プレイヤー" << i << "のゲームパッド切断";
   DEBUGLOG(oss.str());
                wasConnected[i] = false;
        }
      }
    }
);
