//////
//上手涼太郎
// 2025  11.11
// hp.h追加
//プレイヤーがダメージでHPを減らし、0でリセット
//
/////
#pragma once

#include "components/Component.h"


struct Hp : IComponent
{
    float currentHp = 3.0f;
    float maxHp = 3.0f;

    void TakeDamage()
    {
        float damage = 1.0f;
        currentHp -= damage;
        if (currentHp < 0.0f)
            currentHp = 0.0f;
    }
    void Reset()
    {
        currentHp = maxHp;
    }

};




