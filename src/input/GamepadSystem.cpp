/**
 * @file GamepadSystem.cpp
 * @brief ゲームパッド入力管理システムの実装
 * @author 山内陽
 * @date 2025
 * @version 6.0
 */

#include "input/GamepadSystem.h"
#include "app/DebugLog.h"
#include <wbemidl.h>
#include <oleauto.h>
#include <sstream>
#include <chrono>

#pragma comment(lib, "wbemuuid.lib")
#pragma comment(lib, "oleaut32.lib")

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p) = nullptr; } }
#endif

// ========================================================
// コンストラクタ / デストラクタ
// ========================================================

GamepadSystem::GamepadSystem()
 : dinput_(nullptr)
 , nextDInputSlot_(0)
 , deltaTime_(0.0f)
{
}

GamepadSystem::~GamepadSystem()
{
    Shutdown();
}

// ========================================================
// 初期化 / シャットダウン
// ========================================================

bool GamepadSystem::Init()
{
#ifdef _DEBUG
    DEBUGLOG_CATEGORY(DebugLog::Category::Input, "GamepadSystem::Init() - 初期化開始");
#endif

    // DirectInput8の作成
  HRESULT hr = DirectInput8Create(
  GetModuleHandle(nullptr),
    DIRECTINPUT_VERSION,
 IID_IDirectInput8,
   (LPVOID*)&dinput_,
 nullptr
    );

    if (FAILED(hr)) {
  std::ostringstream oss;
    oss << "GamepadSystem::Init() - DirectInput8の作成に失敗: HRESULT=0x" << std::hex << hr;
        DEBUGLOG_ERROR(oss.str());
  return false;
    }

    // DirectInputデバイスを列挙
    hr = dinput_->EnumDevices(
    DI8DEVCLASS_GAMECTRL,
        EnumDevicesCallback,
     this,
        DIEDFL_ATTACHEDONLY
    );

    if (FAILED(hr)) {
    std::ostringstream oss;
        oss << "GamepadSystem::Init() - デバイス列挙に失敗: HRESULT=0x" << std::hex << hr;
        DEBUGLOG_ERROR(oss.str());
        return false;
    }

#ifdef _DEBUG
  DEBUGLOG_CATEGORY(DebugLog::Category::Input, "GamepadSystem::Init() - 初期化完了");
#endif

    return true;
}

void GamepadSystem::Shutdown()
{
#ifdef _DEBUG
    DEBUGLOG_CATEGORY(DebugLog::Category::Input, "GamepadSystem::Shutdown() - シャットダウン開始");
#endif

    // すべてのDirectInputデバイスを解放
    for (int i = 0; i < MAX_GAMEPADS; ++i) {
  if (gamepads_[i].dinputDevice) {
    gamepads_[i].dinputDevice->Unacquire();
    SAFE_RELEASE(gamepads_[i].dinputDevice);
    }
        gamepads_[i] = GamepadState();
    }

    // DirectInputを解放
    SAFE_RELEASE(dinput_);
    nextDInputSlot_ = 0;

#ifdef _DEBUG
    DEBUGLOG_CATEGORY(DebugLog::Category::Input, "GamepadSystem::Shutdown() - シャットダウン完了");
#endif
}

// ========================================================
// 更新処理
// ========================================================

