#pragma once

// ========================================================
// Component - ECSコンポーネントシステム
// ========================================================
// 【コンポーネントとは？】
// ゲームオブジェクトに取り付ける「部品」のこと
// 例: 「位置」「見た目」「動き」などを別々のコンポーネントとして管理
// ========================================================

class World; // 前方宣言
struct Entity; // 前方宣言

// ========================================================
// IComponent - すべてのコンポーネントの基底インターフェース
// ========================================================
// 【役割】型情報を保持するための共通の親クラス
// （初学者は特に意識しなくてOK）
struct IComponent {
    virtual ~IComponent() = default;
};

// ========================================================
// Behaviour - 毎フレーム更新されるコンポーネント
// ========================================================
// 【いつ使う？】
// - オブジェクトを動かしたい
// - 時間経過で何か処理をしたい
// - アニメーションさせたい
// 
// 【使い方】
// struct MyBehaviour : Behaviour {
//     void OnStart(World& w, Entity self) override {
//         // 最初に1回だけ呼ばれる（初期化処理）
//     }
//     void OnUpdate(World& w, Entity self, float dt) override {
//         // 毎フレーム呼ばれる（dt = 前フレームからの経過時間）
//     }
// };
// ========================================================
struct Behaviour : IComponent {
    // 最初に1回だけ呼ばれる（初期化用）
    virtual void OnStart(World&, Entity) {}
    
    // 毎フレーム呼ばれる（dt = デルタタイム = 前フレームからの経過秒数）
    virtual void OnUpdate(World&, Entity, float dt) {}
};

// ========================================================
// 簡単コンポーネント定義マクロ
// ========================================================
// 【使い方】データだけのコンポーネントを1行で定義できる
//
// 例: 体力コンポーネント
// DEFINE_DATA_COMPONENT(Health, float hp = 100.0f; float maxHp = 100.0f;)
//
// 例: 速度コンポーネント
// DEFINE_DATA_COMPONENT(Velocity, 
//     DirectX::XMFLOAT3 velocity{0, 0, 0};
// )
// ========================================================
#define DEFINE_DATA_COMPONENT(ComponentName, ...) \
    struct ComponentName : IComponent { \
        __VA_ARGS__ \
    }

// ========================================================
// 簡単Behaviour定義マクロ
// ========================================================
// 【使い方】動きのあるコンポーネントを簡単に定義
//
// 例: 上下に動くコンポーネント
// DEFINE_BEHAVIOUR(Bouncer,
//     float speed = 1.0f;
//     float time = 0.0f;
// ,
//     time += dt * speed;
//     auto* t = w.TryGet<Transform>(self);
//     if (t) t->position.y = sinf(time);
// )
// ========================================================
#define DEFINE_BEHAVIOUR(BehaviourName, DataMembers, UpdateCode) \
    struct BehaviourName : Behaviour { \
        DataMembers \
        void OnUpdate(World& w, Entity self, float dt) override { \
            UpdateCode \
        } \
    }
