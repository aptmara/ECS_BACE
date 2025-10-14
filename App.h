#pragma once
// ========================================================
// プリプロセッサディレクティブ（コンパイル前の設定）
// ========================================================

// WIN32_LEAN_AND_MEAN: Windowsヘッダーから使わない部分を除外して、コンパイルを高速化
#define WIN32_LEAN_AND_MEAN

// NOMINMAX: Windowsの min/max マクロを無効化（C++標準のstd::min/maxと競合するため）
#define NOMINMAX

// ========================================================
// 必要なヘッダーファイルのインクルード（外部機能の読み込み）
// ========================================================

// Windows API の基本機能（ウィンドウ作成、メッセージ処理など）
#include <Windows.h>

// 自作ヘッダーファイル群
#include "GfxDevice.h"      // DirectX11のデバイス管理クラス
#include "RenderSystem.h"   // 描画システム（シェーダー、描画処理）
#include "World.h"          // ECSワールド（エンティティとコンポーネントの管理）
#include "Camera.h"         // カメラ（視点と投影の設定）
#include "Transform.h"      // 位置・回転・スケール情報
#include "MeshRenderer.h"   // メッシュの描画設定
#include "Rotator.h"        // 自動回転の振る舞い

// DirectXの数学ライブラリ（ベクトル、行列計算用）
#include <DirectXMath.h>

// 時間計測用のC++標準ライブラリ
#include <chrono>

// ========================================================
// App - アプリケーション全体を管理するクラス
// （ウィンドウ、グラフィックス、ゲームループを統括）
// ========================================================
struct App {
    // ========================================================
    // メンバ変数（このクラスが保持するデータ）
    // ========================================================
    
    HWND hwnd = nullptr;        // ウィンドウハンドル（作成したウィンドウの識別子）
    GfxDevice gfx;              // DirectX11のデバイス管理オブジェクト
    RenderSystem renderer;      // 描画システム（シェーダーや描画処理）
    World world;                // ECSワールド（全エンティティとコンポーネントを管理）
    Camera cam;                 // カメラ（3D空間の視点設定）

