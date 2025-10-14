#pragma once
#include "Component.h"
#include "Entity.h"
#include "World.h"
#include "Transform.h"
#include <DirectXMath.h>

// ========================================================
// Rotator - 自動回転を行うBehaviour（動きのあるコンポーネント）
// ========================================================
// 【役割】
// エンティティを自動的にY軸周りで回転させる
//
// 【メンバ変数】
// - speedDegY: 回転速度（度/秒）
//
// 【使い方】
// Entity e = world.CreateEntity();
// world.Add<Transform>(e, Transform{...});
// world.Add<Rotator>(e, Rotator{45.0f}); // 毎秒45度回転
//
// 【仕組み】
// 毎フレームOnUpdate()が呼ばれ、Transformの回転値を更新する
// ========================================================
struct Rotator : Behaviour {
    float speedDegY = 45.0f; // Y軸周りの回転速度（度/秒）

    // デフォルトコンストラクタ
    Rotator() = default;
    
    // 回転速度を指定するコンストラクタ
    explicit Rotator(float s) : speedDegY(s) {}

    // 毎フレーム呼ばれる更新処理
    void OnUpdate(World& w, Entity self, float dt) override {
        // このエンティティのTransformを取得
        auto* t = w.TryGet<Transform>(self);
        if (!t) return; // Transformがなければ何もしない
        
        // 回転値を更新（dt = デルタタイム = 前フレームからの経過秒数）
        t->rotation.y += speedDegY * dt;
        
        // 360度を超えたら正規化（見やすさのため、なくてもOK）
        while (t->rotation.y >= 360.0f) t->rotation.y -= 360.0f;
        while (t->rotation.y < 0.0f) t->rotation.y += 360.0f;
    }
};
