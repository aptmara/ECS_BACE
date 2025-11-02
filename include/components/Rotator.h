#pragma once
#include "components/Component.h"
#include "ecs/Entity.h"
#include "ecs/World.h"
#include "components/Transform.h"
#include <DirectXMath.h>

struct Rotator : Behaviour {
    float speedDegY = 45.0f;

    /**
     * @brief デフォルトコンストラクタ
     * @details 回転速度を45.0度/秒に設定します。
     */
    Rotator() = default;

    Rotator(float s) : speedDegY(s) {}

    void OnUpdate(World& w, Entity self, float dt) override {
        // このエンティティのTransformを取得
        auto* t = w.TryGet<Transform>(self);
        if (!t) return; // Transformがなければ何もしない

        // 回転値を更新(dt = デルタタイム = 前フレームからの経過時間)
        t->rotation.y += speedDegY * dt;

        // 360度を超えたら正規化(見やすくするため、なくてもOK)
        while (t->rotation.y >= 360.0f) t->rotation.y -= 360.0f;
        while (t->rotation.y < 0.0f) t->rotation.y += 360.0f;
    }
};
