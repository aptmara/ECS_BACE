/**
 * @file TransformHierarchySamples.h
 * @brief Transform階層システムのサンプル実装
 * @author 山内陽
 * @date 2025
 * @version 6.0
 *
 * @details
 * TransformHierarchyとTransformHierarchySystemの使用例を示します。
 */
#pragma once

#include "ecs/World.h"
#include "components/Transform.h"
#include "components/TransformHierarchy.h"
#include "components/MeshRenderer.h"
#include "systems/TransformHierarchySystem.h"
#include <DirectXMath.h>

namespace TransformHierarchySamples {

/**
 * @brief ロボットの腕の階層構造サンプル
 * @param world Worldインスタンス
 * @return ルートエンティティ（胴体）
 */
inline Entity CreateRobotArmHierarchy(World &world) {
    Entity body = world.Create()
                      .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
                      .With<MeshRenderer>(DirectX::XMFLOAT3{0.5f, 0.5f, 0.5f})
                      .With<TransformHierarchy>()
                      .Build();

    Entity rightShoulder = world.Create()
                               .With<Transform>(DirectX::XMFLOAT3{1, 0, 0})
                               .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
                               .With<TransformHierarchy>()
                               .Build();

    Entity rightArm = world.Create()
                          .With<Transform>(DirectX::XMFLOAT3{0, -1, 0})
                          .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0.5f, 0.5f})
                          .With<TransformHierarchy>()
                          .Build();

    auto &bodyHierarchy = world.Get<TransformHierarchy>(body);
    auto &shoulderHierarchy = world.Get<TransformHierarchy>(rightShoulder);
    auto &armHierarchy = world.Get<TransformHierarchy>(rightArm);

    shoulderHierarchy.SetParent(body);
    bodyHierarchy.AddChild(rightShoulder);

    armHierarchy.SetParent(rightShoulder);
    shoulderHierarchy.AddChild(rightArm);

    return body;
}

/**
 * @brief 太陽系シミュレーションの階層構造サンプル
 * @param world Worldインスタンス
 * @return ルートエンティティ（太陽）
 */
inline Entity CreateSolarSystemHierarchy(World &world) {
    Entity sun = world.Create()
                     .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
                     .With<MeshRenderer>(DirectX::XMFLOAT3{1, 1, 0})
                     .With<TransformHierarchy>()
                     .Build();
    world.Get<Transform>(sun).scale = DirectX::XMFLOAT3{2, 2, 2};

    Entity earth = world.Create()
                       .With<Transform>(DirectX::XMFLOAT3{10, 0, 0})
                       .With<MeshRenderer>(DirectX::XMFLOAT3{0, 0, 1})
                       .With<TransformHierarchy>()
                       .Build();

    Entity moon = world.Create()
                      .With<Transform>(DirectX::XMFLOAT3{2, 0, 0})
                      .With<MeshRenderer>(DirectX::XMFLOAT3{0.7f, 0.7f, 0.7f})
                      .With<TransformHierarchy>()
                      .Build();
    world.Get<Transform>(moon).scale = DirectX::XMFLOAT3{0.5f, 0.5f, 0.5f};

    auto &sunHierarchy = world.Get<TransformHierarchy>(sun);
    auto &earthHierarchy = world.Get<TransformHierarchy>(earth);
    auto &moonHierarchy = world.Get<TransformHierarchy>(moon);

    earthHierarchy.SetParent(sun);
    sunHierarchy.AddChild(earth);

    moonHierarchy.SetParent(earth);
    earthHierarchy.AddChild(moon);

    return sun;
}

/**
 * @brief 車の階層構造サンプル
 * @param world Worldインスタンス
 * @return ルートエンティティ（車体）
 */
inline Entity CreateCarHierarchy(World &world) {
    Entity carBody = world.Create()
                         .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
                         .With<MeshRenderer>(DirectX::XMFLOAT3{0.8f, 0.2f, 0.2f})
                         .With<TransformHierarchy>()
                         .Build();
    world.Get<Transform>(carBody).scale = DirectX::XMFLOAT3{2, 1, 3};

    auto &bodyHierarchy = world.Get<TransformHierarchy>(carBody);

    DirectX::XMFLOAT3 wheelPositions[] = {
        {-0.8f, -0.5f, 1.0f},
        {0.8f, -0.5f, 1.0f},
        {-0.8f, -0.5f, -1.0f},
        {0.8f, -0.5f, -1.0f}};

    for (int i = 0; i < 4; ++i) {
        Entity wheel = world.Create()
                           .With<Transform>(wheelPositions[i])
                           .With<MeshRenderer>(DirectX::XMFLOAT3{0.2f, 0.2f, 0.2f})
                           .With<TransformHierarchy>()
                           .Build();
        world.Get<Transform>(wheel).scale = DirectX::XMFLOAT3{0.5f, 0.5f, 0.5f};

        auto &wheelHierarchy = world.Get<TransformHierarchy>(wheel);
        wheelHierarchy.SetParent(carBody);
        bodyHierarchy.AddChild(wheel);
    }

    return carBody;
}

