#pragma once
#include "ecs/World.h"
#include "components/Model.h"
#include "components/ModelComponent.h"
#include "app/ServiceLocator.h"
#include "app/ResourceManager.h"

/**
 * @file ModelLoadingSystem.h
 * @brief Modelコンポーネントを持つエンティティのモデルをロードするシステム
 * @author 山内陽
 * @date 2025
 * @version 6.0
 */

struct ModelLoadingSystem : public Behaviour {
    void OnUpdate(World& world, Entity self, float dt) override {
        auto& resMgr = ServiceLocator::Get<ResourceManager>();

        // Modelコンポーネントを持つが、ModelComponentを持たないエンティティを検索
        world.ForEach<Model>([&](Entity e, Model& model) {
            if (!world.Has<ModelComponent>(e)) {
                // モデルをロード（またはキャッシュから取得）
                std::vector<ModelComponent> loadedComponents = resMgr.GetModel(model.filePath);

                if (!loadedComponents.empty()) {
                    // 最初のメッシュをこのエンティティに追加
                    world.Add<ModelComponent>(e, std::move(loadedComponents[0]));

                    // (オプション) 複数のメッシュがある場合、子エンティティとして追加することも可能
                    for (size_t i = 1; i < loadedComponents.size(); ++i) {
                        Entity child = world.Create()
                            .With<Transform>() // 子エンティティのTransformは親からの相対位置に設定する必要がある
                            .With<ModelComponent>(std::move(loadedComponents[i]));
                        // TODO: 親子関係をTransformに実装する
                    }
                }
                 else {
                    // ロード失敗時の処理（例えば、エラーを示すためにModelコンポーネントを削除）
                    world.Remove<Model>(e);
                }
            }
        });
    }
};