void GamepadSystem::Update()
{
 // デルタタイムを計算(簡易版: 60FPS想定)
    static auto lastTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> elapsed = currentTime - lastTime;
    deltaTime_ = elapsed.count();
    lastTime = currentTime;

    // XInputデバイスを最初にチェック(0-3のスロット)
    for (DWORD i = 0; i < XUSER_MAX_COUNT; ++i) {
        XINPUT_STATE state;
   ZeroMemory(&state, sizeof(XINPUT_STATE));

        DWORD result = XInputGetState(i, &state);

      if (result == ERROR_SUCCESS) {
   // XInputデバイスが接続されている
   if (gamepads_[i].type != Type_XInput) {
         gamepads_[i].type = Type_XInput;
   gamepads_[i].connected = true;
            gamepads_[i].xinputIndex = i;
#ifdef _DEBUG
    std::ostringstream oss;
    oss << "GamepadSystem::Update() - XInputデバイス接続: Index=" << i;
     DEBUGLOG_CATEGORY(DebugLog::Category::Input, oss.str());
#endif
      }
            UpdateXInput(static_cast<int>(i));
   }
  else {
          // XInputデバイスが切断された
        if (gamepads_[i].type == Type_XInput) {
        gamepads_[i].connected = false;
#ifdef _DEBUG
  std::ostringstream oss;
    oss << "GamepadSystem::Update() - XInputデバイス切断: Index=" << i;
      DEBUGLOG_CATEGORY(DebugLog::Category::Input, oss.str());
#endif
       }
        }
        
        // チャージシステムを更新
     UpdateChargeSystem(static_cast<int>(i), deltaTime_);
    }

    // DirectInputデバイスを更新
  for (int i = 0; i < MAX_GAMEPADS; ++i) {
     if (gamepads_[i].type == Type_DInput && gamepads_[i].dinputDevice) {
   UpdateDInput(i);
   UpdateChargeSystem(i, deltaTime_);
}
    }
}

void GamepadSystem::UpdateXInput(int index)
{
    if (index < 0 || index >= MAX_GAMEPADS) return;

    GamepadState& pad = gamepads_[index];
    XINPUT_STATE state;
    ZeroMemory(&state, sizeof(XINPUT_STATE));

  DWORD result = XInputGetState(pad.xinputIndex, &state);
    if (result != ERROR_SUCCESS) {
        pad.connected = false;
        return;
    }

    pad.connected = true;

 // 前フレームの状態を保存
    memcpy(pad.prevButtons, pad.buttons, sizeof(pad.buttons));

    // ボタン状態を更新
    const WORD buttons = state.Gamepad.wButtons;

    auto updateButton = [&](GamepadButton btn, WORD xinputBtn) {
 bool isDown = (buttons & xinputBtn) != 0;
        ButtonState prevState = static_cast<ButtonState>(pad.prevButtons[btn]);
     bool wasDown = (prevState == Down || prevState == Pressed);

        if (isDown) {
            pad.buttons[btn] = static_cast<uint8_t>(wasDown ? Pressed : Down);
    } else {
  pad.buttons[btn] = static_cast<uint8_t>(wasDown ? Up : None);
        }
    };

    updateButton(Button_A, XINPUT_GAMEPAD_A);
    updateButton(Button_B, XINPUT_GAMEPAD_B);
    updateButton(Button_X, XINPUT_GAMEPAD_X);
    updateButton(Button_Y, XINPUT_GAMEPAD_Y);
    updateButton(Button_LB, XINPUT_GAMEPAD_LEFT_SHOULDER);
    updateButton(Button_RB, XINPUT_GAMEPAD_RIGHT_SHOULDER);
    updateButton(Button_Back, XINPUT_GAMEPAD_BACK);
    updateButton(Button_Start, XINPUT_GAMEPAD_START);
    updateButton(Button_LS, XINPUT_GAMEPAD_LEFT_THUMB);
    updateButton(Button_RS, XINPUT_GAMEPAD_RIGHT_THUMB);
    updateButton(Button_DPad_Up, XINPUT_GAMEPAD_DPAD_UP);
    updateButton(Button_DPad_Down, XINPUT_GAMEPAD_DPAD_DOWN);
    updateButton(Button_DPad_Left, XINPUT_GAMEPAD_DPAD_LEFT);
  updateButton(Button_DPad_Right, XINPUT_GAMEPAD_DPAD_RIGHT);

    // スティック値を正規化してデッドゾーン適用
    float rawLeftX = static_cast<float>(state.Gamepad.sThumbLX) / 32767.0f;
    float rawLeftY = static_cast<float>(state.Gamepad.sThumbLY) / 32767.0f;
    float rawRightX = static_cast<float>(state.Gamepad.sThumbRX) / 32767.0f;
    float rawRightY = static_cast<float>(state.Gamepad.sThumbRY) / 32767.0f;

    ApplyDeadzone(rawLeftX, rawLeftY, pad.leftStickX, pad.leftStickY, XINPUT_LEFT_DEADZONE);
    ApplyDeadzone(rawRightX, rawRightY, pad.rightStickX, pad.rightStickY, XINPUT_RIGHT_DEADZONE);

    // トリガー値を正規化
    pad.leftTrigger = static_cast<float>(state.Gamepad.bLeftTrigger) / 255.0f;
  pad.rightTrigger = static_cast<float>(state.Gamepad.bRightTrigger) / 255.0f;

    // トリガー閾値適用
  if (pad.leftTrigger < XINPUT_TRIGGER_THRESHOLD) pad.leftTrigger = 0.0f;
    if (pad.rightTrigger < XINPUT_TRIGGER_THRESHOLD) pad.rightTrigger = 0.0f;
}

