/**
 * @file SceneManager.h
 * @brief シーン管理システム
 * @author 山内陽
 * @date 2025
 * @version 4.0
 *
 * @details
 * ### 役割: ゲームの画面(シーン)を切り替える
 * ### 例: タイトル画面 → ゲーム画面 → リザルト画面
 *
 * シーン管理の流れ:
 * 1. ISceneを継承してシーンを作成
 * 2. SceneManagerに登録
 * 3. ChangeScene()で切り替え
 */
#pragma once

#include "ecs/World.h"
#include "input/InputSystem.h"
#include <unordered_map>
#include <string>

// ========================================================
// IScene - シーンの基底クラス
// ========================================================

/**
 * @class IScene
 * @brief すべてのシーンの基底クラス
 *
 * @details
 * ゲームの各画面(シーン)はこのクラスを継承して作成します。
 *
 * ### 実装が必要なメソッド:
 * - OnEnter(): シーンが始まるときに1度だけ呼ばれる
 * - OnUpdate(): 毎フレーム呼ばれる(ゲームロジック)
 * - OnExit(): シーンが終わるときに1度だけ呼ばれる
 *
 * ### オプションのメソッド:
 * - ShouldChangeScene(): 次のシーンへ移行するか判定
 * - GetNextScene(): 次のシーン名を返す
 *
 * @par 使用例
 * @code
 * class TitleScene : public IScene {
 * public:
 *     void OnEnter(World& world) override {
 *         // タイトル画面の初期化
 *     }
 *
 *     void OnUpdate(World& world, InputSystem& input, float dt) override {
 *         // スペースキーでゲーム開始
 *         if (input.GetKeyDown(VK_SPACE)) {
 *             changeScene_ = true;
 *         }
 *     }
 *
 *     void OnExit(World& world) override {
 *         // クリーンアップ
 *     }
 *
 *     bool ShouldChangeScene() const override { return changeScene_; }
 *     const char* GetNextScene() const override { return "GameScene"; }
 *
 * private:
 *     bool changeScene_ = false;
 * };
 * @endcode
 *
 * @author 山内陽
 */
class IScene {
public:
    /**
     * @brief 仮想デストラクタ
     * @details 派生クラスで正しくデストラクタが呼ばれることを保証します
     */
    virtual ~IScene() = default;

    /**
     * @brief シーン開始時の初期化処理
     *
     * @param[in,out] world ワールド参照
     *
     * @details
     * シーンが開始されるときに1度だけ呼ばれます。
     * エンティティの生成、変数の初期化などを行います。
     *
     * @par 使用例
     * @code
     * void OnEnter(World& world) override {
     *     // プレイヤーを生成
     *     player_ = world.Create()
     *         .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
     *         .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
     *         .Build();
     * }
     * @endcode
     */
    virtual void OnEnter(World& world) = 0;

    /**
     * @brief 毎フレームの更新処理
     *
     * @param[in,out] world ワールド参照(コンポーネント取得などに使用)
     * @param[in] input 入力システム参照
     * @param[in] deltaTime デルタタイム(秒単位)
     *
     * @details
     * 毎フレーム呼ばれます。
     * 入力処理、移動、衝突判定などのゲームロジックを実装します。
     *
     * @par 使用例
     * @code
     * void OnUpdate(World& world, InputSystem& input, float dt) override {
     *     // 入力処理
     *     if (input.GetKey('W')) {
     *         // プレイヤーを移動
     *     }
     *
     *     // ゲームロジック更新
     *     world.Tick(dt);
     * }
     * @endcode
     */
    virtual void OnUpdate(World& world, InputSystem& input, float deltaTime) = 0;

    /**
     * @brief シーン終了時のクリーンアップ処理
     *
     * @param[in,out] world ワールド参照
     *
     * @details
     * シーンが終了するときに1度だけ呼ばれます。
     * エンティティの削除、リソースの解放などを行います。
     *
     * @par 使用例
     * @code
     * void OnExit(World& world) override {
     *     // すべてのエンティティを削除
     *     world.DestroyEntity(player_);
     *     world.DestroyEntity(enemy_);
     * }
     * @endcode
     */
    virtual void OnExit(World& world) = 0;

    /**
     * @brief 次のシーンへ移行するか判定
     *
     * @return true 移行する, false 移行しない
     *
     * @details
     * SceneManagerが毎フレーム呼び出しtrueならシーンを切り替えます。
     *
     * @par 使用例
     * @code
     * bool ShouldChangeScene() const override {
     *     return gameOver_;  // ゲームオーバーなら移行
     * }
     * @endcode
     */
    virtual bool ShouldChangeScene() const { return false; }

    /**
     * @brief 次のシーン名を取得
     *
     * @return const char* 次のシーン名
     *
     * @details
     * ShouldChangeScene()がtrueのときに呼ばれます。
     * RegisterScene()で登録した名前を返してください。
     *
     * @par 使用例
     * @code
     * const char* GetNextScene() const override {
     *     return "ResultScene";
     * }
     * @endcode
     */
    virtual const char* GetNextScene() const { return nullptr; }
};

// ========================================================
// SceneManager - シーン切り替え管理
// ========================================================

