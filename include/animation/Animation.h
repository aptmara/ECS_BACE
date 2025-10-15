#pragma once
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
 * @date 2024
 * @version 5.0
 * 
 * @details
 * このファイルはスプライトアニメーションとUVスクロールアニメーションを
 * 実現するBehaviourコンポーネントを定義します。
 */

/**
 * @struct SpriteAnimation
 * @brief スプライトアニメーション（複数テクスチャの切り替え）コンポーネント
 * 
 * @details
 * 複数のテクスチャを順番に切り替えてパラパラアニメーションを実現します。
 * ループ再生や1回だけの再生にも対応しています。
 * 
 * @par 使用例（基本）:
 * @code
 * SpriteAnimation anim;
 * anim.frames = { tex1, tex2, tex3, tex4 };  // 4フレーム
 * anim.frameTime = 0.1f;                     // 1フレーム0.1秒（10fps）
 * anim.loop = true;                          // ループ再生
 * world.Add<SpriteAnimation>(entity, anim);
 * @endcode
 * 
 * @par 使用例（毎フレーム現在のテクスチャを取得）:
 * @code
 * auto* anim = world.TryGet<SpriteAnimation>(entity);
 * auto* renderer = world.TryGet<MeshRenderer>(entity);
 * if (anim && renderer) {
 *     renderer->texture = anim->GetCurrentTexture();
 * }
 * @endcode
 * 
 * @see UVAnimation UVスクロールアニメーション
 * @author 山内陽
 */
struct SpriteAnimation : Behaviour {
    std::vector<TextureManager::TextureHandle> frames;  ///< アニメーションフレーム（テクスチャ配列）
    float frameTime = 0.1f;   ///< 1フレームの表示時間（秒）
    bool loop = true;         ///< ループ再生するか
    bool playing = true;      ///< 再生中か
    
    float currentTime = 0.0f;   ///< 内部時間（触らなくてOK）
    size_t currentFrame = 0;    ///< 現在のフレーム番号
    bool finished = false;      ///< アニメーション終了フラグ

    /**
     * @brief 毎フレーム更新処理
     * @param[in,out] w ワールド参照
     * @param[in] self このコンポーネントが付いているエンティティ
     * @param[in] dt デルタタイム
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
     */
    TextureManager::TextureHandle GetCurrentTexture() const {
        if (frames.empty()) return TextureManager::INVALID_TEXTURE;
        return frames[currentFrame];
    }

    /**
     * @brief アニメーションを再生
     */
    void Play() {
        playing = true;
        finished = false;
    }

    /**
     * @brief アニメーションを停止
     */
    void Stop() {
        playing = false;
    }

    /**
     * @brief アニメーションをリセット
     */
    void Reset() {
        currentFrame = 0;
        currentTime = 0.0f;
        finished = false;
    }
};

/**
 * @struct UVAnimation
 * @brief UVスクロールアニメーション（テクスチャ移動）コンポーネント
 * 
 * @details
 * テクスチャをスクロールさせて流れているように見せます。
 * 用途: 流れる水、回るタイヤ、ベルトコンベア等
 * 
 * @par 使用例（横にスクロール）:
 * @code
 * UVAnimation uv;
 * uv.scrollSpeed = DirectX::XMFLOAT2{0.5f, 0.0f};  // 毎秒0.5横に移動
 * world.Add<UVAnimation>(entity, uv);
 * @endcode
 * 
 * @par 使用例（簡略版）:
 * @code
 * world.Add<UVAnimation>(entity, UVAnimation{0.5f, 0.0f});
 * @endcode
 * 
 * @par 使用例（MeshRendererに反映）:
 * @code
 * auto* uv = world.TryGet<UVAnimation>(entity);
 * auto* renderer = world.TryGet<MeshRenderer>(entity);
 * if (uv && renderer) {
 *     renderer->uvOffset = uv->currentOffset;
 * }
 * @endcode
 * 
 * @see SpriteAnimation スプライトアニメーション
 * @author 山内陽
 */
struct UVAnimation : Behaviour {
    DirectX::XMFLOAT2 scrollSpeed{ 0.0f, 0.0f };   ///< UV座標のスクロール速度（単位/秒）
    DirectX::XMFLOAT2 currentOffset{ 0.0f, 0.0f }; ///< 現在のオフセット（自動更新）

    /**
     * @brief デフォルトコンストラクタ
     */
    UVAnimation() = default;
    
    /**
     * @brief スクロール速度を指定するコンストラクタ
     * @param[in] speed スクロール速度
     */
    explicit UVAnimation(const DirectX::XMFLOAT2& speed) : scrollSpeed(speed) {}
    
    /**
     * @brief 個別にU,Vを指定するコンストラクタ
     * @param[in] u 横方向スクロール速度
     * @param[in] v 縦方向スクロール速度
     */
    UVAnimation(float u, float v) : scrollSpeed{u, v} {}

    /**
     * @brief 毎フレーム更新処理
     * @param[in,out] w ワールド参照
     * @param[in] self このコンポーネントが付いているエンティティ
     * @param[in] dt デルタタイム
     */
    void OnUpdate(World& w, Entity self, float dt) override {
        // スクロール量を加算
        currentOffset.x += scrollSpeed.x * dt;
        currentOffset.y += scrollSpeed.y * dt;
        
        // 0-1範囲に正規化（繰り返し）
        currentOffset.x = fmodf(currentOffset.x, 1.0f);
        currentOffset.y = fmodf(currentOffset.y, 1.0f);
        
        if (currentOffset.x < 0.0f) currentOffset.x += 1.0f;
        if (currentOffset.y < 0.0f) currentOffset.y += 1.0f;
    }
};