void GamepadSystem::UpdateDInput(int index)
{
    if (index < 0 || index >= MAX_GAMEPADS) return;

    GamepadState& pad = gamepads_[index];
    if (!pad.dinputDevice) return;

    // デバイスを取得
    HRESULT hr = pad.dinputDevice->Poll();
    if (FAILED(hr)) {
        hr = pad.dinputDevice->Acquire();
  if (FAILED(hr)) {
       pad.connected = false;
return;
        }
        pad.dinputDevice->Poll();
    }

    DIJOYSTATE2 js;
    hr = pad.dinputDevice->GetDeviceState(sizeof(DIJOYSTATE2), &js);
    if (FAILED(hr)) {
        pad.connected = false;
        return;
    }

    pad.connected = true;

  // 前フレームの状態を保存
    memcpy(pad.prevButtons, pad.buttons, sizeof(pad.buttons));

    // ボタン状態を更新(最大14ボタンをサポート)
    auto updateButton = [&](GamepadButton btn, int dinputBtn) {
        if (dinputBtn >= 128) return; // 範囲外

        bool isDown = (js.rgbButtons[dinputBtn] & 0x80) != 0;
        ButtonState prevState = static_cast<ButtonState>(pad.prevButtons[btn]);
      bool wasDown = (prevState == Down || prevState == Pressed);

        if (isDown) {
 pad.buttons[btn] = static_cast<uint8_t>(wasDown ? Pressed : Down);
        } else {
            pad.buttons[btn] = static_cast<uint8_t>(wasDown ? Up : None);
        }
    };

    // 一般的なボタンマッピング(Xbox配置を想定)
    updateButton(Button_A, 0);
    updateButton(Button_B, 1);
    updateButton(Button_X, 2);
    updateButton(Button_Y, 3);
    updateButton(Button_LB, 4);
    updateButton(Button_RB, 5);
updateButton(Button_Back, 6);
    updateButton(Button_Start, 7);
    updateButton(Button_LS, 8);
    updateButton(Button_RS, 9);

  // POV(十字キー)の処理
    bool dpadUp = false, dpadDown = false, dpadLeft = false, dpadRight = false;
    if (js.rgdwPOV[0] != 0xFFFFFFFF) {
        DWORD pov = js.rgdwPOV[0];
  dpadUp = (pov >= 31500 || pov <= 4500);
   dpadRight = (pov >= 4500 && pov <= 13500);
        dpadDown = (pov >= 13500 && pov <= 22500);
    dpadLeft = (pov >= 22500 && pov <= 31500);
    }

    auto updateDPad = [&](GamepadButton btn, bool isDown) {
        ButtonState prevState = static_cast<ButtonState>(pad.prevButtons[btn]);
        bool wasDown = (prevState == Down || prevState == Pressed);

  if (isDown) {
  pad.buttons[btn] = static_cast<uint8_t>(wasDown ? Pressed : Down);
        } else {
            pad.buttons[btn] = static_cast<uint8_t>(wasDown ? Up : None);
 }
  };

    updateDPad(Button_DPad_Up, dpadUp);
    updateDPad(Button_DPad_Down, dpadDown);
    updateDPad(Button_DPad_Left, dpadLeft);
    updateDPad(Button_DPad_Right, dpadRight);

  // スティック値を正規化(-1.0 ～ +1.0)
    float rawLeftX = (static_cast<float>(js.lX) - 32767.0f) / 32767.0f;
    float rawLeftY = -(static_cast<float>(js.lY) - 32767.0f) / 32767.0f; // Y軸反転
  float rawRightX = (static_cast<float>(js.lRx) - 32767.0f) / 32767.0f;
    float rawRightY = -(static_cast<float>(js.lRy) - 32767.0f) / 32767.0f; // Y軸反転

    ApplyDeadzone(rawLeftX, rawLeftY, pad.leftStickX, pad.leftStickY, XINPUT_LEFT_DEADZONE);
    ApplyDeadzone(rawRightX, rawRightY, pad.rightStickX, pad.rightStickY, XINPUT_RIGHT_DEADZONE);

    // トリガー値(Z軸とZ回転軸を使用)
    pad.leftTrigger = (static_cast<float>(js.lZ) / 65535.0f);
    pad.rightTrigger = (static_cast<float>(js.lRz) / 65535.0f);

    if (pad.leftTrigger < XINPUT_TRIGGER_THRESHOLD) pad.leftTrigger = 0.0f;
    if (pad.rightTrigger < XINPUT_TRIGGER_THRESHOLD) pad.rightTrigger = 0.0f;
}

