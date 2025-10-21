/**
 * @file App.h
 * @brief ミニゲームのメインアプリケーションクラス
 * @author 山内 陽
 * @date 2025
 * @version 5.1
 */
#pragma once
// ========================================================
// App.h - ミニゲームのアプリケーション
// ========================================================
// 【ゲーム内容】シンプルなシューティングゲーム
// 【操作方法】A/D: 移動, スペース: 弾発射, ESC: 終了
// ========================================================

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include <DirectXMath.h>
#include <chrono>
#include <sstream>

// DirectX11 & ECS システム
#include "graphics/GfxDevice.h"
#include "graphics/RenderSystem.h"
#include "ecs/World.h"
#include "graphics/Camera.h"
#include "input/InputSystem.h"
#include "graphics/TextureManager.h"
#include "graphics/DebugDraw.h"

// コンポーネント
#include "components/Transform.h"
#include "components/MeshRenderer.h"

// ゲームシステム
#include "scenes/SceneManager.h"
#include "scenes/MiniGame.h"

/**
 * @struct App
 * @brief ミニゲームのメインアプリケーションクラス
 * @details
 * ウィンドウ作成、DirectX11の初期化、ECSワールドの管理、
 * メインループの実行など、アプリケーション全体のライフサイクルを管理します。
 */
struct App {
    // Windows関連
    HWND hwnd_ = nullptr; ///< メインウィンドウのハンドル

    // DirectX11システム
    GfxDevice gfx_; ///< グラフィックスデバイス
    RenderSystem renderer_; ///< 描画システム
    TextureManager texManager_; ///< テクスチャ管理

    // ECSシステム
    World world_; ///< ECSワールド
    Camera camera_; ///< カメラ
    InputSystem input_; ///< 入力システム

    // シーン管理
    SceneManager sceneManager_; ///< シーンマネージャー
    GameScene* gameScene_ = nullptr; ///< 現在のゲームシーン

#ifdef _DEBUG
    DebugDraw debugDraw_; ///< デバッグ描画用
#endif

    // ========================================================
    // 初期化
    // ========================================================
    /**
     * @brief アプリケーションの初期化
     * @param[in] hInst アプリケーションのインスタンスハンドル
     * @param[in] width ウィンドウの幅
     * @param[in] height ウィンドウの高さ
     * @return bool 初期化が成功した場合は true, それ以外は false
     */
    bool Init(HINSTANCE hInst, int width = 1280, int height = 720) {
        CoInitializeEx(nullptr, COINIT_MULTITHREADED);

        if (!CreateAppWindow(hInst, width, height)) {
            return false;
        }

        if (!InitializeGraphics(width, height)) {
            return false;
        }

        SetupCamera(width, height);
        
        // ゲームシーンの初期化
        InitializeGame();

        return true;
    }

    // ========================================================
    // メインループ
    // ========================================================
    /**
     * @brief メインループの実行
     * @details
     * アプリケーションが終了するまで、メッセージ処理、更新、描画を繰り返します。
     */
    void Run() {
        MSG msg{};
        auto previousTime = std::chrono::high_resolution_clock::now();
        
        while (msg.message != WM_QUIT) {
            // Windowsメッセージ処理
            if (ProcessWindowsMessages(msg)) {
                continue;
            }
            
            // 時間の計算
            float deltaTime = CalculateDeltaTime(previousTime);
            
            // 入力の更新
            input_.Update();
            
            // ESCキーで終了
            if (input_.GetKeyDown(VK_ESCAPE)) {
                PostQuitMessage(0);
            }
            
            // シーンの更新
            sceneManager_.Update(world_, input_, deltaTime);
            
            // 画面の描画
            RenderFrame();
            
            // ウィンドウタイトルにスコア表示
            UpdateWindowTitle();
        }
    }

    /**
     * @brief デストラクタ
     */
    ~App() {
        CoUninitialize();
    }

private:
    // ========================================================
    // 初期化ヘルパー
    // ========================================================
    
    /**
     * @brief ウィンドウを作成する
     * @param[in] hInst インスタンスハンドル
     * @param[in] width 幅
     * @param[in] height 高さ
     * @return bool 成功した場合は true
     */
    bool CreateAppWindow(HINSTANCE hInst, int width, int height) {
        WNDCLASSEX wc{ sizeof(WNDCLASSEX) };
        wc.style = CS_HREDRAW | CS_VREDRAW;
        wc.lpfnWndProc = WndProcStatic;
        wc.hInstance = hInst;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.lpszClassName = L"MiniGame_Class";
        
        if (!RegisterClassEx(&wc)) {
            return false;
        }

        RECT rc{ 0, 0, width, height };
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
        
        hwnd_ = CreateWindowW(
            wc.lpszClassName,
            L"シューティングゲーム - A/D:移動 スペース:発射 ESC:終了",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            rc.right - rc.left, rc.bottom - rc.top,
            nullptr, nullptr, hInst, this
        );
        
        if (!hwnd_) {
            return false;
        }
        
        ShowWindow(hwnd_, SW_SHOW);
        return true;
    }

