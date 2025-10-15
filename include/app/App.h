#pragma once
// ========================================================
// App.h - �~�j�Q�[���ŃA�v���P�[�V����
// ========================================================
// �y�Q�[�����e�z�V���v���ȃV���[�e�B���O�Q�[��
// �y������@�zA/D: �ړ��A�X�y�[�X: �e���ˁAESC: �I��
// ========================================================

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include <DirectXMath.h>
#include <chrono>
#include <sstream>

// DirectX11 & ECS �V�X�e��
#include "graphics/GfxDevice.h"
#include "graphics/RenderSystem.h"
#include "ecs/World.h"
#include "graphics/Camera.h"
#include "input/InputSystem.h"
#include "graphics/TextureManager.h"
#include "graphics/DebugDraw.h"

// �R���|�[�l���g
#include "components/Transform.h"
#include "components/MeshRenderer.h"

// �Q�[���V�X�e��
#include "scenes/SceneManager.h"
#include "scenes/MiniGame.h"

// ========================================================
// App - �~�j�Q�[���A�v���P�[�V����
// ========================================================
struct App {
    // Windows�֘A
    HWND hwnd_ = nullptr;
    
    // DirectX11�V�X�e��
    GfxDevice gfx_;
    RenderSystem renderer_;
    TextureManager texManager_;
    
    // ECS�V�X�e��
    World world_;
    Camera camera_;
    InputSystem input_;
    
    // �V�[���Ǘ�
    SceneManager sceneManager_;
    GameScene* gameScene_ = nullptr;
    
#ifdef _DEBUG
    DebugDraw debugDraw_;
#endif

    // ========================================================
    // ������
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
        
        // �Q�[���V�[���̏�����
        InitializeGame();

        return true;
    }

    // ========================================================
    // ���C�����[�v
    // ========================================================
    void Run() {
        MSG msg{};
        auto previousTime = std::chrono::high_resolution_clock::now();
        
        while (msg.message != WM_QUIT) {
            // Windows���b�Z�[�W����
            if (ProcessWindowsMessages(msg)) {
                continue;
            }
            
            // ���Ԃ̌v�Z
            float deltaTime = CalculateDeltaTime(previousTime);
            
            // ���͂̍X�V
            input_.Update();
            
            // ESC�L�[�ŏI��
            if (input_.GetKeyDown(VK_ESCAPE)) {
                PostQuitMessage(0);
            }
            
            // �V�[���̍X�V
            sceneManager_.Update(world_, input_, deltaTime);
            
            // ��ʂ̕`��
            RenderFrame();
            
            // �E�B���h�E�^�C�g���ɃX�R�A�\��
            UpdateWindowTitle();
        }
    }

    ~App() {
        CoUninitialize();
    }

private:
    // ========================================================
    // �������w���p�[
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
            L"�V���[�e�B���O�Q�[�� - A/D:�ړ� �X�y�[�X:���� ESC:�I��",
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
            MessageBoxA(nullptr, "DirectX11�̏������Ɏ��s", "�G���[", MB_OK | MB_ICONERROR);
            return false;
        }
        
        if (!texManager_.Init(gfx_)) {
            MessageBoxA(nullptr, "TextureManager�̏������Ɏ��s", "�G���[", MB_OK | MB_ICONERROR);
            return false;
        }
        
        if (!renderer_.Init(gfx_, texManager_)) {
            MessageBoxA(nullptr, "RenderSystem�̏������Ɏ��s", "�G���[", MB_OK | MB_ICONERROR);
            return false;
        }

        input_.Init();

#ifdef _DEBUG
        if (!debugDraw_.Init(gfx_)) {
            MessageBoxA(nullptr, "DebugDraw�̏������Ɏ��s", "�x��", MB_OK | MB_ICONWARNING);
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
            DirectX::XMFLOAT3{ 0, 0, -20 },  // �J�����������đS�̂�������悤��
            DirectX::XMFLOAT3{ 0, 0, 0 },
            DirectX::XMFLOAT3{ 0, 1, 0 }
        );
    }
    
    void InitializeGame() {
        // �Q�[���V�[�����쐬
        gameScene_ = new GameScene();
        
        // �V�[���}�l�[�W���[�ɓo�^
        sceneManager_.RegisterScene("Game", gameScene_);
        sceneManager_.Init(gameScene_, world_);
    }

    // ========================================================
    // ���C�����[�v�̃w���p�[
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
            ss << L"�V���[�e�B���O�Q�[�� - �X�R�A: " << gameScene_->GetScore()
               << L" | A/D:�ړ� �X�y�[�X:���� ESC:�I��";
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
    // Windows���b�Z�[�W����
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
// �쐬��: �R���z
// �o�[�W����: v5.0 - �~�j�Q�[�������Łi�ǐ��ŏd���j
// ========================================================