// ========================================================
// 入力取得
// ========================================================

bool GamepadSystem::IsConnected(int index) const
{
    if (index < 0 || index >= MAX_GAMEPADS) return false;
    return gamepads_[index].connected;
}

bool GamepadSystem::GetButton(int index, GamepadButton button) const
{
    if (index < 0 || index >= MAX_GAMEPADS) return false;
    if (button < 0 || button >= Button_Count) return false;

    ButtonState state = static_cast<ButtonState>(gamepads_[index].buttons[button]);
    return state == Pressed || state == Down;
}

bool GamepadSystem::GetButtonDown(int index, GamepadButton button) const
{
    if (index < 0 || index >= MAX_GAMEPADS) return false;
    if (button < 0 || button >= Button_Count) return false;

    return static_cast<ButtonState>(gamepads_[index].buttons[button]) == Down;
}

bool GamepadSystem::GetButtonUp(int index, GamepadButton button) const
{
    if (index < 0 || index >= MAX_GAMEPADS) return false;
    if (button < 0 || button >= Button_Count) return false;

return static_cast<ButtonState>(gamepads_[index].buttons[button]) == Up;
}

float GamepadSystem::GetLeftStickX(int index) const
{
    if (index < 0 || index >= MAX_GAMEPADS) return 0.0f;
    return gamepads_[index].leftStickX;
}

float GamepadSystem::GetLeftStickY(int index) const
{
    if (index < 0 || index >= MAX_GAMEPADS) return 0.0f;
    return gamepads_[index].leftStickY;
}

float GamepadSystem::GetRightStickX(int index) const
{
    if (index < 0 || index >= MAX_GAMEPADS) return 0.0f;
    return gamepads_[index].rightStickX;
}

float GamepadSystem::GetRightStickY(int index) const
{
  if (index < 0 || index >= MAX_GAMEPADS) return 0.0f;
    return gamepads_[index].rightStickY;
}

float GamepadSystem::GetLeftTrigger(int index) const
{
    if (index < 0 || index >= MAX_GAMEPADS) return 0.0f;
    return gamepads_[index].leftTrigger;
}

