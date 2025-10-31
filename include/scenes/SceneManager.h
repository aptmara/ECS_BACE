/**
 * @file SceneManager.h
 * @brief シーン管理システム
 * @author 山内陽
 * @date 2025
 * @version 4.0
 *
 * ゲームシーンの切り替えを管理するシステムです。
 * タイトル画面 → ゲーム画面 → リザルト画面など、複数のシーンを動的に切り替えることができます。
 *
 * シーン管理の流れ：
 * -# ISceneを継承してシーンを作成
 * -# SceneManagerに登録
 * -# ChangeScene()で切り替え
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
 * ゲームの各画面(シーン)はこのクラスを継承して作成します。
 * シーンのライフサイクル（開始、更新、終了）を管理します。
 *
 * 実装が必要な仮想メソッド：
 * - OnEnter() : シーン開始時に1度だけ呼ばれる
 * - OnUpdate() : 毎フレーム呼ばれる（ゲームロジック）
 * - OnExit() : シーン終了時に1度だけ呼ばれる
 *
 * オプションのメソッド：
 * - ShouldChangeScene() : 次のシーンへ移行するかどうかを判定
 * - GetNextScene() : 次のシーン名を返す
 *
 * ShouldChangeScene()がtrueを返すと、自動的にGetNextScene()で指定されたシーンに遷移します。
 *
 * @author 山内陽
 * @see SceneManager
 */
class IScene {
public:
    /**
     * @brief 仮想デストラクタ
     */
    virtual ~IScene() = default;

    /**
     * @brief シーン開始時の初期化処理
     * @param[in,out] world ワールド参照
     *
     * シーンが開始されるときに1度だけ呼ばれます。
     * エンティティの生成や変数の初期化などを実装します。
     *
     * @see OnUpdate(), OnExit()
     */
    virtual void OnEnter(World& world) = 0;

    /**
     * @brief 毎フレームの更新処理
     * @param[in,out] world ワールド参照（コンポーネント取得などに使用）
     * @param[in] input 入力システム参照
     * @param[in] deltaTime デルタタイム（秒単位）
     */
    virtual void OnUpdate(World& world, InputSystem& input, float deltaTime) = 0;

    /**
     * @brief シーン終了時のクリーンアップ処理
     * @param[in,out] world ワールド参照
     *
     * シーンが終了するときに1度だけ呼ばれます。
     * エンティティの削除やリソースの解放などを実装します。
     *
     * @see OnEnter(), OnUpdate()
     */
    virtual void OnExit(World& world) = 0;

    /**
     * @brief 次のシーンへ移行するかどうかを判定
     * @return true: 移行する、false: 移行しない
     *
     * SceneManagerが毎フレーム呼び出し、trueを返すとシーンを切り替えます。
     *
     * @see GetNextScene()
     */
    virtual bool ShouldChangeScene() const { return false; }

    /**
     * @brief 次のシーン名を取得
     * @return const char* 次のシーン名
     *
     * ShouldChangeScene()がtrueのときに呼ばれます。
     * RegisterScene()で登録した名前を返してください。
     *
     * @see ShouldChangeScene()
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
 * 複数のシーンを登録し、動的にシーンを切り替えます。
 * シーンのライフサイクル管理とシーン間の遷移を担当します。
 *
 * 基本的な使い方：
 * -# シーンを作成（ISceneを継承）
 * -# RegisterScene()で登録
 * -# Init()で開始シーンを設定
 * -# 毎フレームUpdate()を呼ぶ
 * -# 必要に応じてChangeScene()で切り替え
 *
 * @author 山内陽
 * @see IScene
 */
class SceneManager {
public:
    /**
     * @brief SceneManagerの初期化（最初のシーンを設定）
     * @param[in] startScene 開始シーンのポインタ
     * @param[in,out] world ワールド参照
     *
     * SceneManagerを初期化し、最初のシーンを開始します。
     * 指定されたシーンのOnEnter()が自動的に呼ばれます。
     *
     * @see RegisterScene(), Shutdown()
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
     * @brief シーンを登録
     * @param[in] name シーンの名前
     * @param[in] scene シーンのポインタ
     *
     * シーンを登録します。RegisterされたシーンはChangeScene()で名前を指定して遷移できます。
     *
     * @see ChangeScene()
     */
    void RegisterScene(const char* name, IScene* scene) {
        scenes_[name] = scene;
    }

