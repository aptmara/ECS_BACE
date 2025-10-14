#pragma once
// ========================================================
// SceneManager.h - シーン管理システム
// ========================================================
// 【役割】ゲームの画面（シーン）を切り替える
// 【例】タイトル画面 → ゲーム画面 → リザルト画面
// ========================================================

#include "World.h"
#include "InputSystem.h"
#include <unordered_map>
#include <string>

// ========================================================
// IScene - シーンの基底クラス
// ========================================================
// 【使い方】
// 各シーンはこのクラスを継承して、以下を実装する：
// - OnEnter(): シーンが始まるときに1回だけ呼ばれる
// - OnUpdate(): 毎フレーム呼ばれる（ゲームロジック）
// - OnExit(): シーンが終わるときに1回だけ呼ばれる
// ========================================================
class IScene {
public:
    virtual ~IScene() = default;

    // シーン開始時の処理（エンティティの生成など）
    virtual void OnEnter(World& world) = 0;

    // 毎フレームの更新処理（入力、移動、衝突判定など）
    virtual void OnUpdate(World& world, InputSystem& input, float deltaTime) = 0;

    // シーン終了時の処理（クリーンアップ）
    virtual void OnExit(World& world) = 0;

    // 次のシーンへ遷移するか？
    virtual bool ShouldChangeScene() const { return false; }

    // 次のシーン名を返す
    virtual const char* GetNextScene() const { return nullptr; }
};

// ========================================================
// SceneManager - シーン切り替え管理
// ========================================================
class SceneManager {
public:
    // 初期化（最初のシーンを設定）
    void Init(IScene* startScene, World& world) {
        currentScene_ = startScene;
        if (currentScene_) {
            currentScene_->OnEnter(world);
        }
    }

    // シーンを登録（名前でシーンを切り替えられるようにする）
    void RegisterScene(const char* name, IScene* scene) {
        scenes_[name] = scene;
    }

    // 毎フレームの更新
    void Update(World& world, InputSystem& input, float deltaTime) {
        if (!currentScene_) return;

        // 現在のシーンを更新
        currentScene_->OnUpdate(world, input, deltaTime);

        // シーン遷移チェック
        if (currentScene_->ShouldChangeScene()) {
            const char* nextSceneName = currentScene_->GetNextScene();
            ChangeScene(nextSceneName, world);
        }
    }

    // シーンを切り替え
    void ChangeScene(const char* sceneName, World& world) {
        if (!sceneName) return;

        auto it = scenes_.find(sceneName);
        if (it == scenes_.end()) return;

        // 現在のシーンを終了
        if (currentScene_) {
            currentScene_->OnExit(world);
        }

        // 新しいシーンを開始
        currentScene_ = it->second;
        if (currentScene_) {
            currentScene_->OnEnter(world);
        }
    }

    ~SceneManager() {
        // 各シーンを削除
        for (auto& pair : scenes_) {
            delete pair.second;
        }
    }

private:
    IScene* currentScene_ = nullptr;
    std::unordered_map<std::string, IScene*> scenes_;
};