float GamepadSystem::GetRightTrigger(int index) const
{
    if (index < 0 || index >= MAX_GAMEPADS) return 0.0f;
    return gamepads_[index].rightTrigger;
}

void GamepadSystem::SetVibration(int index, float leftMotor, float rightMotor)
{
    if (index < 0 || index >= MAX_GAMEPADS) return;

    const GamepadState& pad = gamepads_[index];
    if (pad.type != Type_XInput || !pad.connected) return;

    // 値を0.0-1.0にクランプ
    leftMotor = (leftMotor < 0.0f) ? 0.0f : (leftMotor > 1.0f) ? 1.0f : leftMotor;
    rightMotor = (rightMotor < 0.0f) ? 0.0f : (rightMotor > 1.0f) ? 1.0f : rightMotor;

    XINPUT_VIBRATION vibration;
    ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
    vibration.wLeftMotorSpeed = static_cast<WORD>(leftMotor * 65535.0f);
    vibration.wRightMotorSpeed = static_cast<WORD>(rightMotor * 65535.0f);

    XInputSetState(pad.xinputIndex, &vibration);
}

// ========================================================
// チャージ&リリースシステム
// ========================================================

void GamepadSystem::UpdateChargeSystem(int index, float dt)
{
    if (index < 0 || index >= MAX_GAMEPADS) return;
    if (!gamepads_[index].connected) return;

    GamepadState& pad = gamepads_[index];

    // 左スティックのチャージ判定
    float leftMagnitude = sqrtf(pad.leftStickX * pad.leftStickX + pad.leftStickY * pad.leftStickY);
    bool leftCharging = leftMagnitude > CHARGE_DETECTION_THRESHOLD;

    if (leftCharging) {
        // チャージ中
        pad.leftStickChargeTime += dt;
        pad.leftStickIntensitySum += leftMagnitude;
        pad.leftStickChargeSamples++;
        pad.leftStickWasCharging = true;
    }
    else {
        // ニュートラル
   if (pad.leftStickWasCharging) {
            // リリースされた瞬間(次のフレームでリセット)
      }
        else {
         // チャージデータをリセット
         pad.leftStickChargeTime = 0.0f;
     pad.leftStickIntensitySum = 0.0f;
            pad.leftStickChargeSamples = 0;
        }
      pad.leftStickWasCharging = false;
}

    // 右スティックのチャージ判定
    float rightMagnitude = sqrtf(pad.rightStickX * pad.rightStickX + pad.rightStickY * pad.rightStickY);
    bool rightCharging = rightMagnitude > CHARGE_DETECTION_THRESHOLD;

    if (rightCharging) {
        // チャージ中
        pad.rightStickChargeTime += dt;
        pad.rightStickIntensitySum += rightMagnitude;
        pad.rightStickChargeSamples++;
        pad.rightStickWasCharging = true;
    }
    else {
        // ニュートラル
  if (pad.rightStickWasCharging) {
      // リリースされた瞬間(次のフレームでリセット)
        }
    else {
            // チャージデータをリセット
 pad.rightStickChargeTime = 0.0f;
   pad.rightStickIntensitySum = 0.0f;
            pad.rightStickChargeSamples = 0;
      }
     pad.rightStickWasCharging = false;
    }
}

bool GamepadSystem::IsLeftStickCharging(int index) const
{
    if (index < 0 || index >= MAX_GAMEPADS) return false;
  const GamepadState& pad = gamepads_[index];
    
    float magnitude = sqrtf(pad.leftStickX * pad.leftStickX + pad.leftStickY * pad.leftStickY);
    return magnitude > CHARGE_DETECTION_THRESHOLD;
}

bool GamepadSystem::IsRightStickCharging(int index) const
{
    if (index < 0 || index >= MAX_GAMEPADS) return false;
    const GamepadState& pad = gamepads_[index];
    
  float magnitude = sqrtf(pad.rightStickX * pad.rightStickX + pad.rightStickY * pad.rightStickY);
    return magnitude > CHARGE_DETECTION_THRESHOLD;
}

