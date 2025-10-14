// ========================================================
// ComponentSamples.h - 学習用コンポーネント集
// ========================================================
// 【目的】コピペして使える実用的なコンポーネントサンプル
// 【学び方】コードを読む → 改造する → 自分で作る
// ========================================================
#pragma once

#include "Component.h"
#include "World.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include <DirectXMath.h>
#include <cmath>
#include <cstdlib>

// ========================================================
// カテゴリ1: データだけのコンポーネント
// ========================================================
// 【特徴】状態を保存するだけ、動かない
// 【使い方】他のBehaviourから参照される
// ========================================================

// 体力コンポーネント
struct Health : IComponent {
    float current = 100.0f;  // 現在の体力
    float max = 100.0f;      // 最大体力
    
    // ダメージを受ける
    void TakeDamage(float damage) {
        current -= damage;
        if (current < 0.0f) current = 0.0f;
    }
    
    // 回復する
    void Heal(float amount) {
        current += amount;
        if (current > max) current = max;
    }
    
    // 死んでいるか
    bool IsDead() const {
        return current <= 0.0f;
    }
};

// 速度コンポーネント
struct Velocity : IComponent {
    DirectX::XMFLOAT3 velocity{ 0.0f, 0.0f, 0.0f };
    
    // 速度を加える
    void AddVelocity(float x, float y, float z) {
        velocity.x += x;
        velocity.y += y;
        velocity.z += z;
    }
};

// スコアコンポーネント（マクロ版）
DEFINE_DATA_COMPONENT(Score,
    int points = 0;
    
    void AddPoints(int p) {
        points += p;
    }
    
    void Reset() {
        points = 0;
    }
);

// 名前コンポーネント（マクロ版）
DEFINE_DATA_COMPONENT(Name,
    const char* name = "Unnamed";
);

// タグコンポーネント（データなし）
struct PlayerTag : IComponent {};
struct EnemyTag : IComponent {};
struct BulletTag : IComponent {};

// ========================================================
// カテゴリ2: シンプルなBehaviour（1つの機能）
// ========================================================
// 【特徴】1つの明確な動作をする
// 【学習ポイント】OnUpdateの使い方
// ========================================================

// 上下に動く（バウンス）
struct Bouncer : Behaviour {
    float speed = 2.0f;      // 動く速度
    float amplitude = 2.0f;  // 動く幅
    float time = 0.0f;       // 経過時間（内部管理）
    float startY = 0.0f;     // 開始位置（内部管理）
    
    void OnStart(World& w, Entity self) override {
        // 最初に1回だけ呼ばれる
        auto* t = w.TryGet<Transform>(self);
        if (t) {
            startY = t->position.y; // 開始位置を記録
        }
    }
    
    void OnUpdate(World& w, Entity self, float dt) override {
        // 毎フレーム呼ばれる
        time += dt * speed;
        
        auto* t = w.TryGet<Transform>(self);
        if (t) {
            // sin波で上下に動かす
            t->position.y = startY + sinf(time) * amplitude;
        }
    }
};

// 前に進む
struct MoveForward : Behaviour {
    float speed = 2.0f; // 前進速度（単位/秒）
    
    void OnUpdate(World& w, Entity self, float dt) override {
        auto* t = w.TryGet<Transform>(self);
        if (!t) return;
        
        // Z軸方向（前）に進む
        t->position.z += speed * dt;
        
        // 遠くに行ったら削除（オプション）
        if (t->position.z > 20.0f) {
            w.DestroyEntity(self);
        }
    }
};

// 拡大縮小（パルス）
struct PulseScale : Behaviour {
    float speed = 3.0f;           // パルス速度
    float minScale = 0.5f;        // 最小スケール
    float maxScale = 1.5f;        // 最大スケール
    float time = 0.0f;            // 経過時間（内部管理）
    
    void OnUpdate(World& w, Entity self, float dt) override {
        time += dt * speed;
        
        auto* t = w.TryGet<Transform>(self);
        if (!t) return;
        
        // sin波でスケールを変化
        float scale = minScale + (maxScale - minScale) * (sinf(time) * 0.5f + 0.5f);
        t->scale = DirectX::XMFLOAT3{ scale, scale, scale };
    }
};

// 色が変わる（サイクル）
struct ColorCycle : Behaviour {
    float speed = 1.0f;  // 色変化の速度
    float time = 0.0f;   // 経過時間（内部管理）
    
    void OnUpdate(World& w, Entity self, float dt) override {
        time += dt * speed;
        
        auto* mr = w.TryGet<MeshRenderer>(self);
        if (!mr) return;
        
        // HSV風に色を変化（虹色）
        float hue = fmodf(time, 1.0f);
        mr->color.x = sinf(hue * DirectX::XM_2PI) * 0.5f + 0.5f;
        mr->color.y = sinf((hue + 0.333f) * DirectX::XM_2PI) * 0.5f + 0.5f;
        mr->color.z = sinf((hue + 0.666f) * DirectX::XM_2PI) * 0.5f + 0.5f;
    }
};

// ========================================================
// カテゴリ3: 少し複雑なBehaviour（複数の機能）
// ========================================================
// 【特徴】複数のコンポーネントを組み合わせる
// 【学習ポイント】コンポーネント間の連携
// ========================================================

