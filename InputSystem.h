#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <cstdint>
#include <cstring>

// ========================================================
// InputSystem - キーボード・マウス入力管理
// ========================================================
class InputSystem {
public:
    // キーの状態
    enum class KeyState : uint8_t {
        None = 0,
        Down = 1,      // 今フレーム押された
        Pressed = 2,   // 押され続けている
        Up = 3         // 今フレーム離された
    };
    
    // マウスボタン
    enum MouseButton {
        Left = 0,
        Right = 1,
        Middle = 2
    };

    // 初期化
    void Init() {
        memset(keyStates_, 0, sizeof(keyStates_));
        memset(prevKeyStates_, 0, sizeof(prevKeyStates_));
        memset(mouseStates_, 0, sizeof(mouseStates_));
        memset(prevMouseStates_, 0, sizeof(prevMouseStates_));
        mouseX_ = mouseY_ = 0;
        mouseDeltaX_ = mouseDeltaY_ = 0;
        mouseWheel_ = 0;
    }

    // 更新（メインループの最初で呼ぶ）
    void Update() {
        // 前フレームの状態を保存
        memcpy(prevKeyStates_, keyStates_, sizeof(keyStates_));
        memcpy(prevMouseStates_, mouseStates_, sizeof(mouseStates_));
        
        // 現在のキー状態を取得
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
        
        // マウス位置の更新
        POINT pt;
        if (GetCursorPos(&pt)) {
            int newX = pt.x;
            int newY = pt.y;
            mouseDeltaX_ = newX - mouseX_;
            mouseDeltaY_ = newY - mouseY_;
            mouseX_ = newX;
            mouseY_ = newY;
        }
        
        // マウスホイールはリセット
        mouseWheel_ = 0;
    }

    // キーが押されているか
    bool GetKey(int vkCode) const {
        if (vkCode < 0 || vkCode >= 256) return false;
        KeyState state = static_cast<KeyState>(keyStates_[vkCode]);
        return state == KeyState::Pressed || state == KeyState::Down;
    }

    // キーが今フレーム押されたか
    bool GetKeyDown(int vkCode) const {
        if (vkCode < 0 || vkCode >= 256) return false;
        return static_cast<KeyState>(keyStates_[vkCode]) == KeyState::Down;
    }

    // キーが今フレーム離されたか
    bool GetKeyUp(int vkCode) const {
        if (vkCode < 0 || vkCode >= 256) return false;
        return static_cast<KeyState>(keyStates_[vkCode]) == KeyState::Up;
    }

    // マウスボタンが押されているか
    bool GetMouseButton(MouseButton button) const {
        int vk = VK_LBUTTON;
        if (button == Right) vk = VK_RBUTTON;
        else if (button == Middle) vk = VK_MBUTTON;
        return GetKey(vk);
    }

    // マウスボタンが今フレーム押されたか
    bool GetMouseButtonDown(MouseButton button) const {
        int vk = VK_LBUTTON;
        if (button == Right) vk = VK_RBUTTON;
        else if (button == Middle) vk = VK_MBUTTON;
        return GetKeyDown(vk);
    }

    // マウスボタンが今フレーム離されたか
    bool GetMouseButtonUp(MouseButton button) const {
        int vk = VK_LBUTTON;
        if (button == Right) vk = VK_RBUTTON;
        else if (button == Middle) vk = VK_MBUTTON;
        return GetKeyUp(vk);
    }

    // マウス座標取得
    int GetMouseX() const { return mouseX_; }
    int GetMouseY() const { return mouseY_; }
    
    // マウスの移動量取得
    int GetMouseDeltaX() const { return mouseDeltaX_; }
    int GetMouseDeltaY() const { return mouseDeltaY_; }
    
    // マウスホイール取得
    int GetMouseWheel() const { return mouseWheel_; }
    
    // マウスホイールイベント（WM_MOUSEWHEELから呼ぶ）
    void OnMouseWheel(int delta) {
        mouseWheel_ = delta / 120; // 120単位で正規化
    }

private:
    uint8_t keyStates_[256];
    uint8_t prevKeyStates_[256];
    uint8_t mouseStates_[3];
    uint8_t prevMouseStates_[3];
    
    int mouseX_;
    int mouseY_;
    int mouseDeltaX_;
    int mouseDeltaY_;
    int mouseWheel_;
};

// グローバルな入力システムインスタンス
inline InputSystem& GetInput() {
    static InputSystem instance;
    return instance;
}