float GamepadSystem::GetLeftStickChargeTime(int index) const
{
    if (index < 0 || index >= MAX_GAMEPADS) return 0.0f;
  return gamepads_[index].leftStickChargeTime;
}

float GamepadSystem::GetRightStickChargeTime(int index) const
{
    if (index < 0 || index >= MAX_GAMEPADS) return 0.0f;
    return gamepads_[index].rightStickChargeTime;
}

bool GamepadSystem::IsLeftStickReleased(int index) const
{
    if (index < 0 || index >= MAX_GAMEPADS) return false;
    const GamepadState& pad = gamepads_[index];
    
    // 前フレームでチャージ中、このフレームでニュートラル
    float magnitude = sqrtf(pad.leftStickX * pad.leftStickX + pad.leftStickY * pad.leftStickY);
    bool isNowCharging = magnitude > CHARGE_DETECTION_THRESHOLD;
    
    return pad.leftStickWasCharging && !isNowCharging;
}

bool GamepadSystem::IsRightStickReleased(int index) const
{
    if (index < 0 || index >= MAX_GAMEPADS) return false;
    const GamepadState& pad = gamepads_[index];
    
    // 前フレームでチャージ中、このフレームでニュートラル
    float magnitude = sqrtf(pad.rightStickX * pad.rightStickX + pad.rightStickY * pad.rightStickY);
    bool isNowCharging = magnitude > CHARGE_DETECTION_THRESHOLD;
    
    return pad.rightStickWasCharging && !isNowCharging;
}

float GamepadSystem::GetLeftStickChargeAmount(int index, float maxChargeTime) const
{
    if (index < 0 || index >= MAX_GAMEPADS) return 0.0f;
    if (maxChargeTime <= 0.0f) return 0.0f;
    
    float chargeTime = gamepads_[index].leftStickChargeTime;
    float amount = chargeTime / maxChargeTime;
    
    // 0.0 ～ 1.0 にクランプ
    if (amount < 0.0f) amount = 0.0f;
    if (amount > 1.0f) amount = 1.0f;
    
    return amount;
}

float GamepadSystem::GetRightStickChargeAmount(int index, float maxChargeTime) const
{
    if (index < 0 || index >= MAX_GAMEPADS) return 0.0f;
    if (maxChargeTime <= 0.0f) return 0.0f;
    
    float chargeTime = gamepads_[index].rightStickChargeTime;
    float amount = chargeTime / maxChargeTime;
    
    // 0.0 ～ 1.0 にクランプ
    if (amount < 0.0f) amount = 0.0f;
    if (amount > 1.0f) amount = 1.0f;
    
    return amount;
}

float GamepadSystem::GetLeftStickAverageIntensity(int index) const
{
    if (index < 0 || index >= MAX_GAMEPADS) return 0.0f;
    const GamepadState& pad = gamepads_[index];
    
    if (pad.leftStickChargeSamples == 0) return 0.0f;
    
    return pad.leftStickIntensitySum / static_cast<float>(pad.leftStickChargeSamples);
}

float GamepadSystem::GetRightStickAverageIntensity(int index) const
{
  if (index < 0 || index >= MAX_GAMEPADS) return 0.0f;
    const GamepadState& pad = gamepads_[index];
    
    if (pad.rightStickChargeSamples == 0) return 0.0f;
    
    return pad.rightStickIntensitySum / static_cast<float>(pad.rightStickChargeSamples);
}

// ========================================================
// ユーティリティ
// ========================================================

void GamepadSystem::ApplyDeadzone(float x, float y, float& outX, float& outY, float deadzone) const
{
    // 円形デッドゾーンの適用
    float magnitude = sqrtf(x * x + y * y);

    if (magnitude < deadzone) {
        // デッドゾーン内
        outX = 0.0f;
    outY = 0.0f;
    }
    else {
        // デッドゾーン外 - 正規化して再スケール
        float normalizedX = x / magnitude;
      float normalizedY = y / magnitude;

        // 最大値でクリップ
        if (magnitude > 1.0f) magnitude = 1.0f;

        // デッドゾーンからの相対値に調整
 magnitude = (magnitude - deadzone) / (1.0f - deadzone);

        outX = normalizedX * magnitude;
        outY = normalizedY * magnitude;
    }
}

