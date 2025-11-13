/**
 * @file PlayerComponents.h
 * @brief プレイヤー専用コンポーネント集
 * @author 山内陽
 * @date 2025
 * @version 1.0
 *
 * @details
 * このファイルはプレイヤーキャラクターに関連するコンポーネントを定義します。
 * 移動、ステータス管理などのプレイヤー専用機能を提供します。
 */
#pragma once

#include "components/Component.h"
#include "ecs/World.h"
#include "components/Transform.h"
#include "components/MeshRenderer.h"
#include "input/InputSystem.h"
#include "input/GamepadSystem.h"
#include <DirectXMath.h>

// =========================================
// ベロシティ計算コンポーネント
// =========================================

struct PlayerVelocity : Behaviour {
    float speed = 10.0f;                       ///< 移動速度(単位/秒) - 速度を上げて動きを明確に
    DirectX::XMFLOAT2 velocity = {0.0f, 0.0f}; ///< 現在の移動ベロシティ

    void SetVelocity(DirectX::XMFLOAT2 speed)
    {
        velocity.x = speed.x;
        velocity.y = speed.y;
    }

    void UpdateVelocity(const DirectX::XMFLOAT2 &inputDir) {
        // 入力がある場合はベロシティを更新
        if (inputDir.x != 0.0f || inputDir.y != 0.0f) {

            // ベクトルの長さを計算
            float length = std::sqrt(inputDir.x * inputDir.x + inputDir.y * inputDir.y);

            if (length > 0.0f) {
                // 正規化し、speedを乗算してベロシティを更新
                float normal_x = inputDir.x / length;
                float normal_y = inputDir.y / length;
                velocity.x = normal_x * speed;
                velocity.y = normal_y * speed;
            }
        }
    }

    DirectX::XMFLOAT2 GetVelocity() {
        return velocity;
    }
};

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
    InputSystem *input_ = nullptr;     ///< 入力システムへのポインタ
    GamepadSystem *gamepad_ = nullptr; ///< ゲームパッドシステムへのポインタ

    /**
     * @brief 毎フレーム更新処理
     * @param[in,out] w ワールド参照
     * @param[in] self このコンポーネントが付いているエンティティ
     * @param[in] dt デルタタイム(前フレームからの経過時間)
     *
     * @details
     * スティック入力を読み取り、プレイヤーの位置とベロシティを更新します。
     * キーボード入力とすべての接続されているゲームパッド（XInput + DirectInput）の入力を統合します。
     * 入力がない場合は最後のベロシティに基づいて移動を続けます。
     */
    void OnUpdate(World &w, Entity self, float dt) override {
        auto *t = w.TryGet<Transform>(self);
        auto *v = w.TryGet<PlayerVelocity>(self);
       
        if (!t || !v || (!input_ && !gamepad_))
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

        // すべての接続されているゲームパッドの入力を統合（XInput + DirectInput）
        if (gamepad_) 
        {
            float gx = gamepad_->GetLeftStickX();
            float gy = gamepad_->GetLeftStickY();
#ifdef _DEBUG
            static int debugCounter = 0;
            if (debugCounter % 30 == 0 && (gx != 0.0f || gy != 0.0f)) { // 入力があるときだけログ出力
                DEBUGLOG("PlayerMovement: Gamepad input - LX=" + std::to_string(gx) + ", LY=" + std::to_string(gy));
            }
            debugCounter++;
#endif
           // inputDir.x += gx;
           // inputDir.y += gy; 
            static bool isCharging   = false;                       //チャージ中かどうか
            static float ChargePower = 0.0f;                        //チャージ具合
            static DirectX::XMFLOAT2 stickDir = {0.0f, 0.0f};       //スティックの傾き方向
            static float prev_gXY    = 0.0f;                        //前のスティック位置
            float angle = sqrtf(gx * gx + gy * gy);                 //sqrtf…平方根　これで角度を求める

            //チャージ(スティック傾け)
            if (angle > 0.5f)
            {
                isCharging = true;
                while (isCharging)
                {
                    //角度を求める処理
                    stickDir = {gx / angle, gy / angle};
                    float currentCharge = gamepad_->GetLeftStickChargeAmount(1.0f);

                    //経過時間に応じてチャージ量を溜める
                    ChargePower += currentCharge * dt;

                    //1.0以上は溜めれない
                    if (ChargePower > 1.0f)
                    {
                        ChargePower = 1.0f;
                    } 

                    v->speed = 1.0f;

                    break;
                }
            }

            //スティックが元の位置に戻ったらプレイヤー移動
            if (isCharging && prev_gXY > 0.5f && angle < 0.1f )
            {
                //テスト用
                //float Speed = 1.0f + ChargePower * 5.0f;
              
                inputDir.x = -stickDir.x * ChargePower;
                inputDir.y = -stickDir.y * ChargePower;

                //状態リセット
                isCharging  = false;
                ChargePower = 0.0f;
                stickDir    ={0.0f, 0.0f};
            }
            prev_gXY = angle;
        }

        v->UpdateVelocity(inputDir);

        // ベロシティに基づいて位置を更新
        t->position.x += v->velocity.x * dt;
        t->position.z += v->velocity.y * dt;

        // 画面外に出ないように制限
        const float limitX = 8.0f;
        const float limitY = 10.0f;
        if (t->position.x < -limitX)
            t->position.x = -limitX;
        if (t->position.x > limitX)
            t->position.x = limitX;
        if (t->position.z < -limitY)
            t->position.z = -limitY;
        if (t->position.z > limitY)
            t->position.z = limitY;
    }
};
