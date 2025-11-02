#pragma once
/**
 * @file GamepadSystem.h
 * @brief XInput と DirectInput を抽象化したゲームパッド入力管理システム
 * @author 山内陽
 * @date 2025
 * @version 6.0
 * 
 * @details
 * XInput と DirectInput を統合し、最大4つのゲームパッドの入力を管理します。
 * XInputデバイスを優先的に使用し、XInputで認識されないデバイスはDirectInputで処理します。
 */

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <Xinput.h>
#include <dinput.h>
#include <cstdint>
#include <cstring>
#include <cmath>

#pragma comment(lib, "xinput.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

/**
 * @class GamepadSystem
 * @brief ゲームパッド入力を統合管理するクラス
 * 
 * @details
 * XInput と DirectInput の両方をサポートし、接続されたゲームパッドを自動検出します。
 * 最大4つのゲームパッドを同時にサポートします。
 * 
 * @par 使用例
 * @code
 * GamepadSystem gamepad;
 * gamepad.Init();
 * 
 * while (running) {
 *  gamepad.Update();
 *     
 *     if (gamepad.GetButtonDown(0, GamepadSystem::Button_A)) {
 *       // プレイヤー0のAボタン押下時
 *     }
 *     
 *   float leftX = gamepad.GetLeftStickX(0);
 *     float leftY = gamepad.GetLeftStickY(0);
 * }
 * 
 * gamepad.Shutdown();
 * @endcode
 * 
 * @author 山内陽
 */
class GamepadSystem {
public:
    /**
     * @brief サポートする最大ゲームパッド数
     */
    static const int MAX_GAMEPADS = 4;

    /**
     * @enum GamepadButton
     * @brief ゲームパッドボタンの識別子(Xbox配置)
     */
    enum GamepadButton {
        Button_A = 0,     ///< Aボタン(下)
        Button_B = 1,      ///< Bボタン(右)
        Button_X = 2,           ///< Xボタン(左)
        Button_Y = 3,        ///< Yボタン(上)
        Button_LB = 4,          ///< 左バンパー
        Button_RB = 5, ///< 右バンパー
        Button_Back = 6,        ///< Backボタン
        Button_Start = 7,     ///< Startボタン
        Button_LS = 8,          ///< 左スティック押し込み
        Button_RS = 9,          ///< 右スティック押し込み
        Button_DPad_Up = 10,    ///< 十字キー上
        Button_DPad_Down = 11,  ///< 十字キー下
      Button_DPad_Left = 12,  ///< 十字キー左
        Button_DPad_Right = 13, ///< 十字キー右
        Button_Count = 14 ///< ボタン総数
    };

    /**
     * @brief デフォルトコンストラクタ
     */
    GamepadSystem();

    /**
     * @brief デストラクタ
     */
    ~GamepadSystem();

    /**
   * @brief ゲームパッドシステムの初期化
   * @return true 初期化成功, false 初期化失敗
     * 
     * @details
     * DirectInputの初期化とゲームパッドの列挙を行います。
     */
  bool Init();

    /**
     * @brief ゲームパッドシステムのシャットダウン
     * 
   * @details
     * DirectInputとすべてのデバイスリソースを解放します。
     */
    void Shutdown();

    /**
     * @brief 入力状態の更新(毎フレーム呼ぶ)
     * 
     * @details
     * XInputとDirectInputの状態を取得し、内部状態を更新します。
     * ボタンの押下・離された瞬間の判定はこの更新処理によって行われます。
     */
    void Update();

    /**
     * @brief ゲームパッドが接続されているか
     * @param[in] index ゲームパッドインデックス(0-3)
     * @return true 接続中, false 未接続
*/
    bool IsConnected(int index) const;

  /**
     * @brief ボタンが押されているか
     * @param[in] index ゲームパッドインデックス(0-3)
     * @param[in] button ボタン識別子
     * @return true 押されている, false 押されていない
     * 
     * @details
     * ボタンが押され続けている間、またはこのフレームで押された瞬間にtrueを返します。
     */
    bool GetButton(int index, GamepadButton button) const;

    /**
   * @brief ボタンがこのフレームで押された瞬間か
     * @param[in] index ゲームパッドインデックス(0-3)
     * @param[in] button ボタン識別子
     * @return true 押された瞬間, false それ以外
     * 
     * @details
  * ボタンが押された最初のフレームのみtrueを返します。
     */
    bool GetButtonDown(int index, GamepadButton button) const;

