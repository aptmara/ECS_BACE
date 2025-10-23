﻿#pragma once
#include "components/Component.h"
#include "ecs/Entity.h"
#include "ecs/World.h"
#include "graphics/TextureManager.h"
#include <vector>
#include <cmath>

/**
 * @file Animation.h
 * @brief アニメーションコンポーネントの定義
 * @author 山内陽
 * @date 2025
 * @version 5.0
 * 
 * @details
 * このファイルはスプライトアニメーションとUVスクロールアニメーションを
 * 実現するBehaviourコンポーネントを定義します。
 */

/**
 * @struct SpriteAnimation
 * @brief スプライトアニメーション(複数テクスチャの切り替え)コンポーネント
 * 
 * @details
 * 複数のテクスチャを順番に切り替えてパラパラアニメーションを実現します。
 * ループ再生や1回だけの再生にも対応しています。
 * 
 * ### アニメーションの仕組み:
 * 1. frames配列に複数のテクスチャを登録
 * 2. frameTime間隔でテクスチャを切り替え
 * 3. loopフラグで繰り返し再生を制御
 * 
 * @par 使用例(基本)
 * @code
 * SpriteAnimation anim;
 * anim.frames = { tex1, tex2, tex3, tex4 };  // 4フレーム
 * anim.frameTime = 0.1f;                     // 1フレーム0.1秒(10fps)
 * anim.loop = true;                          // ループ再生
 * world.Add<SpriteAnimation>(entity, anim);
 * @endcode
 * 
 * @par 使用例(毎フレーム現在のテクスチャを取得)
 * @code
 * auto* anim = world.TryGet<SpriteAnimation>(entity);
 * auto* renderer = world.TryGet<MeshRenderer>(entity);
 * if (anim && renderer) {
 *     renderer->texture = anim->GetCurrentTexture();
 * }
 * @endcode
 * 
 * @par 使用例(歩行アニメーション)
 * @code
 * // 4フレームの歩行アニメーション
 * SpriteAnimation walkAnim;
 * walkAnim.frames = {
 *     texManager.LoadFromFile("walk1.png"),
 *     texManager.LoadFromFile("walk2.png"),
 *     texManager.LoadFromFile("walk3.png"),
 *     texManager.LoadFromFile("walk4.png")
 * };
 * walkAnim.frameTime = 0.15f;  // 1フレーム0.15秒
 * walkAnim.loop = true;
 * @endcode
 * 
 * @see UVAnimation UVスクロールアニメーション
 * @author 山内陽
 */
struct SpriteAnimation : Behaviour {
    std::vector<TextureManager::TextureHandle> frames;  ///< アニメーションフレーム(テクスチャ配列)
    float frameTime = 0.1f;   ///< 1フレームの表示時間(秒)
    bool loop = true;         ///< ループ再生するか
    bool playing = true;      ///< 再生中か
    
    float currentTime = 0.0f;   ///< 内部時間(触らなくてOK)
    size_t currentFrame = 0;    ///< 現在のフレーム番号
    bool finished = false;      ///< アニメーション終了フラグ

    /**
     * @brief 毎フレーム更新処理
     * @param[in,out] w ワールド参照
     * @param[in] self このコンポーネントが付いているエンティティ
     * @param[in] dt デルタタイム
     * 
     * @details
     * 時間を加算し、frameTimeを超えたら次のフレームに進みます。
     * ループが無効で最後のフレームに達したら再生を停止します。
     */
    void OnUpdate(World& w, Entity self, float dt) override {
        if (!playing || frames.empty()) return;

        currentTime += dt;
        
        // フレーム切り替え
        if (currentTime >= frameTime) {
            currentTime -= frameTime;
            currentFrame++;
            
            if (currentFrame >= frames.size()) {
                if (loop) {
                    currentFrame = 0;
                } else {
                    currentFrame = frames.size() - 1;
                    playing = false;
                    finished = true;
                }
            }
        }
    }

    /**
     * @brief 現在のテクスチャを取得
     * @return TextureManager::TextureHandle 現在表示すべきテクスチャ
     * 
     * @details
     * MeshRendererに設定するテクスチャを取得します。
     * 
     * @par 使用例
     * @code
     * auto* renderer = world.TryGet<MeshRenderer>(entity);
     * auto* anim = world.TryGet<SpriteAnimation>(entity);
     * if (renderer && anim) {
     *     renderer->texture = anim->GetCurrentTexture();
     * }
     * @endcode
     */
    TextureManager::TextureHandle GetCurrentTexture() const {
        if (frames.empty()) return TextureManager::INVALID_TEXTURE;
        return frames[currentFrame];
    }

    /**
     * @brief アニメーションを再生
     * 
     * @details
     * 停止していたアニメーションを再開します。
     * finishedフラグもリセットされます。
     */
    void Play() {
        playing = true;
        finished = false;
    }