    // ========================================================
    // 初期化メソッド - アプリケーションの起動準備
    // ========================================================
    // 引数:
    //   hInst: アプリケーションのインスタンスハンドル（Windowsが自動的に渡す）
    //   width: ウィンドウの横幅（ピクセル単位）デフォルトは1280
    //   height: ウィンドウの縦幅（ピクセル単位）デフォルトは720
    // 戻り値:
    //   true: 初期化成功
    //   false: 初期化失敗（どこかでエラーが発生）
    bool Init(HINSTANCE hInst, int width = 1280, int height = 720) {
        
        // ========================================================
        // ステップ1: ウィンドウクラスの登録
        // （Windowsにウィンドウの設計図を教える）
        // ========================================================
        
        // WNDCLASSEX構造体を初期化（サイズ情報を自動設定）
        WNDCLASSEX wc{ sizeof(WNDCLASSEX) };
        
        // ウィンドウスタイル: 横幅・縦幅が変わったら全体を再描画
        wc.style = CS_HREDRAW | CS_VREDRAW;
        
        // ウィンドウプロシージャ（イベント処理関数）を登録
        // WndProcStatic: ウィンドウに何か起きたとき（クリック、閉じる等）に呼ばれる関数
        wc.lpfnWndProc = WndProcStatic;
        
        // このアプリケーションのインスタンスハンドルを設定
        wc.hInstance = hInst;
        
        // マウスカーソルの形状を設定（IDC_ARROW = 通常の矢印カーソル）
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        
        // ウィンドウクラスの名前（識別用の文字列、L""はワイド文字列）
        wc.lpszClassName = L"SimpleECS_DX11_Class";
        
        // Windowsにウィンドウクラスを登録（失敗したら false を返す）
        if (!RegisterClassEx(&wc)) return false;

        // ========================================================
        // ステップ2: ウィンドウサイズの調整
        // （クライアント領域を指定サイズにするため、枠の分を計算）
        // ========================================================
        
        // RECT構造体: 矩形領域を表す（left, top, right, bottom）
        RECT rc{ 0, 0, width, height };
        
        // AdjustWindowRect: タイトルバーや枠を含めた実際のウィンドウサイズを計算
        // WS_OVERLAPPEDWINDOW: 標準的なウィンドウ（タイトルバー、最小化/最大化ボタン付き）
        // FALSE: メニューバーなし
        AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
        
        // ========================================================
        // ステップ3: ウィンドウの作成
        // ========================================================
        
        hwnd = CreateWindowW(
            wc.lpszClassName,           // 登録したウィンドウクラスの名前
            L"Simple ECS + DX11",       // ウィンドウのタイトルバーに表示される文字列
            WS_OVERLAPPEDWINDOW,        // ウィンドウスタイル（通常のウィンドウ）
            CW_USEDEFAULT,              // X座標（デフォルト位置）
            CW_USEDEFAULT,              // Y座標（デフォルト位置）
            rc.right - rc.left,         // ウィンドウの幅（枠込み）
            rc.bottom - rc.top,         // ウィンドウの高さ（枠込み）
            nullptr,                    // 親ウィンドウなし
            nullptr,                    // メニューなし
            hInst,                      // アプリケーションインスタンス
            this                        // WM_NCCREATEメッセージでこのポインタが渡される
        );
        
        // ウィンドウ作成に失敗した場合
        if (!hwnd) return false;
        
        // ウィンドウを画面に表示（SW_SHOW = 通常表示）
        ShowWindow(hwnd, SW_SHOW);

        // ========================================================
        // ステップ4: グラフィックス初期化
        // ========================================================
        
        // DirectX11デバイスの初期化（失敗したら false を返す）
        if (!gfx.Init(hwnd, width, height)) return false;
        
        // レンダリングシステムの初期化（シェーダーや描画パイプラインの準備）
        if (!renderer.Init(gfx)) return false;

        // ========================================================
        // ステップ5: カメラの設定
        // （3D空間をどの視点から見るかを決定）
        // ========================================================
        
        // アスペクト比（横幅÷縦幅）を計算
        float aspect = float(width) / float(height);
        
        // カメラを作成（LookAtLH = Left-Handed座標系でカメラを配置）
        cam = Camera::LookAtLH(
            DirectX::XM_PIDIV4,              // 視野角（fovY）= π/4 ? 45度
            aspect,                          // アスペクト比（横÷縦）
            0.1f,                            // ニアクリップ面（これより近い物は描画されない）
            100.0f,                          // ファークリップ面（これより遠い物は描画されない）
            DirectX::XMFLOAT3{ 0, 2, -6 },   // カメラの位置（目の位置: X=0, Y=2, Z=-6）
            DirectX::XMFLOAT3{ 0, 0, 0 },    // 注視点（カメラが見ている場所: 原点）
            DirectX::XMFLOAT3{ 0, 1, 0 }     // 上方向ベクトル（Y軸が上）
        );

        // ========================================================
        // ステップ6: シーンの作成（ゲームオブジェクトの配置）
        // ========================================================
        
        // 新しいエンティティ（ゲームオブジェクト）を作成
        Entity e = world.CreateEntity();
        
        // Transformコンポーネントを追加（位置・回転・スケール情報）
        // 位置: (0, 0, 0) 原点
        // 回転: (0, 0, 0) 回転なし
        // スケール: (1, 1, 1) 等倍サイズ
        world.Add<Transform>(e, Transform{ 
            DirectX::XMFLOAT3{0, 0, 0},     // position（位置）
            DirectX::XMFLOAT3{0, 0, 0},     // rotation（回転、度数法）
            DirectX::XMFLOAT3{1, 1, 1}      // scale（スケール）
        });
        
        // MeshRendererコンポーネントを追加（描画設定）
        // 色: RGB(0.2, 0.7, 1.0) = 水色
        world.Add<MeshRenderer>(e, MeshRenderer{ 
            DirectX::XMFLOAT3{0.2f, 0.7f, 1.0f}  // color（赤・緑・青の値、0.0?1.0）
        });
        
        // Rotatorコンポーネントを追加（自動回転の振る舞い）
        // 毎秒45度の速度でY軸周りに回転
        world.Add<Rotator>(e, Rotator{ 45.0f });

        // 初期化成功
        return true;
    }