    /**
     * @brief 毎フレームの更新
     * @param[in,out] world ワールド参照
     * @param[in] input 入力システム参照
     * @param[in] deltaTime デルタタイム（秒単位）
     *
     * 現在のシーンを更新し、シーン移行をチェックします。
     * 現在のシーンのOnUpdate()を呼び、ShouldChangeScene()がtrueなら自動的にシーンを切り替えます。
     *
     * @see ChangeScene(), IScene::OnUpdate()
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
     * @param[in] sceneName 次のシーン名
     * @param[in,out] world ワールド参照
     *
     * 現在のシーンを終了し、新しいシーンを開始します。
     *
     * 処理の流れ：
     * -# 現在のシーンのOnExit()を呼ぶ
     * -# world.FlushDestroyEndOfFrame()でエンティティを破棄
     * -# 新しいシーンのOnEnter()を呼ぶ
     *
     * sceneName が nullptr またはRegisteredされていない場合は何も行いません。
     *
     * @see Update(), IScene::OnExit(), IScene::OnEnter()
     */
    void ChangeScene(const char* sceneName, World& world) {
        if (!sceneName) return;

        auto it = scenes_.find(sceneName);
        if (it == scenes_.end()) return;

        // 現在のシーンを終了
        if (currentScene_) {
            DEBUGLOG_CATEGORY(DebugLog::Category::Scene, "シーン切り替え: OnExit()を呼び出し");
            currentScene_->OnExit(world);
            
            // シーン切り替え時のエンティティ破棄原因を明示
            world.FlushDestroyEndOfFrame();
        }

        // 新しいシーンを開始
        currentScene_ = it->second;
        if (currentScene_) {
            DEBUGLOG_CATEGORY(DebugLog::Category::Scene, "新しいシーンを開始: OnEnter()を呼び出し");
            currentScene_->OnEnter(world);
        }
    }

    /**
     * @brief 現在のシーンを取得
     * @return IScene* 現在のシーン（nullptr の可能性あり）
     *
     * @see ChangeScene()
     */
    IScene* GetCurrentScene() const {
        return currentScene_;
    }

    /**
     * @brief デストラクタ
     *
     * 登録されたすべてのシーンを削除します。
     * Shutdown()が明示的に呼ばれていない場合は自動クリーンアップします。
     *
     * @see Shutdown()
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
     * @param[in,out] world ワールド参照
     *
     * デストラクタが呼ばれる前に明示的にリソースを解放したい場合に使用します。
     * 現在のシーンを終了し、登録済みシーンをすべて削除します。
     *
     * @see Init()
     */
    void Shutdown(World& world) {
        if (isShutdown_) {
            return; // 冪等性の確保
        }
        DEBUGLOG_CATEGORY(DebugLog::Category::Scene, "SceneManager::Shutdown() 呼び出し");

        // 現在のシーンを終了
        if (currentScene_) {
            DEBUGLOG_CATEGORY(DebugLog::Category::Scene, "現在のシーンを終了中");
            currentScene_->OnExit(world);
            currentScene_ = nullptr;
        }

        // 登録済みシーンをすべて削除
        if (!scenes_.empty()) {
            DEBUGLOG_CATEGORY(DebugLog::Category::Scene, std::to_string(scenes_.size()) + " 個のシーンを削除中");
            for (auto& pair : scenes_) {
                if (pair.second) {
                    DEBUGLOG_CATEGORY(DebugLog::Category::Scene, "シーン削除: " + pair.first);
                    delete pair.second;
                    pair.second = nullptr;
                }
            }
            scenes_.clear();
        }

        isShutdown_ = true;
        DEBUGLOG_CATEGORY(DebugLog::Category::Scene, "SceneManager::Shutdown() 完了");
    }

private:
    IScene* currentScene_ = nullptr;                           ///< 現在実行中のシーン
    std::unordered_map<std::string, IScene*> scenes_;          ///< 登録されたシーン一覧
    bool isShutdown_ = false;                                   ///< シャットダウン済みフラグ
};
