/**
 * @file Game.h
 * @brief UIを統合したゲームシーン
 * @author 山内陽
 * @date 2025
 * @version 1.0
 */
#pragma once

#include "pch.h"
#include "components/GameTags.h"
#include "components/PlayerComponents.h"
#include "components/MeshRenderer.h"
#include "components/Collision.h"
#include "components/UIComponents.h"
#include "input/InputSystem.h"
#include "input/GamepadSystem.h"
#include "components/Rotator.h"
#include "components/Light.h"
#include "systems/UISystem.h"
#include "graphics/TextSystem.h"
#include "graphics/GfxDevice.h"
#include "app/ServiceLocator.h"
#include "SenesUIController.h"
#include "components/GameStats.h"
#include <sstream>
#include <iomanip>

/**
 * @struct PlayerCollisionHandler
 * @brief プレイヤーの衝突イベントを処理
 */
struct PlayerCollisionHandler : ICollisionHandler {
    void OnCollisionEnter(World &w, Entity self, Entity other, const CollisionInfo &info) override {
        if (w.Has<EnemyTag>(other)) {
            DEBUGLOG("Player collision with enemy - penetration depth: " + std::to_string(info.penetrationDepth));

            w.ForEach<GameStats>([](Entity e, GameStats &stats) {
                stats.score += 10;
            });
        }
    }

    void OnCollisionStay(World &w, Entity self, Entity other, const CollisionInfo &info) override {
    }

    void OnCollisionExit(World &w, Entity self, Entity other) override {
    }
};

/**
 * @struct EnemyCollisionHandler
 * @brief 敵の衝突イベントを処理
 */
struct EnemyCollisionHandler : ICollisionHandler {
    void OnCollisionEnter(World &w, Entity self, Entity other, const CollisionInfo &info) override {
        if (w.Has<PlayerTag>(other)) {
            DEBUGLOG("Enemy collision with player");
        }
    }
};

/**
 * @struct WallCollisionHandler
 * @brief 壁の衝突イベントを処理
 */
struct WallCollisionHandler : ICollisionHandler {
    void OnCollisionEnter(World& w, Entity self, Entity other, const CollisionInfo& info) override {
        if (w.Has<WallTag>(other)) {
            DEBUGLOG("Wall collision with player");
        }
    }
};

/**
 * @class GameScene
 * @brief 3DゲームとUIを統合したシーン
 */
class GameScene : public IScene {
  public:
    void OnEnter(World &world) override {
        DEBUGLOG("GameWithUIScene::OnEnter()");

        auto *gfx = ServiceLocator::TryGet<GfxDevice>();
        if (!gfx) {
            DEBUGLOG_ERROR("GfxDevice not found");
            return;
        }

        if (!textSystem_.Init(*gfx)) {
            DEBUGLOG_ERROR("TextSystem initialization failed");
            return;
        }

        CreateTextFormats();

        float screenWidth = static_cast<float>(gfx->Width());
        float screenHeight = static_cast<float>(gfx->Height());

        Entity gameStats = world.Create()
                               .With<GameStats>()
                               .Build();
        ownedEntities_.push_back(gameStats);

        Entity collisionSystem = world.Create()
                                     .With<CollisionDetectionSystem>()
                                     .Build();
        ownedEntities_.push_back(collisionSystem);

#ifdef _DEBUG
        Entity debugRenderer = world.Create()
                                   .With<CollisionDebugRenderer>()
                                   .Build();
        ownedEntities_.push_back(debugRenderer);
#endif

        world.Create().With<DirectionalLight>();

        CreateFloor(world);
        CreatePlayer(world);
        CreateTestEnemy(world);
        CreateWall(world);

        CreateUI(world, screenWidth, screenHeight);

        DEBUGLOG("GameWithUIScene initialized successfully");
    }

    void OnUpdate(World &world, InputSystem &input, float deltaTime) override {
        world.ForEach<GameStats>([&](Entity e, GameStats &stats) {
            if (input.GetKeyDown(VK_ESCAPE) || input.GetKeyDown('P')) {
                stats.isPaused = !stats.isPaused;
                DEBUGLOG(stats.isPaused ? "Game paused" : "Game resumed");
            }

            if (stats.isPaused) {
                deltaTime = 0.0f;
            }
        });

        world.ForEach<PlayerMovement>([&](Entity e, PlayerMovement &pm) {
            if (!pm.input_) {
                pm.input_ = &input;
            }
            if (!pm.gamepad_) {
                pm.gamepad_ = &ServiceLocator::Get<GamepadSystem>();
            }
        });

        world.ForEach<UIInteractionSystem>([&](Entity e, UIInteractionSystem &sys) {
            if (!sys.input_) {
                sys.input_ = &input;
            }
        });

        world.Tick(deltaTime);
    }

