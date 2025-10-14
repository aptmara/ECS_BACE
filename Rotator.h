#pragma once
#include "Component.h"
#include "Entity.h"
#include "World.h"
#include "Transform.h"
#include <DirectXMath.h>

// ========================================================
// Rotator - ©“®‰ñ“]‚ğs‚¤Behaviour
// ========================================================
struct Rotator : Behaviour {
    float speedDegY = 45.0f;

    Rotator() = default;
    explicit Rotator(float s) : speedDegY(s) {}

    void OnUpdate(World& w, Entity self, float dt) override {
        auto* t = w.TryGet<Transform>(self);
        if (!t) return;
        t->rotation.y += DirectX::XMConvertToRadians(speedDegY) * dt;
    }
};
