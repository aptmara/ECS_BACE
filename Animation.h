#pragma once
#include "Component.h"
#include "Entity.h"
#include "World.h"
#include "TextureManager.h"
#include <vector>
#include <cmath>

// ========================================================
// SpriteAnimation - スプライトアニメーションコンポーネント
// ========================================================
// 【役割】
// 複数のテクスチャを順番に切り替えてパラパラアニメーションを実現
//
// 【使い方】
// SpriteAnimation anim;
// anim.frames = { tex1, tex2, tex3 }; // アニメーションフレーム
// anim.frameTime = 0.1f;              // 1フレーム0.1秒（10fps）
// anim.loop = true;                   // ループ再生
// world.Add<SpriteAnimation>(entity, anim);
//
// 【仕組み】
// 時間経過でframesを順番に切り替えていく
// ========================================================
struct SpriteAnimation : Behaviour {
    std::vector<TextureManager::TextureHandle> frames; // アニメーションフレーム
    float frameTime = 0.1f;   // 1フレームの表示時間（秒）
    bool loop = true;         // ループ再生するか
    bool playing = true;      // 再生中か
    
    // 内部状態（自動管理されるので触らなくてOK）
    float currentTime = 0.0f;
    size_t currentFrame = 0;
    bool finished = false;

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

    // 現在のテクスチャを取得
    TextureManager::TextureHandle GetCurrentTexture() const {
        if (frames.empty()) return TextureManager::INVALID_TEXTURE;
        return frames[currentFrame];
    }

    // アニメーションを再生
    void Play() {
        playing = true;
        finished = false;
    }

    // アニメーションを停止
    void Stop() {
        playing = false;
    }

    // アニメーションをリセット
    void Reset() {
        currentFrame = 0;
        currentTime = 0.0f;
        finished = false;
    }
};

// ========================================================
// UVAnimation - UVスクロールアニメーション
// ========================================================
// 【役割】
// テクスチャをスクロールさせて動いているように見せる
// （例: 流れる水、動く雲、ベルトコンベア）
//
// 【使い方】
// UVAnimation uv;
// uv.scrollSpeed = DirectX::XMFLOAT2{0.5f, 0.0f}; // 横方向に流れる
// world.Add<UVAnimation>(entity, uv);
//
// または簡潔に:
// world.Add<UVAnimation>(entity, UVAnimation{0.5f, 0.0f});
// ========================================================
struct UVAnimation : Behaviour {
    DirectX::XMFLOAT2 scrollSpeed{ 0.0f, 0.0f }; // UV座標のスクロール速度（単位/秒）
    DirectX::XMFLOAT2 currentOffset{ 0.0f, 0.0f }; // 現在のオフセット（自動更新）

    // デフォルトコンストラクタ
    UVAnimation() = default;
    
    // スクロール速度を指定するコンストラクタ
    explicit UVAnimation(const DirectX::XMFLOAT2& speed) : scrollSpeed(speed) {}
    
    // 個別にU,Vを指定するコンストラクタ
    UVAnimation(float u, float v) : scrollSpeed{u, v} {}

    void OnUpdate(World& w, Entity self, float dt) override {
        // スクロール量を加算
        currentOffset.x += scrollSpeed.x * dt;
        currentOffset.y += scrollSpeed.y * dt;
        
        // 0-1の範囲に正規化（繰り返し）
        currentOffset.x = fmodf(currentOffset.x, 1.0f);
        currentOffset.y = fmodf(currentOffset.y, 1.0f);
        
        if (currentOffset.x < 0.0f) currentOffset.x += 1.0f;
        if (currentOffset.y < 0.0f) currentOffset.y += 1.0f;
    }
};