    void OnExit(World &world) override {
        DEBUGLOG("GameWithUIScene::OnExit()");

        for (const auto &entity : ownedEntities_) {
            if (world.IsAlive(entity)) {
                world.DestroyEntityWithCause(entity, World::Cause::SceneUnload);
            }
        }
        ownedEntities_.clear();

        textSystem_.Shutdown();
        DEBUGLOG("GameWithUIScene cleanup completed");
    }

  private:
    void CreateTextFormats() {
        TextSystem::TextFormat hudFormat;
        hudFormat.fontSize = 24.0f;
        hudFormat.fontFamily = L"メイリオ";
        hudFormat.alignment = DWRITE_TEXT_ALIGNMENT_LEADING;
        textSystem_.CreateTextFormat("hud", hudFormat);

        TextSystem::TextFormat pauseFormat;
        pauseFormat.fontSize = 72.0f;
        pauseFormat.fontFamily = L"メイリオ";
        pauseFormat.alignment = DWRITE_TEXT_ALIGNMENT_CENTER;
        pauseFormat.paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
        textSystem_.CreateTextFormat("pause", pauseFormat);

        TextSystem::TextFormat buttonFormat;
        buttonFormat.fontSize = 20.0f;
        buttonFormat.alignment = DWRITE_TEXT_ALIGNMENT_CENTER;
        buttonFormat.paragraphAlignment = DWRITE_PARAGRAPH_ALIGNMENT_CENTER;
        textSystem_.CreateTextFormat("button", buttonFormat);

        TextSystem::TextFormat panelFormat;
        panelFormat.fontSize = 200.0f;
        textSystem_.CreateTextFormat("panel", panelFormat);
    }

    void CreateUI(World &world, float screenWidth, float screenHeight) {
        Entity canvas = world.Create()
                            .With<UICanvas>()
                            .Build();
        ownedEntities_.push_back(canvas);

        Entity uiRenderSystem = world.Create()
                                    .With<UIRenderSystem>()
                                    .Build();
        auto *renderSys = world.TryGet<UIRenderSystem>(uiRenderSystem);
        if (renderSys) {
            renderSys->SetTextSystem(&textSystem_);
            renderSys->SetScreenSize(screenWidth, screenHeight);
        }
        ownedEntities_.push_back(uiRenderSystem);

        Entity uiInteractionSystem = world.Create()
                                         .With<UIInteractionSystem>()
                                         .Build();
        auto *interactionSys = world.TryGet<UIInteractionSystem>(uiInteractionSystem);
        if (interactionSys) {
            interactionSys->SetScreenSize(screenWidth, screenHeight);
        }
        ownedEntities_.push_back(uiInteractionSystem);

        UITransform scoreTransform;
        scoreTransform.position = {20.0f, 20.0f};
        scoreTransform.size = {300.0f, 40.0f};
        scoreTransform.anchor = {0.0f, 0.0f};
        scoreTransform.pivot = {0.0f, 0.0f};

        UIText scoreText{L"Score: 0"};
        scoreText.color = {1.0f, 1.0f, 0.0f, 1.0f};
        scoreText.formatId = "hud";

        Entity scoreEntity = world.Create()
                                 .With<UITransform>(scoreTransform)
                                 .With<UIText>(scoreText)
                                 .Build();
        ownedEntities_.push_back(scoreEntity);

        UITransform timeTransform;
        timeTransform.position = {20.0f, 70.0f};
        timeTransform.size = {300.0f, 40.0f};
        timeTransform.anchor = {0.0f, 0.0f};
        timeTransform.pivot = {0.0f, 0.0f};

        UIText timeText{L"Time: 00:00"};
        timeText.color = {1.0f, 1.0f, 1.0f, 1.0f};
        timeText.formatId = "hud";

        Entity timeEntity = world.Create()
                                .With<UITransform>(timeTransform)
                                .With<UIText>(timeText)
                                .Build();
        ownedEntities_.push_back(timeEntity);

        UITransform fpsTransform;
        fpsTransform.position = {-20.0f, 20.0f};
        fpsTransform.size = {200.0f, 40.0f};
        fpsTransform.anchor = {1.0f, 0.0f};
        fpsTransform.pivot = {1.0f, 0.0f};

        UIText fpsText{L"FPS: 0.0"};
        fpsText.color = {0.0f, 1.0f, 0.0f, 1.0f};
        fpsText.formatId = "hud";

        Entity fpsEntity = world.Create()
                               .With<UITransform>(fpsTransform)
                               .With<UIText>(fpsText)
                               .Build();
        ownedEntities_.push_back(fpsEntity);

        UITransform pauseTransform;
        pauseTransform.position = {0.0f, 0.0f};
        pauseTransform.size = {0.0f, 0.0f};
        pauseTransform.anchor = {0.5f, 0.5f};
        pauseTransform.pivot = {0.5f, 0.5f};

        UIText pauseText{L""};
        pauseText.color = {1.0f, 0.0f, 0.0f, 1.0f};
        pauseText.formatId = "pause";

        Entity pauseEntity = world.Create()
                                 .With<UITransform>(pauseTransform)
                                 .With<UIText>(pauseText)
                                 .Build();
        ownedEntities_.push_back(pauseEntity);

        Entity uiUpdater = world.Create()
                               .With<GameUIUpdater>()
                               .Build();
        auto *updater = world.TryGet<GameUIUpdater>(uiUpdater);
        if (updater) {
            updater->scoreTextEntity_ = scoreEntity;
            updater->timeTextEntity_ = timeEntity;
            updater->fpsTextEntity_ = fpsEntity;
            updater->pauseTextEntity_ = pauseEntity;
        }
        ownedEntities_.push_back(uiUpdater);
    }