/**
 * @brief 回転アニメーションBehaviour
 */
struct RotateAnimation : Behaviour {
    float speedY = 45.0f;

    void OnUpdate(World &w, Entity self, float dt) override {
        auto *transform = w.TryGet<Transform>(self);
        if (transform) {
            transform->rotation.y += speedY * dt;
            if (transform->rotation.y >= 360.0f) {
                transform->rotation.y -= 360.0f;
            }
        }
    }
};

/**
 * @brief 公転アニメーションBehaviour
 */
struct OrbitAnimation : Behaviour {
    float orbitSpeed = 30.0f;
    float radius = 10.0f;
    float angle = 0.0f;

    void OnUpdate(World &w, Entity self, float dt) override {
        auto *transform = w.TryGet<Transform>(self);
        if (transform) {
            angle += orbitSpeed * dt;
            if (angle >= 360.0f) {
                angle -= 360.0f;
            }

            float radians = DirectX::XMConvertToRadians(angle);
            transform->position.x = cosf(radians) * radius;
            transform->position.z = sinf(radians) * radius;
        }
    }
};

/**
 * @brief 階層情報をコンソールに出力
 * @param world Worldインスタンス
 * @param entity エンティティ
 * @param depth 階層の深さ（インデント用）
 */
inline void PrintHierarchy(World &world, Entity entity, int depth = 0) {
    auto *hierarchy = world.TryGet<TransformHierarchy>(entity);
    auto *transform = world.TryGet<Transform>(entity);

    if (!hierarchy || !transform) {
        return;
    }

    std::string indent(depth * 2, ' ');
    printf("%sEntity %u: pos=(%.1f, %.1f, %.1f), rot=(%.1f, %.1f, %.1f), scale=(%.1f, %.1f, %.1f)\n",
           indent.c_str(), entity.id,
           transform->position.x, transform->position.y, transform->position.z,
           transform->rotation.x, transform->rotation.y, transform->rotation.z,
           transform->scale.x, transform->scale.y, transform->scale.z);

    for (const Entity &child : hierarchy->GetChildren()) {
        PrintHierarchy(world, child, depth + 1);
    }
}

/**
 * @brief すべてのサンプルを実行
 * @param world Worldインスタンス
 */
inline void RunAllSamples(World &world) {
    Entity hierarchySystem = world.Create()
                                 .With<TransformHierarchySystem>()
                                 .Build();

    printf("\n=== Transform Hierarchy Samples ===\n\n");

    printf("--- Sample 1: Robot Arm ---\n");
    Entity robotArm = CreateRobotArmHierarchy(world);
    PrintHierarchy(world, robotArm);
    printf("\n");

    printf("--- Sample 2: Solar System ---\n");
    Entity solarSystem = CreateSolarSystemHierarchy(world);
    world.Add<RotateAnimation>(solarSystem);
    PrintHierarchy(world, solarSystem);
    printf("\n");

    printf("--- Sample 3: Car ---\n");
    Entity car = CreateCarHierarchy(world);
    PrintHierarchy(world, car);
    printf("\n");

    printf("=== All Samples Created ===\n\n");
}

/**
 * @brief インタラクティブなデモ
 * @param world Worldインスタンス
 */
inline void RunInteractiveDemo(World &world) {
    Entity hierarchySystem = world.Create()
                                 .With<TransformHierarchySystem>()
                                 .Build();

    Entity parent = world.Create()
                        .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
                        .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
                        .With<TransformHierarchy>()
                        .With<RotateAnimation>()
                        .Build();

    for (int i = 0; i < 3; ++i) {
        Entity child = world.Create()
                           .With<Transform>(DirectX::XMFLOAT3{static_cast<float>(i + 1) * 2.0f, 0, 0})
                           .With<MeshRenderer>(DirectX::XMFLOAT3{0, static_cast<float>(i + 1) / 3.0f, 1})
                           .With<TransformHierarchy>()
                           .Build();

        auto &childHierarchy = world.Get<TransformHierarchy>(child);
        auto &parentHierarchy = world.Get<TransformHierarchy>(parent);

        childHierarchy.SetParent(parent);
        parentHierarchy.AddChild(child);
    }

    printf("Interactive demo: Parent rotates, children follow\n");
    printf("Run world.Tick(dt) in your game loop to see the animation\n");
}

} // namespace TransformHierarchySamples