// ========================================================
// DirectInput デバイス列挙
// ========================================================

BOOL CALLBACK GamepadSystem::EnumDevicesCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef)
{
    GamepadSystem* pThis = static_cast<GamepadSystem*>(pvRef);
    if (!pThis) return DIENUM_STOP;

 // XInputデバイスはスキップ
if (IsXInputDevice(&lpddi->guidProduct)) {
        return DIENUM_CONTINUE;
  }

 // 空きスロットを探す
    int slot = -1;
    for (int i = 0; i < MAX_GAMEPADS; ++i) {
  if (pThis->gamepads_[i].type == Type_None) {
    slot = i;
  break;
   }
  }

    if (slot == -1) {
     // 空きスロットなし
        return DIENUM_CONTINUE;
    }

    // DirectInputデバイスを作成
    LPDIRECTINPUTDEVICE8 device = nullptr;
  HRESULT hr = pThis->dinput_->CreateDevice(lpddi->guidInstance, &device, nullptr);
    if (FAILED(hr)) {
    std::ostringstream oss;
 oss << "GamepadSystem::EnumDevicesCallback() - デバイス作成失敗: HRESULT=0x" << std::hex << hr;
      DEBUGLOG_ERROR(oss.str());
        return DIENUM_CONTINUE;
    }

    // データフォーマットを設定
    hr = device->SetDataFormat(&c_dfDIJoystick2);
 if (FAILED(hr)) {
   std::ostringstream oss;
        oss << "GamepadSystem::EnumDevicesCallback() - データフォーマット設定失敗: HRESULT=0x" << std::hex << hr;
     DEBUGLOG_ERROR(oss.str());
    SAFE_RELEASE(device);
        return DIENUM_CONTINUE;
    }

  // 協調レベルを設定(バックグラウンドでも動作、排他的でない)
  hr = device->SetCooperativeLevel(GetActiveWindow(), DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
    if (FAILED(hr)) {
      std::ostringstream oss;
        oss << "GamepadSystem::EnumDevicesCallback() - 協調レベル設定失敗: HRESULT=0x" << std::hex << hr;
        DEBUGLOG_ERROR(oss.str());
  SAFE_RELEASE(device);
 return DIENUM_CONTINUE;
    }

    // デバイスを取得
    hr = device->Acquire();
    if (FAILED(hr)) {
        // 取得失敗は致命的ではない(後で再取得可能)
  }

    // ゲームパッド状態に設定
    pThis->gamepads_[slot].type = Type_DInput;
    pThis->gamepads_[slot].connected = true;
    pThis->gamepads_[slot].dinputDevice = device;

#ifdef _DEBUG
  std::ostringstream oss;
    oss << "GamepadSystem::EnumDevicesCallback() - DirectInputデバイス登録: Slot=" << slot 
      << ", Name=" << lpddi->tszProductName;
    DEBUGLOG_CATEGORY(DebugLog::Category::Input, oss.str());
#endif

    return DIENUM_CONTINUE;
}

bool GamepadSystem::IsXInputDevice(const GUID* pGuidProductFromDirectInput)
{
    IWbemLocator* pIWbemLocator = nullptr;
    IEnumWbemClassObject* pEnumDevices = nullptr;
    IWbemClassObject* pDevices[20] = {};
    IWbemServices* pIWbemServices = nullptr;
    BSTR bstrNamespace = nullptr;
    BSTR bstrDeviceID = nullptr;
    BSTR bstrClassName = nullptr;
    bool bIsXinputDevice = false;

    // CoInit
    HRESULT hr = CoInitialize(nullptr);
    bool bCleanupCOM = SUCCEEDED(hr);

    VARIANT var = {};
    VariantInit(&var);

    // WMI作成
    hr = CoCreateInstance(__uuidof(WbemLocator),
        nullptr,
        CLSCTX_INPROC_SERVER,
        __uuidof(IWbemLocator),
      (LPVOID*)&pIWbemLocator);
    if (FAILED(hr) || pIWbemLocator == nullptr)
goto LCleanup;

  bstrNamespace = SysAllocString(L"\\\\.\\root\\cimv2");  if (bstrNamespace == nullptr) goto LCleanup;
    bstrClassName = SysAllocString(L"Win32_PNPEntity");     if (bstrClassName == nullptr) goto LCleanup;
    bstrDeviceID = SysAllocString(L"DeviceID");     if (bstrDeviceID == nullptr)  goto LCleanup;

    // WMI接続
    hr = pIWbemLocator->ConnectServer(bstrNamespace, nullptr, nullptr, 0L,
  0L, nullptr, nullptr, &pIWbemServices);
    if (FAILED(hr) || pIWbemServices == nullptr)
        goto LCleanup;

    // セキュリティレベル設定
    hr = CoSetProxyBlanket(pIWbemServices,
     RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, nullptr,
        RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE,
 nullptr, EOAC_NONE);
    if (FAILED(hr))
     goto LCleanup;

    hr = pIWbemServices->CreateInstanceEnum(bstrClassName, 0, nullptr, &pEnumDevices);
    if (FAILED(hr) || pEnumDevices == nullptr)
        goto LCleanup;

    // デバイスをループ
    for (;;)
    {
    ULONG uReturned = 0;
   hr = pEnumDevices->Next(10000, sizeof(pDevices) / sizeof(pDevices[0]), pDevices, &uReturned);
      if (FAILED(hr))
         goto LCleanup;
        if (uReturned == 0)
         break;

        for (size_t iDevice = 0; iDevice < uReturned; ++iDevice)
        {
            // デバイスIDを取得
            hr = pDevices[iDevice]->Get(bstrDeviceID, 0L, &var, nullptr, nullptr);
     if (SUCCEEDED(hr) && var.vt == VT_BSTR && var.bstrVal != nullptr)
    {
         // "IG_"が含まれているかチェック(XInputデバイスの印)
  if (wcsstr(var.bstrVal, L"IG_"))
          {
     // VID/PIDを取得
     DWORD dwPid = 0, dwVid = 0;
          WCHAR* strVid = wcsstr(var.bstrVal, L"VID_");
          if (strVid && swscanf_s(strVid, L"VID_%4X", &dwVid) != 1)
             dwVid = 0;
            WCHAR* strPid = wcsstr(var.bstrVal, L"PID_");
             if (strPid && swscanf_s(strPid, L"PID_%4X", &dwPid) != 1)
dwPid = 0;

      // DInputデバイスのGUIDと比較
     DWORD dwVidPid = MAKELONG(dwVid, dwPid);
          if (dwVidPid == pGuidProductFromDirectInput->Data1)
        {
      bIsXinputDevice = true;
           goto LCleanup;
                    }
      }
     }
   VariantClear(&var);
         SAFE_RELEASE(pDevices[iDevice]);
        }
    }

LCleanup:
    VariantClear(&var);

    if (bstrNamespace)
        SysFreeString(bstrNamespace);
    if (bstrDeviceID)
        SysFreeString(bstrDeviceID);
    if (bstrClassName)
        SysFreeString(bstrClassName);

    for (size_t iDevice = 0; iDevice < sizeof(pDevices) / sizeof(pDevices[0]); ++iDevice)
        SAFE_RELEASE(pDevices[iDevice]);

    SAFE_RELEASE(pEnumDevices);
    SAFE_RELEASE(pIWbemLocator);
    SAFE_RELEASE(pIWbemServices);

    if (bCleanupCOM)
        CoUninitialize();

    return bIsXinputDevice;
}
