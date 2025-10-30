/**
 * @file PlayerComponents.h
 * @brief プレイヤー専用コンポーネント集
 * @author 山内陽
 * @date 2025
 * @version 1.0
 *
 * @details
 * このファイルはプレイヤーキャラクターに関連するコンポーネントを定義します。
 * 移動、射撃、ステータス管理などのプレイヤー専用機能を提供します。
 */
#pragma once

#include "components/Component.h"
#include "ecs/World.h"
#include "components/Transform.h"
#include "components/MeshRenderer.h"
#include "input/InputSystem.h"
#include "input/GamepadSystem.h"
#include <DirectXMath.h>

// ========================================================
// プレイヤー移動コンポーネント
// ========================================================

/**
 * @struct PlayerMovement
 * @brief プレイヤーの移動を管理するBehaviour
 *
 * @details
 * スティック入力に基づいてプレイヤーの移動を制御します。
 * スティックを倒している間はその方向に移動し、ニュートラルに戻った際には最後に入力された方向に基づいて慣性（ベロシティ）を与えます。
 * また、画面外に出ないように自動的に境界制限を行います。
 *
 * @par 使用例
 * @code
 * Entity player = world.Create()
 * .With<Transform>()
 * .With<MeshRenderer>()
 * .With<PlayerTag>()
 * .Build();
 *
 * auto& movement = world.Add<PlayerMovement>(player);
 * movement.input_ = &GetInput();
 * movement.speed =8.0f;
 * @endcode
 *
 * @note InputSystemへの参照を設定する必要があります
 * @see InputSystem
 */
struct PlayerMovement : Behaviour {
    InputSystem *input_ = nullptr;             ///< 入力システムへのポインタ
    GamepadSystem *gamepad_ = nullptr;         ///< ゲームパッドシステムへのポインタ
    float speed = 5.0f;                        ///< 移動速度(単位/秒)
    DirectX::XMFLOAT2 velocity = {0.0f, 0.0f}; ///< 現在の移動ベロシティ

    /**
 * @brief 毎フレーム更新処理
 * @param[in,out] w ワールド参照
 * @param[in] self このコンポーネントが付いているエンティティ
 * @param[in] dt デルタタイム(前フレームからの経過時間)
 *
 * @details
 * スティック入力を読み取り、プレイヤーの位置とベロシティを更新します。
 * 入力がない場合は最後のベロシティに基づいて移動を続けます。
 */
    void OnUpdate(World &w, Entity self, float dt) override {
        auto *t = w.TryGet<Transform>(self);
        if (!t || (!input_ && !gamepad_))
            return;

        DirectX::XMFLOAT2 inputDir = {0.0f, 0.0f};

        // キーボード入力の処理
        if (input_) {
            if (input_->GetKey('W') || input_->GetKey(VK_UP)) {
                inputDir.y += 1.0f;
            }
            if (input_->GetKey('S') || input_->GetKey(VK_DOWN)) {
                inputDir.y -= 1.0f;
            }
            if (input_->GetKey('A') || input_->GetKey(VK_LEFT)) {
                inputDir.x -= 1.0f;
            }
            if (input_->GetKey('D') || input_->GetKey(VK_RIGHT)) {
                inputDir.x += 1.0f;
            }
        }

        // ゲームパッド入力の処理
        if (gamepad_ && gamepad_->IsConnected(0)) {
            float leftStickX = gamepad_->GetLeftStickX(0);
            float leftStickY = gamepad_->GetLeftStickY(0);

            // デッドゾーン処理済みの値を使用
            inputDir.x += leftStickX;
            inputDir.y += leftStickY;
        }

        // 入力がある場合はベロシティを更新
        if (inputDir.x != 0.0f || inputDir.y != 0.0f) {
            velocity = {inputDir.x * speed, inputDir.y * speed};
        }

        // ベロシティに基づいて位置を更新
        t->position.x += velocity.x * dt;
        t->position.y += velocity.y * dt;

        // 画面外に出ないように制限
        const float limitX = 8.0f;
        const float limitY = 10.0f;
        if (t->position.x < -limitX)
            t->position.x = -limitX;
        if (t->position.x > limitX)
            t->position.x = limitX;
        if (t->position.y < -limitY)
            t->position.y = -limitY;
        if (t->position.y > limitY)
            t->position.y = limitY;
    }
};