    /**
     * @brief アニメーションを停止
     * 
     * @details
     * アニメーションを一時停止します。
     * 現在のフレームは保持されます。
     */
    void Stop() {
        playing = false;
    }

    /**
     * @brief アニメーションをリセット
     * 
     * @details
     * 最初のフレームに戻り、時間もリセットします。
     * 再生状態は変更されません。
     */
    void Reset() {
        currentFrame = 0;
        currentTime = 0.0f;
        finished = false;
    }
};

/**
 * @struct UVAnimation
 * @brief UVスクロールアニメーション(テクスチャ移動)コンポーネント
 * 
 * @details
 * テクスチャをスクロールさせて流れているように見せます。
 * 用途: 流れる水、回るタイヤ、ベルトコンベア等
 * 
 * ### UVスクロールとは:
 * テクスチャの表示位置(UV座標)をずらすことで、
 * テクスチャが移動しているように見せる技術です。
 * 
 * @par 使用例(横にスクロール)
 * @code
 * UVAnimation uv;
 * uv.scrollSpeed = DirectX::XMFLOAT2{0.5f, 0.0f};  // 毎秒0.5横に移動
 * world.Add<UVAnimation>(entity, uv);
 * @endcode
 * 
 * @par 使用例(簡潔版)
 * @code
 * world.Add<UVAnimation>(entity, UVAnimation{0.5f, 0.0f});
 * @endcode
 * 
 * @par 使用例(MeshRendererに反映)
 * @code
 * auto* uv = world.TryGet<UVAnimation>(entity);
 * auto* renderer = world.TryGet<MeshRenderer>(entity);
 * if (uv && renderer) {
 *     renderer->uvOffset = uv->currentOffset;
 * }
 * @endcode
 * 
 * @par 使用例(流れる水)
 * @code
 * // 水のテクスチャをゆっくり斜めに流す
 * UVAnimation waterFlow{0.1f, 0.05f};  // 横0.1、縦0.05の速度
 * world.Add<UVAnimation>(waterEntity, waterFlow);
 * @endcode
 * 
 * @see SpriteAnimation スプライトアニメーション
 * @author 山内陽
 */
struct UVAnimation : Behaviour {
    DirectX::XMFLOAT2 scrollSpeed{ 0.0f, 0.0f };   ///< UV座標のスクロール速度(単位/秒)
    DirectX::XMFLOAT2 currentOffset{ 0.0f, 0.0f }; ///< 現在のオフセット(自動更新)

    /**
     * @brief デフォルトコンストラクタ
     */
    UVAnimation() = default;
    
    /**
     * @brief スクロール速度を指定するコンストラクタ
     * @param[in] speed スクロール速度
     * 
     * @par 使用例
     * @code
     * UVAnimation anim(DirectX::XMFLOAT2{0.5f, 0.0f});
     * @endcode
     */
    explicit UVAnimation(const DirectX::XMFLOAT2& speed) : scrollSpeed(speed) {}
    
    /**
     * @brief 個別にU,Vを指定するコンストラクタ
     * @param[in] u 横方向スクロール速度
     * @param[in] v 縦方向スクロール速度
     * 
     * @details
     * より直感的にスクロール速度を指定できます。
     * 
     * @par 使用例
     * @code
     * UVAnimation anim(0.5f, 0.0f);  // 横に毎秒0.5移動
     * @endcode
     */
    UVAnimation(float u, float v) : scrollSpeed{u, v} {}

    /**
     * @brief 毎フレーム更新処理
     * @param[in,out] w ワールド参照
     * @param[in] self このコンポーネントが付いているエンティティ
     * @param[in] dt デルタタイム
     * 
     * @details
     * スクロール量を加算し、0-1の範囲に正規化します。
     * 正規化することで、数値が無限に大きくなることを防ぎます。
     * 
     * @par 処理の詳細:
     * @code
     * // 例: scrollSpeed = {0.5, 0.0}, dt = 0.016(60FPS)
     * currentOffset.x += 0.5 * 0.016 = 0.008加算
     * 
     * // 1.0を超えたら引き算(テクスチャは繰り返すため)
     * if (currentOffset.x >= 1.0f) currentOffset.x -= 1.0f;
     * @endcode
     */
    void OnUpdate(World& w, Entity self, float dt) override {
        // スクロール量を加算
        currentOffset.x += scrollSpeed.x * dt;
        currentOffset.y += scrollSpeed.y * dt;
        
        // 0-1範囲に正規化(繰り返し)
        currentOffset.x = fmodf(currentOffset.x, 1.0f);
        currentOffset.y = fmodf(currentOffset.y, 1.0f);
        
        if (currentOffset.x < 0.0f) currentOffset.x += 1.0f;
        if (currentOffset.y < 0.0f) currentOffset.y += 1.0f;
    }
};