    void CreateFloor(World &world) {
        Transform transform{
            {0.0f, -2.0f, 0.0f},
            {0.0f, 0.0f, 0.0f},
            {20.0f, 0.2f, 20.0f},
        };

        MeshRenderer renderer;
        renderer.meshType = MeshType::Plane;
        renderer.color = DirectX::XMFLOAT3{0.5f, 0.5f, 0.5f};

        Entity floor = world.Create()
                           .With<Transform>(transform)
                           .With<MeshRenderer>(renderer)
                           .With<CollisionBox>(DirectX::XMFLOAT3{20.0f, 0.2f, 20.0f})
                           .Build();

        ownedEntities_.push_back(floor);
    }

    void CreatePlayer(World &world) {
        Transform transform{
            {0.0f, 0.0f, 5.0f},
            {0.0f, 0.0f, 0.0f},
            {1.0f, 1.0f, 1.0f},
        };

        MeshRenderer renderer;
        renderer.meshType = MeshType::Cube;
        renderer.color = DirectX::XMFLOAT3{0.0f, 1.0f, 0.0f};

        Entity player = world.Create()
                            .With<Transform>(transform)
                            .With<MeshRenderer>(renderer)
                            .With<PlayerTag>()
                            .With<PlayerMovement>()
                            .With<Rotator>(45.0f)
                            .With<CollisionBox>(DirectX::XMFLOAT3{1.0f, 2.0f, 1.0f})
                            .With<PlayerCollisionHandler>()
                            .Build();

        ownedEntities_.push_back(player);
    }

    void CreateWall(World& world)
    {
        Transform transform{
            {3.0f, 0.0f, 3.0f},
            {0.0f, 0.0f, 0.0f},
            {1.0f, 1.0f, 1.0f},
        };

        MeshRenderer renderer;
        renderer.meshType = MeshType::Cube;
        renderer.color = DirectX::XMFLOAT3{1.0f,1.0f,1.0f};

        Entity wall = world.Create()
                          .With<Transform>(transform)
                          .With<MeshRenderer>(renderer)
                          .With<WallTag>()
                          .With<CollisionBox>(DirectX::XMFLOAT3{1.0f,1.0f,1.0f})
                          .With<WallCollisionHandler>()
                          .Build();
    }

    void CreateTestEnemy(World &world) {
        Transform transform{
            {1.5f, 0.0f, 5.0f},
            {0.0f, 0.0f, 0.0f},
            {1.0f, 1.0f, 1.0f},
        };

        MeshRenderer renderer;
        renderer.meshType = MeshType::Sphere;
        renderer.color = DirectX::XMFLOAT3{1.0f, 0.0f, 0.0f};

        Entity enemy = world.Create()
                           .With<Transform>(transform)
                           .With<MeshRenderer>(renderer)
                           .With<EnemyTag>()
                           .With<CollisionSphere>(0.5f)
                           .With<EnemyCollisionHandler>()
                           .Build();

        ownedEntities_.push_back(enemy);
    }

    TextSystem textSystem_;
    std::vector<Entity> ownedEntities_;
};
