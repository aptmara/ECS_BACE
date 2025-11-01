/**
 * @file ModelLoadingSystem.h
 * @brief Loads Model components into renderable data.
 */
#pragma once

#include "ecs/World.h"
#include "components/Model.h"
#include "components/Component.h"
#include "components/ModelComponent.h"
#include "components/Transform.h"
#include "app/ServiceLocator.h"
#include "app/ResourceManager.h"

struct ModelLoadingSystem : public Behaviour {
    void OnUpdate(World& world, Entity self, float dt) override {
        auto& resMgr = ServiceLocator::Get<ResourceManager>();

        world.ForEach<Model>([&](Entity entity, Model& model) {
            if (world.Has<ModelComponent>(entity)) {
                return;
            }

            const auto& components = resMgr.GetModel(model.filePath);
            if (components.empty()) {
                world.Remove<Model>(entity);
                return;
            }

            world.Add<ModelComponent>(entity, components[0]);

            for (size_t i = 1; i < components.size(); ++i) {
                Entity child = world.Create()
                    .With<Transform>()
                    .With<ModelComponent>(components[i])
                    .Build();
                // TODO: Relate child transform to parent.
                static_cast<void>(child);
            }
        });
    }
};

