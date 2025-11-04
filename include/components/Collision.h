/*****************************************************************/ /*
 * \file   Collision.h
 * \brief  当たり判定を取得するプログラム
 * 
 * \author 立山悠朔・上手凉太郎
 * \date   11/4
 *********************************************************************/
#pragma once
#include "components/Component.h"
#include "ecs/Entity.h"
#include "ecs/World.h"
#include "components/Transform.h"
#include <DirectXMath.h>

using namespace DirectX;

/**
 * @enum CollisionType
 * @brief 当たり判定の形のタイプ
 */
enum CollisionType {
    None,
    AABB,   //箱
    Circle, //円
    OBB,    //回転できる箱(回転し続けてるやつじゃなくて傾いてる)
};

/** 
 * @struct CollisionGet
 * @brief 当たり判定に必要なものをまとめた
 */
struct CollisionGet {
    CollisionType type; //初期化
    XMFLOAT3 center;    //判定を取るオブジェクトの中心座標
    XMFLOAT3 size;      //判定を取るオブジェクトのサイズ
    XMFLOAT3 rotation;  //判定を取るオブジェクトの角度
};

/** 
 * @struct Collision :Behavior
 * @brief形状組の詳細判定（AABB/AABB, AABB/Circle, OBB/*）
 */
struct Collision : Behaviour {
    CollisionGet data;
    void OnUpdate(World &w, Entity self, float dt) override {
        auto *t = w.TryGet<Transform>(self);
        if (!t)
            return; // Transformがなければ何もしない

        data.center = t->position;
        data.size = t->scale;
        data.rotation = t->rotation;
        data.type = CollisionType::AABB;
    }
};