// 体力が0になったら削除
struct DestroyOnDeath : Behaviour {
    void OnUpdate(World& w, Entity self, float dt) override {
        // Healthコンポーネントを確認
        auto* health = w.TryGet<Health>(self);
        if (!health) return;
        
        // 体力が0以下なら削除
        if (health->IsDead()) {
            w.DestroyEntity(self);
        }
    }
};

// ランダムに動き回る
struct RandomWalk : Behaviour {
    float speed = 2.0f;             // 移動速度
    float changeInterval = 2.0f;    // 方向転換の間隔（秒）
    float timer = 0.0f;             // タイマー（内部管理）
    DirectX::XMFLOAT3 direction{ 1.0f, 0.0f, 0.0f }; // 現在の方向
    
    void OnStart(World& w, Entity self) override {
        // ランダムな初期方向
        ChooseRandomDirection();
    }
    
    void OnUpdate(World& w, Entity self, float dt) override {
        timer += dt;
        
        // 一定時間ごとに方向転換
        if (timer >= changeInterval) {
            timer = 0.0f;
            ChooseRandomDirection();
        }
        
        // 移動
        auto* t = w.TryGet<Transform>(self);
        if (!t) return;
        
        t->position.x += direction.x * speed * dt;
        t->position.y += direction.y * speed * dt;
        t->position.z += direction.z * speed * dt;
        
        // 範囲外に出たら戻す
        ClampPosition(t);
    }
    
private:
    void ChooseRandomDirection() {
        // -1.0 ～ 1.0 のランダムな値
        direction.x = (static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f;
        direction.y = (static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f;
        direction.z = (static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f;
        
        // 正規化（長さを1に）
        float length = sqrtf(direction.x * direction.x + 
                           direction.y * direction.y + 
                           direction.z * direction.z);
        if (length > 0.0f) {
            direction.x /= length;
            direction.y /= length;
            direction.z /= length;
        }
    }
    
    void ClampPosition(Transform* t) {
        const float range = 10.0f;
        if (t->position.x < -range) t->position.x = -range;
        if (t->position.x > range) t->position.x = range;
        if (t->position.y < -range) t->position.y = -range;
        if (t->position.y > range) t->position.y = range;
        if (t->position.z < -range) t->position.z = -range;
        if (t->position.z > range) t->position.z = range;
    }
};

// 時間経過で削除
struct LifeTime : Behaviour {
    float remainingTime = 5.0f; // 残り時間（秒）
    
    void OnUpdate(World& w, Entity self, float dt) override {
        remainingTime -= dt;
        
        // 時間切れで削除
        if (remainingTime <= 0.0f) {
            w.DestroyEntity(self);
        }
    }
};

// ========================================================
// カテゴリ4: マクロを使った簡潔な定義
// ========================================================
// 【特徴】DEFINE_BEHAVIOURマクロで短く書ける
// 【学習ポイント】ボイラープレートの削減
// ========================================================

// 回転しながら色が変わる（マクロ版）
DEFINE_BEHAVIOUR(SpinAndColor,
    float rotSpeed = 90.0f;
    float colorSpeed = 1.0f;
    float time = 0.0f;
,
    time += dt * colorSpeed;
    
    // 回転
    auto* t = w.TryGet<Transform>(self);
    if (t) {
        t->rotation.y += rotSpeed * dt;
    }
    
    // 色変化
    auto* mr = w.TryGet<MeshRenderer>(self);
    if (mr) {
        float hue = fmodf(time, 1.0f);
        mr->color.x = sinf(hue * 6.28f) * 0.5f + 0.5f;
        mr->color.y = cosf(hue * 6.28f) * 0.5f + 0.5f;
        mr->color.z = 0.5f;
    }
);

// 円軌道を描く（マクロ版）
DEFINE_BEHAVIOUR(CircularMotion,
    float radius = 3.0f;
    float speed = 1.0f;
    float angle = 0.0f;
    float centerY = 0.0f;
,
    angle += speed * dt;
    
    auto* t = w.TryGet<Transform>(self);
    if (t) {
        t->position.x = cosf(angle) * radius;
        t->position.z = sinf(angle) * radius;
        t->position.y = centerY;
    }
);

// ========================================================
// 使い方の例
// ========================================================
/*

// 例1: 上下に動く赤いキューブ
Entity cube = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
    .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
    .With<Bouncer>()
    .Build();

// 例2: 5秒後に消えるキューブ
Entity temp = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
    .With<MeshRenderer>(DirectX::XMFLOAT3{0, 1, 0})
    .Build();

LifeTime lt;
lt.remainingTime = 5.0f;
world.Add<LifeTime>(temp, lt);

// 例3: 体力システム付きキューブ
Entity enemy = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
    .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
    .With<EnemyTag>()
    .Build();

Health hp;
hp.current = 50.0f;
hp.max = 50.0f;
world.Add<Health>(enemy, hp);
world.Add<DestroyOnDeath>(enemy, DestroyOnDeath{});

// ダメージを与える
auto* health = world.TryGet<Health>(enemy);
if (health) {
    health->TakeDamage(10.0f);
}

*/

// ========================================================
// 作成者: 山内陽
// バージョン: v4.0 - 学習用コンポーネント集
// ========================================================
