// ========================================================
// Simple ECS + DirectX11 Minimal Example (VS2022, C++14)
// ファイル名: main.cpp - エントリーポイント
// ========================================================
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include "App.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

// ========================================================
// エントリーポイント
// ========================================================
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int) {
    App app;
    if (!app.Init(hInst)) {
        MessageBoxA(nullptr, "Initialization failed!\nCheck DirectX 11 support.", "Error", MB_ICONERROR | MB_OK);
        return -1;
    }
    app.Run();
    return 0;
}
