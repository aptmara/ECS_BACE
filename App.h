#pragma once
// ========================================================
// App.h - ミニゲーム版アプリケーション
// ========================================================
// 【ゲーム内容】シンプルなシューティングゲーム
// 【操作方法】A/D: 移動、スペース: 弾発射、ESC: 終了
// ========================================================

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include <DirectXMath.h>
#include <chrono>
#include <sstream>

// DirectX11 & ECS システム
#include "GfxDevice.h"
#include "RenderSystem.h"
#include "World.h"
#include "Camera.h"
#include "InputSystem.h"
#include "TextureManager.h"
#include "DebugDraw.h"

// コンポーネント
#include "Transform.h"
#include "MeshRenderer.h"

// ゲームシステム
#include "SceneManager.h"
#include "MiniGame.h"

// ========================================================
// App - ミニゲームアプリケーション
// ========================================================
struct App {
    // Windows関連
    HWND hwnd_ = nullptr;
    
    // DirectX11システム
    GfxDevice gfx_;
    RenderSystem renderer_;
    TextureManager texManager_;
    
    // ECSシステム
    World world_;
    Camera camera_;
    InputSystem input_;
    
    // シーン管理
    SceneManager sceneManager_;
    GameScene* gameScene_ = nullptr;
    
#ifdef _DEBUG
    DebugDraw debugDraw_;
#endif

    // ========================================================
    // 初期化
    // ========================================================
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

    ~App() {
        CoUninitialize();
    }

private:
    // ========================================================
    // 初期化ヘルパー
    // ========================================================
    
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
    
    bool ProcessWindowsMessages(MSG& msg) {
        if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            return true;
        }
        return false;
    }
    
    float CalculateDeltaTime(std::chrono::high_resolution_clock::time_point& previousTime) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> deltaTime = currentTime - previousTime;
        previousTime = currentTime;
        return deltaTime.count();
    }
    
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
    
    void UpdateWindowTitle() {
        if (gameScene_) {
            std::wstringstream ss;
            ss << L"シューティングゲーム - スコア: " << gameScene_->GetScore()
               << L" | A/D:移動 スペース:発射 ESC:終了";
            SetWindowTextW(hwnd_, ss.str().c_str());
        }
    }

#ifdef _DEBUG
    void DrawDebugInfo() {
        debugDraw_.Clear();
        debugDraw_.DrawGrid(20.0f, 20, DirectX::XMFLOAT3{0.2f, 0.2f, 0.2f});
        debugDraw_.DrawAxes(5.0f);
    }
#endif

    // ========================================================
    // Windowsメッセージ処理
    // ========================================================
    
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
// バージョン: v5.0 - ミニゲーム実装版（可読性最重視）
// ========================================================