/**
 * @class SceneManager
 * @brief ゲームシーンの切り替えを管理するクラス
 *
 * @details
 * 複数のシーンを登録し、動的で切り替えを行います。
 *
 * ### 基本的な使い方:
 * 1. シーンを作成
 * 2. RegisterScene()で登録
 * 3. Init()で開始シーンを設定
 * 4. 毎フレームUpdate()を呼ぶ
 * 5. 必要に応じてChangeScene()で切り替え
 *
 * @par 使用例
 * @code
 * SceneManager sceneManager;
 * World world;
 *
 * // シーンを登録
 * sceneManager.RegisterScene("Title", new TitleScene());
 * sceneManager.RegisterScene("Game", new GameScene());
 * sceneManager.RegisterScene("Result", new ResultScene());
 *
 * // タイトルシーンから開始
 * sceneManager.Init(sceneManager.GetScene("Title"), world);
 *
 * // ゲームループ
 * while (running) {
 *     sceneManager.Update(world, input, deltaTime);
 * }
 * @endcode
 *
 * @author 山内陽
 */
class SceneManager {
public:
    /**
     * @brief 初期化(最初のシーンを設定)
     *
     * @param[in] startScene 開始シーン
     * @param[in,out] world ワールド参照
     *
     * @details
     * SceneManagerを初期化し最初のシーンを開始します。
     * startSceneのOnEnter()が自動的に呼ばれます。
     */
    void Init(IScene* startScene, World& world) {
        // シャットダウン後の再初期化に備えてフラグを戻す
        isShutdown_ = false;
        currentScene_ = startScene;
        if (currentScene_) {
            currentScene_->OnEnter(world);
        }
    }

    /**
     * @brief シーンを登録(動的でシーンを切り替えられるようにする)
     *
     * @param[in] name シーンの名前
     * @param[in] scene シーンのポインタ
     *
     * @details
     * シーンを動的で登録します。
     * ChangeScene()で名前を指定してシーンを切り替えられます。
     *
     * @par 使用例
     * @code
     * sceneManager.RegisterScene("Title", new TitleScene());
     * sceneManager.RegisterScene("Game", new GameScene());
     * @endcode
     */
    void RegisterScene(const char* name, IScene* scene) {
        scenes_[name] = scene;
    }

    /**
     * @brief 毎フレームの更新
     *
     * @param[in,out] world ワールド参照
     * @param[in] input 入力システム参照
     * @param[in] deltaTime デルタタイム(秒単位)
     *
     * @details
     * 現在のシーンを更新し、シーン移行をチェックします。
     * ShouldChangeScene()がtrueなら自動的にシーンを切り替えます。
     */
    void Update(World& world, InputSystem& input, float deltaTime) {
        if (!currentScene_) return;

        // 現在のシーンを更新
        currentScene_->OnUpdate(world, input, deltaTime);

        // シーン移行チェック
        if (currentScene_->ShouldChangeScene()) {
            const char* nextSceneName = currentScene_->GetNextScene();
            ChangeScene(nextSceneName, world);
        }
    }

    /**
     * @brief シーンを切り替え
     *
     * @param[in] sceneName 次のシーン名
     * @param[in,out] world ワールド参照
     *
     * @details
     * 現在のシーンを終了し、新しいシーンを開始します。
     *
     * ### 処理の流れ:
     * 1. 現在のシーンのOnExit()を呼ぶ
     * 2. 新しいシーンのOnEnter()を呼ぶ
     *
     * @par 使用例
     * @code
     * // ゲームシーンに切り替え
     * sceneManager.ChangeScene("Game", world);
     * @endcode
     */
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

    /**
     * @brief 現在のシーンを取得
     * @return IScene* 現在のシーン（nullptrの可能性あり）
     */
    IScene* GetCurrentScene() const {
        return currentScene_;
    }

    /**
     * @brief デストラクタ
     * @details 登録されたすべてのシーンを削除します
     */
    ~SceneManager() {
        DEBUGLOG("SceneManager::~SceneManager() - クリーンアップ中");

        if (!isShutdown_) {
            DEBUGLOG_WARNING("SceneManager::Shutdown()が明示的に呼ばれていません。デストラクタで自動クリーンアップします。");
        }

        if (isShutdown_) {
            DEBUGLOG("SceneManager破棄完了");
            return;
        }

        // 現在のシーンが残っている場合は警告
        if (currentScene_) {
            DEBUGLOG_WARNING("破棄時にシーンがまだアクティブです - App::Shutdown()でクリーンアップされるべきでした");
        }

        // 各シーンを削除
        DEBUGLOG(std::to_string(scenes_.size()) + " 個のシーンを削除中");
        for (auto& pair : scenes_) {
            if (pair.second) {
                DEBUGLOG("シーン削除: " + pair.first);
                delete pair.second;
                pair.second = nullptr;
            }
        }
        scenes_.clear();

        DEBUGLOG("SceneManager破棄完了");
    }

    /**
     * @brief 明示的なシャットダウン
     * @details デストラクタ前にリソースを解放したい場合に使用
     */
    void Shutdown(World& world) {
        if (isShutdown_) {
            return; // 冪等性の確保
        }
        DEBUGLOG("SceneManager::Shutdown() 呼び出し");

        // 現在のシーンを終了
        if (currentScene_) {
            DEBUGLOG("現在のシーンを終了中");
            currentScene_->OnExit(world);
            currentScene_ = nullptr;
        }

        // 登録済みシーンをすべて削除
        if (!scenes_.empty()) {
            DEBUGLOG(std::to_string(scenes_.size()) + " 個のシーンを削除中");
            for (auto& pair : scenes_) {
                if (pair.second) {
                    DEBUGLOG("シーン削除: " + pair.first);
                    delete pair.second;
                    pair.second = nullptr;
                }
            }
            scenes_.clear();
        }

        isShutdown_ = true;
        DEBUGLOG("SceneManager::Shutdown() 完了");
    }

private:
    IScene* currentScene_ = nullptr;                           ///< 現在実行中のシーン
    std::unordered_map<std::string, IScene*> scenes_;          ///< 登録されたシーン一覧
    bool isShutdown_ = false;                                   ///< シャットダウン済みフラグ
};