    /**
* @brief ボタンがこのフレームで離された瞬間か
  * @param[in] index ゲームパッドインデックス(0-3)
     * @param[in] button ボタン識別子
* @return true 離された瞬間, false それ以外
     */
    bool GetButtonUp(int index, GamepadButton button) const;

    /**
     * @brief 左スティックのX軸値を取得
     * @param[in] index ゲームパッドインデックス(0-3)
 * @return float X軸値(-1.0 ～ +1.0)
     * 
     * @details
     * デッドゾーン処理済みの値を返します。
     * -1.0が左、+1.0が右を表します。
  */
    float GetLeftStickX(int index) const;

    /**
     * @brief 左スティックのY軸値を取得
  * @param[in] index ゲームパッドインデックス(0-3)
   * @return float Y軸値(-1.0 ～ +1.0)
     * 
     * @details
     * デッドゾーン処理済みの値を返します。
     * -1.0が下、+1.0が上を表します。
  */
    float GetLeftStickY(int index) const;

    /**
     * @brief 右スティックのX軸値を取得
     * @param[in] index ゲームパッドインデックス(0-3)
     * @return float X軸値(-1.0 ～ +1.0)
     */
    float GetRightStickX(int index) const;

    /**
     * @brief 右スティックのY軸値を取得
     * @param[in] index ゲームパッドインデックス(0-3)
     * @return float Y軸値(-1.0 ～ +1.0)
     */
    float GetRightStickY(int index) const;

    /**
     * @brief 左トリガーの値を取得
     * @param[in] index ゲームパッドインデックス(0-3)
     * @return float トリガー値(0.0 ～ 1.0)
     * 
     * @details
     * 0.0が未押下、1.0が最大押下を表します。
     */
    float GetLeftTrigger(int index) const;

    /**
     * @brief 右トリガーの値を取得
     * @param[in] index ゲームパッドインデックス(0-3)
     * @return float トリガー値(0.0 ～ 1.0)
     */
    float GetRightTrigger(int index) const;

    /**
     * @brief バイブレーション(振動)を設定
     * @param[in] index ゲームパッドインデックス(0-3)
     * @param[in] leftMotor 左モーター強度(0.0 ～ 1.0)
     * @param[in] rightMotor 右モーター強度(0.0 ～ 1.0)
     * 
     * @details
     * XInputデバイスのみサポートします。DirectInputデバイスでは何も起きません。
     * 左モーターは低周波、右モーターは高周波の振動を生成します。
     */
    void SetVibration(int index, float leftMotor, float rightMotor);

    // ========================================================
    // チャージ&リリースシステム(ゲームメインシステム)
// ========================================================

    /**
 * @brief 左スティックがチャージ中かどうか
     * @param[in] index ゲームパッドインデックス(0-3)
     * @return true チャージ中, false ニュートラル
     * 
     * @details
   * スティックが傾けられている状態を返します。
     */
    bool IsLeftStickCharging(int index) const;

    /**
     * @brief 右スティックがチャージ中かどうか
     * @param[in] index ゲームパッドインデックス(0-3)
     * @return true チャージ中, false ニュートラル
  */
    bool IsRightStickCharging(int index) const;

    /**
     * @brief 左スティックのチャージ時間を取得
     * @param[in] index ゲームパッドインデックス(0-3)
   * @return float チャージ時間(秒)
 * 
     * @details
     * スティックを傾け続けている時間を返します。
     * ニュートラルに戻るとリセットされます。
     */
    float GetLeftStickChargeTime(int index) const;

    /**
     * @brief 右スティックのチャージ時間を取得
     * @param[in] index ゲームパッドインデックス(0-3)
     * @return float チャージ時間(秒)
     */
    float GetRightStickChargeTime(int index) const;

    /**
   * @brief 左スティックがこのフレームでリリースされたか
     * @param[in] index ゲームパッドインデックス(0-3)
     * @return true リリースされた, false リリースされていない
     * 
     * @details
     * スティックがチャージ状態からニュートラルに戻った瞬間にtrueを返します。
     * このフレームのみtrueで、次のフレームからfalseになります。
     * 
     * @par ゲームメインシステムでの使用例
* @code
     * if (GetGamepad().IsLeftStickReleased(0)) {
     *     float chargeTime = GetGamepad().GetLeftStickChargeTime(0);
  *     float power = min(chargeTime * 2.0f, 10.0f); // 最大10.0
     *  ShootProjectile(power);
     * }
     * @endcode
     */
    bool IsLeftStickReleased(int index) const;

