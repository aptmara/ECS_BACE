﻿/**
 * @file main.cpp
 * @brief アプリケーションのエントリーポイント
 * @author 山内陽
 * @date 2025
 * @version 5.0
 * 
 * @details
 * このファイルはWindowsアプリケーションのメインエントリーポイント(WinMain)を定義します。
 * アプリケーションの初期化と実行を行います。
 */

// ========================================================
// Simple ECS + DirectX11 Minimal Example (VS2022, C++14)
// ファイル名: main.cpp - エントリーポイント
// ========================================================
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include "app/App.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

// ========================================================
// エントリーポイント
// ========================================================

/**
 * @brief Windowsアプリケーションのメインエントリーポイント
 * 
 * @param[in] hInst アプリケーションのインスタンスハンドル
 * @param[in] HINSTANCE 前のインスタンス(常にNULL、互換性のため残されている)
 * @param[in] LPSTR コマンドライン引数(未使用)
 * @param[in] int ウィンドウの表示状態(未使用)
 * @return int 終了コード(0=成功、-1=失敗)
 * 
 * @details
 * アプリケーションの初期化と実行を行います。
 * 
 * ### 処理の流れ:
 * 1. Appクラスのインスタンスを作成
 * 2. Init()で初期化(DirectX11、ECS、シーンなど)
 * 3. 初期化に失敗した場合、エラーメッセージを表示して終了
 * 4. Run()でメインループを実行
 * 5. アプリケーション終了
 * 
 * @note DirectX11がサポートされていない環境では初期化に失敗します
 * @see App アプリケーションクラス
 */
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int) {
    // アプリケーションインスタンスを作成
    App app;
    
    // 初期化
    if (!app.Init(hInst)) {
        MessageBoxA(nullptr, "Initialization failed!\nCheck DirectX 11 support.", "Error", MB_ICONERROR | MB_OK);
        return -1;
    }
    
    // メインループを実行
    app.Run();
    
    return 0;
}
