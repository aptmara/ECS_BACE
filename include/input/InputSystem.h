#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <cstdint>
#include <cstring>

/**
 * @file InputSystem.h
 * @brief キーボード・マウス入力管理システム
 * @author 山内陽
 * @date 2024
 * @version 5.0
 * 
 * @details
 * このファイルはキーボードとマウスの入力を管理するシステムを提供します。
 */

/**
 * @class InputSystem
 * @brief キーボード・マウス入力を管理するクラス
 * 
 * @details
 * Windows APIを使用してキーボードとマウスの入力状態を管理します。
 * ゲームループ内で毎フレームUpdate()を呼び出すことで、入力状態が更新されます。
 * 
 * @par 使用例:
 * @code
 * InputSystem input;
 * input.Init();
 * 
 * while (running) {
 *     input.Update();
 *     
 *     if (input.GetKeyDown(VK_SPACE)) {
 *         // スペースキー押下
 *     }
 * }
 * @endcode
 * 
 * @author 山内陽
 */
class InputSystem {
public:
    /**
     * @enum KeyState
     * @brief キーの状態を表す列挙型
     */
    enum class KeyState : uint8_t {
        None = 0,      ///< 何も押されていない
        Down = 1,      ///< このフレームで押された
        Pressed = 2,   ///< 押され続けている
        Up = 3         ///< このフレームで離された
    };
    
    /**
     * @enum MouseButton
     * @brief マウスボタンの識別子
     */
    enum MouseButton {
        Left = 0,      ///< 左ボタン
        Right = 1,     ///< 右ボタン
        Middle = 2     ///< 中ボタン
    };

    /**
     * @brief 初期化
     */
    void Init() {
        memset(keyStates_, 0, sizeof(keyStates_));
        memset(prevKeyStates_, 0, sizeof(prevKeyStates_));
        memset(mouseStates_, 0, sizeof(mouseStates_));
        memset(prevMouseStates_, 0, sizeof(prevMouseStates_));
        mouseX_ = mouseY_ = 0;
        mouseDeltaX_ = mouseDeltaY_ = 0;
        mouseWheel_ = 0;
    }

    /**
     * @brief 入力状態の更新（毎フレーム呼ぶ）
     */
    void Update() {
        memcpy(prevKeyStates_, keyStates_, sizeof(keyStates_));
        memcpy(prevMouseStates_, mouseStates_, sizeof(mouseStates_));
        
        for (int i = 0; i < 256; ++i) {
            bool current = (GetAsyncKeyState(i) & 0x8000) != 0;
            bool prev = prevKeyStates_[i];
            
            if (current && !prev) {
                keyStates_[i] = static_cast<uint8_t>(KeyState::Down);
            } else if (current && prev) {
                keyStates_[i] = static_cast<uint8_t>(KeyState::Pressed);
            } else if (!current && prev) {
                keyStates_[i] = static_cast<uint8_t>(KeyState::Up);
            } else {
                keyStates_[i] = static_cast<uint8_t>(KeyState::None);
            }
        }
        
        POINT pt;
        if (GetCursorPos(&pt)) {
            int newX = pt.x;
            int newY = pt.y;
            mouseDeltaX_ = newX - mouseX_;
            mouseDeltaY_ = newY - mouseY_;
            mouseX_ = newX;
            mouseY_ = newY;
        }
        
        mouseWheel_ = 0;
    }

    /**
     * @brief キーが押されているか
     * @param[in] vkCode 仮想キーコード
     * @return true 押されている, false 押されていない
     */
    bool GetKey(int vkCode) const {
        if (vkCode < 0 || vkCode >= 256) return false;
        KeyState state = static_cast<KeyState>(keyStates_[vkCode]);
        return state == KeyState::Pressed || state == KeyState::Down;
    }

    /**
     * @brief キーがこのフレームで押された瞬間か
     * @param[in] vkCode 仮想キーコード
     * @return true 押された瞬間, false それ以外
     */
    bool GetKeyDown(int vkCode) const {
        if (vkCode < 0 || vkCode >= 256) return false;
        return static_cast<KeyState>(keyStates_[vkCode]) == KeyState::Down;
    }

    /**
     * @brief キーがこのフレームで離された瞬間か
     * @param[in] vkCode 仮想キーコード
     * @return true 離された瞬間, false それ以外
     */
    bool GetKeyUp(int vkCode) const {
        if (vkCode < 0 || vkCode >= 256) return false;
        return static_cast<KeyState>(keyStates_[vkCode]) == KeyState::Up;
    }

    /**
     * @brief マウスボタンが押されているか
     * @param[in] button マウスボタン
     * @return true 押されている, false 押されていない
     */
    bool GetMouseButton(MouseButton button) const {
        int vk = VK_LBUTTON;
        if (button == Right) vk = VK_RBUTTON;
        else if (button == Middle) vk = VK_MBUTTON;
        return GetKey(vk);
    }

    /**
     * @brief マウスボタンがこのフレームで押された瞬間か
     * @param[in] button マウスボタン
     * @return true 押された瞬間, false それ以外
     */
    bool GetMouseButtonDown(MouseButton button) const {
        int vk = VK_LBUTTON;
        if (button == Right) vk = VK_RBUTTON;
        else if (button == Middle) vk = VK_MBUTTON;
        return GetKeyDown(vk);
    }

    /**
     * @brief マウスボタンがこのフレームで離された瞬間か
     * @param[in] button マウスボタン
     * @return true 離された瞬間, false それ以外
     */
    bool GetMouseButtonUp(MouseButton button) const {
        int vk = VK_LBUTTON;
        if (button == Right) vk = VK_RBUTTON;
        else if (button == Middle) vk = VK_MBUTTON;
        return GetKeyUp(vk);
    }

    /**
     * @brief マウスX座標を取得
     * @return int X座標
     */
    int GetMouseX() const { return mouseX_; }
    
    /**
     * @brief マウスY座標を取得
     * @return int Y座標
     */
    int GetMouseY() const { return mouseY_; }
    
    /**
     * @brief マウスの移動量（X）を取得
     * @return int X方向移動量
     */
    int GetMouseDeltaX() const { return mouseDeltaX_; }
    
    /**
     * @brief マウスの移動量（Y）を取得
     * @return int Y方向移動量
     */
    int GetMouseDeltaY() const { return mouseDeltaY_; }
    
    /**
     * @brief マウスホイールの回転量を取得
     * @return int ホイール回転量
     */
    int GetMouseWheel() const { return mouseWheel_; }
    
    /**
     * @brief マウスホイールイベント
     * @param[in] delta ホイール回転量
     */
    void OnMouseWheel(int delta) {
        mouseWheel_ = delta / 120;
    }

private:
    uint8_t keyStates_[256];        ///< 現在のキー状態
    uint8_t prevKeyStates_[256];    ///< 前フレームのキー状態
    uint8_t mouseStates_[3];        ///< マウスボタン状態
    uint8_t prevMouseStates_[3];    ///< 前フレームのマウスボタン状態
    
    int mouseX_;        ///< マウスX座標
    int mouseY_;        ///< マウスY座標
    int mouseDeltaX_;   ///< マウスX移動量
    int mouseDeltaY_;   ///< マウスY移動量
    int mouseWheel_;    ///< マウスホイール回転量
};

/**
 * @brief グローバルな入力システムインスタンスを取得
 * @return InputSystem& シングルトンインスタンス
 * 
 * @author 山内陽
 */
inline InputSystem& GetInput() {
    static InputSystem instance;
    return instance;
}