    /**
     * @brief 右スティックがこのフレームでリリースされたか
     * @param[in] index ゲームパッドインデックス(0-3)
     * @return true リリースされた, false リリースされていない
     */
    bool IsRightStickReleased(int index) const;

    /**
     * @brief 左スティックのチャージ量を取得(0.0 ～ 1.0)
     * @param[in] index ゲームパッドインデックス(0-3)
     * @param[in] maxChargeTime 最大チャージ時間(秒、デフォルト3.0秒)
     * @return float チャージ量(0.0 ～ 1.0)
     * 
     * @details
     * チャージ時間を0.0～1.0の範囲に正規化して返します。
     * maxChargeTime秒で1.0(100%)に達します。
 * 
     * @par ゲームメインシステムでの使用例
     * @code
     * // リアルタイムでチャージ量を表示
     * if (GetGamepad().IsLeftStickCharging(0)) {
     *     float charge = GetGamepad().GetLeftStickChargeAmount(0, 2.0f);
     *     DrawChargeGauge(charge);
     * }
     * 
     * // リリース時に発射
     * if (GetGamepad().IsLeftStickReleased(0)) {
     *     float charge = GetGamepad().GetLeftStickChargeAmount(0, 2.0f);
     *     ShootWithPower(charge);
     * }
     * @endcode
     */
    float GetLeftStickChargeAmount(int index, float maxChargeTime = 3.0f) const;

    /**
     * @brief 右スティックのチャージ量を取得(0.0 ～ 1.0)
     * @param[in] index ゲームパッドインデックス(0-3)
     * @param[in] maxChargeTime 最大チャージ時間(秒、デフォルト3.0秒)
     * @return float チャージ量(0.0 ～ 1.0)
     */
    float GetRightStickChargeAmount(int index, float maxChargeTime = 3.0f) const;

    /**
     * @brief 左スティックの平均入力強度を取得
     * @param[in] index ゲームパッドインデックス(0-3)
     * @return float 平均入力強度(0.0 ～ 1.0)
     * 
     * @details
     * チャージ中のスティックの平均的な傾き具合を返します。
     * 強く傾けるほど値が大きくなります。
     * ニュートラルまたはチャージしていない場合は0.0を返します。
     */
    float GetLeftStickAverageIntensity(int index) const;

    /**
     * @brief 右スティックの平均入力強度を取得
     * @param[in] index ゲームパッドインデックス(0-3)
     * @return float 平均入力強度(0.0 ～ 1.0)
     */
    float GetRightStickAverageIntensity(int index) const;

private:
    /**
     * @enum ButtonState
     * @brief ボタンの状態
     */
    enum ButtonState : uint8_t {
        None = 0,      ///< 何も押されていない
      Down = 1,      ///< このフレームで押された
        Pressed = 2,   ///< 押され続けている
        Up = 3         ///< このフレームで離された
    };

    /**
     * @enum DeviceType
     * @brief デバイスタイプ
     */
    enum DeviceType {
        Type_None,      ///< 未接続
    Type_XInput,    ///< XInputデバイス
        Type_DInput     ///< DirectInputデバイス
    };

    /**
     * @struct GamepadState
     * @brief ゲームパッドの状態
     */
    struct GamepadState {
        DeviceType type;   ///< デバイスタイプ
        bool connected; ///< 接続状態
    uint8_t buttons[Button_Count];  ///< ボタン状態
   uint8_t prevButtons[Button_Count];///< 前フレームのボタン状態
        float leftStickX;     ///< 左スティックX
     float leftStickY;   ///< 左スティックY
        float rightStickX;        ///< 右スティックX
        float rightStickY;    ///< 右スティックY
        float leftTrigger; ///< 左トリガー
      float rightTrigger;          ///< 右トリガー
    LPDIRECTINPUTDEVICE8 dinputDevice;      ///< DirectInputデバイス
   DWORD xinputIndex; ///< XInputインデックス

        // チャージ&リリースシステム用
        bool leftStickWasCharging;   ///< 前フレームで左スティックがチャージ中だったか
        bool rightStickWasCharging;  ///< 前フレームで右スティックがチャージ中だったか
     float leftStickChargeTime;   ///< 左スティックチャージ時間
        float rightStickChargeTime;  ///< 右スティックチャージ時間
        float leftStickIntensitySum; ///< 左スティック強度累積値
        float rightStickIntensitySum;///< 右スティック強度累積値
        int leftStickChargeSamples;  ///< 左スティックチャージサンプル数
        int rightStickChargeSamples; ///< 右スティックチャージサンプル数
        float leftStickReleaseTimer; ///< 左スティックリリースタイマー
        float rightStickReleaseTimer; ///<右スティックリリースタイマー

