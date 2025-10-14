// ========================================================
// Simple ECS + DirectX11 Minimal Example (VS2022, C++17)
// ファイル分割版 - エントリーポイント
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
        MessageBoxA(nullptr, "Init failed", "Error", MB_ICONERROR);
        return -1;
    }
    app.Run();
    return 0;
}
