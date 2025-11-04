/*****************************************************************//*
 * \file   MouseCoordinate.h
 * \brief  マウスの座標を毎フレーム取得するシステム
 * 
 * \author 飯島英菜・亀多彩日
 * \date   10/28
 *********************************************************************/
#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <cstdint>
#include <string>
#include "ecs/World.h"
#include"components/Component.h"
#include"InputSystem.h"

#include<dwrite.h>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")


/**
 * @struct MouseCoordinate
 * @brief マウス座標
 */
struct  MouseCoordinate :IComponent
{
    int MouseX = 0;         
    int MouseY = 0;
    int MouseDeltaX = 0;
    int MouseDeltaY = 0;

};


/**
 * @struct MouseCoordinate
 * @brief 毎フレームマウス座標を表示するBehaviour.
 * 
 * 
 */
struct MouseCoordinate :Behaviour
{
    void OnUpdate(World& w, Entity self, float dt) override
    {
        int MouseX = GetInput().GetMouseX(); //マウスX座標の座標取得
        int MouseY = GetInput().GetMouseY(); //マウスY座標の座標取得

        int MouseDeltaX = GetInput().GetMouseDeltaX();
        int MouseDeltaY = GetInput().GetMouseDeltaY();




    }

};