     GamepadState() {
            type = Type_None;
 connected = false;
   memset(buttons, 0, sizeof(buttons));
        memset(prevButtons, 0, sizeof(prevButtons));
  leftStickX = leftStickY = 0.0f;
 rightStickX = rightStickY = 0.0f;
      leftTrigger = rightTrigger = 0.0f;
   dinputDevice = nullptr;
        xinputIndex = 0;
            
          // チャージシステム初期化
            leftStickWasCharging = false;
            rightStickWasCharging = false;
            leftStickChargeTime = 0.0f;
            rightStickChargeTime = 0.0f;
            leftStickIntensitySum = 0.0f;
            rightStickIntensitySum = 0.0f;
            leftStickChargeSamples = 0;
            rightStickChargeSamples = 0;
            leftStickReleaseTimer = 0.0f;
            rightStickReleaseTimer = 0.0f;
        }
    };

    /**
     * @brief XInputデバイスの状態を更新
     * @param[in] index ゲームパッドインデックス
     */
    void UpdateXInput(int index);

    /**
     * @brief DirectInputデバイスの状態を更新
     * @param[in] index ゲームパッドインデックス
     */
    void UpdateDInput(int index);

    /**
     * @brief チャージ&リリースシステムの更新
     * @param[in] index ゲームパッドインデックス
     * @param[in] dt デルタタイム(秒)
     * 
     * @details
     * スティックのチャージ時間と強度を計測します。
     */
    void UpdateChargeSystem(int index, float dt);

    /**
     * @brief スティック値にデッドゾーンを適用
     * @param[in] x X軸生値
     * @param[in] y Y軸生値
     * @param[out] outX 処理済みX軸値
     * @param[out] outY 処理済みY軸値
     * @param[in] deadzone デッドゾーン半径
     * 
     * @details
     * 円形デッドゾーンを適用し、正規化します。
     */
    void ApplyDeadzone(float x, float y, float& outX, float& outY, float deadzone) const;

    /**
     * @brief DirectInputデバイスの列挙コールバック
     */
    static BOOL CALLBACK EnumDevicesCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef);

    /**
     * @brief XInputデバイスかどうかを判定
     * @param[in] pGuidProductFromDirectInput DirectInputデバイスのGUID
     * @return true XInputデバイス, false DirectInputデバイス
     * 
     * @details
     * WMIを使用してデバイスIDを確認し、XInputデバイスを識別します。
     * XInputデバイスの場合はDirectInputで列挙しないようにします。
     */
    static bool IsXInputDevice(const GUID* pGuidProductFromDirectInput);

    GamepadState gamepads_[MAX_GAMEPADS];  ///< ゲームパッド状態
    LPDIRECTINPUT8 dinput_;        ///< DirectInput8インターフェース
    int nextDInputSlot_;       ///< 次に使用するDirectInputスロット
    float deltaTime_;    ///< 前フレームのデルタタイム

    // デッドゾーン定数
    static constexpr float XINPUT_LEFT_DEADZONE = 7849.0f / 32767.0f;   ///< 左スティックデッドゾーン
    static constexpr float XINPUT_RIGHT_DEADZONE = 8689.0f / 32767.0f;  ///< 右スティックデッドゾーン
    static constexpr float XINPUT_TRIGGER_THRESHOLD = 30.0f / 255.0f;   ///< トリガー閾値
    static constexpr float CHARGE_DETECTION_THRESHOLD = 0.1f; ///< チャージ検出閾値
};

/**
 * @brief グローバルなゲームパッドシステムインスタンスを取得
 * @return GamepadSystem& シングルトンインスタンス
 * 
 * @details
 * どこからでもアクセス可能なゲームパッドシステムのインスタンスを返します。
 * 
 * @par 使用例
 * @code
 * if (GetGamepad().GetButtonDown(0, GamepadSystem::Button_A)) {
 *  // プレイヤー0のAボタンが押された
 * }
 * @endcode
 * 
 * @author 山内陽
 */
inline GamepadSystem& GetGamepad() {
    static GamepadSystem instance;
    return instance;
}
