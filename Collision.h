#pragma once
#include "components/Component.h"
#include "ecs/Entity.h"
#include "ecs/World.h"
#include "components/Transform.h"
#include <DirectXMath.h>

using namespace DirectX;

enum CollisionType {
    None,
    AABB,   //” 
    Circle, //‰~
    OBB,    //‰ñ“]‚Å‚«‚é” (‰ñ“]‚µ‘±‚¯‚Ä‚é‚â‚Â‚¶‚á‚È‚­‚ÄŒX‚¢‚Ä‚é)
};

struct Collision {
    CollisionType type; 
    XMFLOAT3 center;   
    XMFLOAT3 size;      
    float radius;  
};

struct Collision :Behaviour
{
    void OnUpdate(World &w, Entity self, float dt) override {
        auto *t = w.TryGet<Transform>(self);
        if (!t)
            return; // Transform‚ª‚È‚¯‚ê‚Î‰½‚à‚µ‚È‚¢

        CollisionType Type = CollisionType::None;

        XMFLOAT3 center{t->position.x, t->position.y, t->position.z};
        XMFLOAT3 size{t->scale.x,t->scale.y,t->scale.z};
        XMFLOAT3 rotation{t->rotation.x, t->rotation.y, t->rotation.z};

      /*  static bool Chack_AABB_AABB(Collision &a)
        {

        };*/
    }
   
};