    /**
     * @brief グラフィックス関連の初期化
     * @param[in] width 幅
     * @param[in] height 高さ
     * @return bool 成功した場合は true
     */
    bool InitializeGraphics(int width, int height) {
        if (!gfx_.Init(hwnd_, width, height)) {
            MessageBoxA(nullptr, "DirectX11の初期化に失敗", "エラー", MB_OK | MB_ICONERROR);
            return false;
        }
        
        if (!texManager_.Init(gfx_)) {
            MessageBoxA(nullptr, "TextureManagerの初期化に失敗", "エラー", MB_OK | MB_ICONERROR);
            return false;
        }
        
        if (!renderer_.Init(gfx_, texManager_)) {
            MessageBoxA(nullptr, "RenderSystemの初期化に失敗", "エラー", MB_OK | MB_ICONERROR);
            return false;
        }

        input_.Init();

#ifdef _DEBUG
        if (!debugDraw_.Init(gfx_)) {
            MessageBoxA(nullptr, "DebugDrawの初期化に失敗", "警告", MB_OK | MB_ICONWARNING);
        }
#endif

        return true;
    }

    /**
     * @brief カメラのセットアップ
     * @param[in] width 幅
     * @param[in] height 高さ
     */
    void SetupCamera(int width, int height) {
        float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
        
        camera_ = Camera::LookAtLH(
            DirectX::XM_PIDIV4,
            aspectRatio,
            0.1f,
            100.0f,
            DirectX::XMFLOAT3{ 0, 0, -20 },  // カメラを引いて全体が見えるように
            DirectX::XMFLOAT3{ 0, 0, 0 },
            DirectX::XMFLOAT3{ 0, 1, 0 }
        );
    }
    
    /**
     * @brief ゲーム関連の初期化
     */
    void InitializeGame() {
        // ゲームシーンを作成
        gameScene_ = new GameScene();
        
        // シーンマネージャーに登録
        sceneManager_.RegisterScene("Game", gameScene_);
        sceneManager_.Init(gameScene_, world_);
    }

    // ========================================================
    // メインループのヘルパー
    // ========================================================
    
    /**
     * @brief Windowsメッセージを処理する
     * @param[out] msg メッセージ構造体
     * @return bool メッセージを処理した場合は true
     */
    bool ProcessWindowsMessages(MSG& msg) {
        if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            return true;
        }
        return false;
    }
    
    /**
     * @brief デルタタイムを計算する
     * @param[in,out] previousTime 前フレームの時刻
     * @return float デルタタイム（秒）
     */
    float CalculateDeltaTime(std::chrono::high_resolution_clock::time_point& previousTime) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTime = currentTime - previousTime;
        previousTime = currentTime;
        return deltaTime.count();
    }
    
    /**
     * @brief 1フレームを描画する
     */
    void RenderFrame() {
        gfx_.BeginFrame();
        
#ifdef _DEBUG
        DrawDebugInfo();
#endif
        
        renderer_.Render(gfx_, world_, camera_);
        
#ifdef _DEBUG
        debugDraw_.Render(gfx_, camera_);
#endif
        
        gfx_.EndFrame();
    }
    
    /**
     * @brief ウィンドウタイトルを更新する
     */
    void UpdateWindowTitle() {
        if (gameScene_) {
            std::wstringstream ss;
            ss << L"シューティングゲーム - スコア: " << gameScene_->GetScore()
               << L" | A/D:移動 スペース:発射 ESC:終了";
            SetWindowTextW(hwnd_, ss.str().c_str());
        }
    }

#ifdef _DEBUG
    /**
     * @brief デバッグ情報を描画する
     */
    void DrawDebugInfo() {
        debugDraw_.Clear();
        
        // グリッドを少し下げて描画（Y = -0.01）
        debugDraw_.DrawGrid(20.0f, 20, DirectX::XMFLOAT3{0.2f, 0.2f, 0.2f});
        
        // 座標軸を後から描画して見やすくする
        debugDraw_.DrawAxes(5.0f);
    }
#endif

    // ========================================================
    // Windowsメッセージ処理
    // ========================================================
    
    /**
     * @brief ウィンドウプロシージャ（静的）
     * @details
     * ウィンドウ作成時にインスタンスポインタをウィンドウに紐付け、
     * 以降のメッセージをメンバ関数のWndProcに転送します。
     */
    static LRESULT CALLBACK WndProcStatic(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
        App* app = nullptr;
        
        if (msg == WM_NCCREATE) {
            CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lp);
            app = reinterpret_cast<App*>(cs->lpCreateParams);
            SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
        } else {
            app = reinterpret_cast<App*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        }
        
        if (app) {
            return app->WndProc(hWnd, msg, wp, lp);
        }
        
        return DefWindowProc(hWnd, msg, wp, lp);
    }

    /**
     * @brief ウィンドウプロシージャ（メンバ関数）
     * @details アプリケーション固有のウィンドウメッセージを処理します。
     */
    LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
        switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
            
        case WM_MOUSEWHEEL:
            input_.OnMouseWheel(GET_WHEEL_DELTA_WPARAM(wp));
            return 0;
        }
        
        return DefWindowProc(hWnd, msg, wp, lp);
    }
};

// ========================================================
// 作成者: 山内陽
// バージョン: v5.1 - Doxygenコメントを追加
// ========================================================