    // ========================================================
    // メインループ実行 - アプリケーションの心臓部
    // （ゲームが動いている間、この関数がずっと動き続ける）
    // ========================================================
    void Run() {
        // MSG構造体: Windowsからのメッセージ（イベント）を格納
        MSG msg{};
        
        // 前回のフレーム時刻を記録（高精度タイマーを使用）
        auto prev = std::chrono::high_resolution_clock::now();
        
        // メインループ（WM_QUIT メッセージを受け取るまで永遠に繰り返す）
        while (msg.message != WM_QUIT) {
            
            // ========================================================
            // フェーズ1: Windowsメッセージの処理
            // （マウス、キーボード、ウィンドウイベント等）
            // ========================================================
            
            // PeekMessage: メッセージキューを確認（待機せずに即座に戻る）
            // PM_REMOVE: メッセージを取得したらキューから削除
            if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
                // TranslateMessage: キーボードメッセージを文字に変換
                TranslateMessage(&msg);
                
                // DispatchMessage: ウィンドウプロシージャにメッセージを送る
                DispatchMessage(&msg);
                
                // メッセージ処理が終わったら次のループへ
                continue;
            }
            
            // ========================================================
            // フェーズ2: デルタタイム（前フレームからの経過時間）の計算
            // ========================================================
            
            // 現在時刻を取得
            auto now = std::chrono::high_resolution_clock::now();
            
            // デルタタイム（dt）= 現在時刻 - 前回時刻
            // duration<float> で秒単位の浮動小数点数に変換
            std::chrono::duration<float> dt = now - prev;
            
            // 次回の計算のため、現在時刻を保存
            prev = now;

            // ========================================================
            // フェーズ3: ゲームロジックの更新
            // ========================================================
            
            // 全Behaviourコンポーネントを更新（Rotatorなどが回転処理を実行）
            world.Tick(dt.count());
            
            // ========================================================
            // フェーズ4: 描画処理
            // ========================================================
            
            // フレーム開始（画面をクリアして描画準備）
            gfx.BeginFrame();
            
            // 全オブジェクトを描画（カメラ視点で3D空間をレンダリング）
            renderer.Render(gfx, world, cam);
            
            // フレーム終了（バックバッファを画面に表示）
            gfx.EndFrame();
        }
    }

    // ========================================================
    // ウィンドウプロシージャ（静的メソッド）
    // Windowsから直接呼び出される、イベント処理の入口
    // ========================================================
    // 【重要な仕組み】
    // Windowsは「クラスのメンバ関数」を直接コールバックとして登録できないため、
    // staticメソッド（thisポインタを持たない）を経由して、
    // 実際のメンバ関数 WndProc を呼び出すトリックを使っている
    // ========================================================
    static LRESULT CALLBACK WndProcStatic(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
        App* self = nullptr;  // このウィンドウに関連付けられたAppオブジェクトへのポインタ
        
        // WM_NCCREATE: ウィンドウが作成される直前に送られるメッセージ
        if (msg == WM_NCCREATE) {
            // lpに渡されたCREATESTRUCT構造体を取得
            CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lp);
            
            // CreateWindowWの最後の引数（this）がlpCreateParamsに格納されている
            self = reinterpret_cast<App*>(cs->lpCreateParams);
            
            // SetWindowLongPtr: ウィンドウにユーザーデータ（Appポインタ）を関連付ける
            // GWLP_USERDATA: ユーザー定義データ用のスロット
            SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)self);
        }
        else {
            // WM_NCCREATE以外のメッセージでは、保存されたポインタを取得
            self = reinterpret_cast<App*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
        }
        
        // Appオブジェクトのポインタが有効なら、メンバ関数 WndProc を呼ぶ
        if (self) return self->WndProc(hWnd, msg, wp, lp);
        
        // Appオブジェクトがまだ関連付けられていない場合は、デフォルト処理
        return DefWindowProc(hWnd, msg, wp, lp);
    }

    // ========================================================
    // ウィンドウプロシージャ（メンバ関数）
    // 実際のイベント処理を行う
    // ========================================================
    // 引数:
    //   hWnd: イベントが発生したウィンドウのハンドル
    //   msg: メッセージの種類（WM_DESTROYなど）
    //   wp: メッセージ固有のパラメータ1
    //   lp: メッセージ固有のパラメータ2
    // 戻り値:
    //   メッセージ処理の結果（通常は0を返す）
    LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) {
        switch (msg) {
        
        // WM_DESTROY: ウィンドウが破棄されるときに送られるメッセージ
        case WM_DESTROY:
            // PostQuitMessage: アプリケーションにWM_QUITメッセージを送る
            // これによりメインループが終了し、プログラムが終わる
            PostQuitMessage(0);
            return 0;
            
        // その他のメッセージ（処理しない）
        default:
            break;
        }
        
        // 処理しないメッセージはWindowsのデフォルト処理に任せる
        return DefWindowProc(hWnd, msg, wp, lp);
    }
};

// ========================================================
// コード署名
// 作成者: 山内陽
// ========================================================
