/**
 * @file ComponentSamples.h
 * @brief 学習用コンポーネント集
 * @author 山内陽
 * @date 2024
 * @version 4.0
 * 
 * @details
 * コピペして使える実用的なコンポーネントサンプル集です。
 * 学習方針：コードを読む → 理解する → 改造する
 */
#pragma once

#include "components/Component.h"
#include "ecs/World.h"
#include "components/Transform.h"
#include "components/MeshRenderer.h"
#include <DirectXMath.h>
#include <cmath>
#include <cstdlib>

// ========================================================
// カテゴリ1: データ型のコンポーネント
// ========================================================
/**
 * @brief データ型のコンポーネント
 * @details 状態を保存するだけで、動作はしない
 * 使い方：他のBehaviourから参照される
 */

/**
 * @struct Health
 * @brief 体力コンポーネント
 * 
 * @details
 * エンティティの体力を管理します。
 * ダメージや回復、死亡判定を提供します。
 * 
 * @author 山内陽
 */
struct Health : IComponent {
    float current = 100.0f;  ///< 現在の体力
    float max = 100.0f;      ///< 最大体力
    
    /**
     * @brief ダメージを受ける
     * @param[in] damage ダメージ量
     */
    void TakeDamage(float damage) {
        current -= damage;
        if (current < 0.0f) current = 0.0f;
    }
    
    /**
     * @brief 回復する
     * @param[in] amount 回復量
     */
    void Heal(float amount) {
        current += amount;
        if (current > max) current = max;
    }
    
    /**
     * @brief 死亡しているか
     * @return true 死亡している, false 生存している
     */
    bool IsDead() const {
        return current <= 0.0f;
    }
};

/**
 * @struct Velocity
 * @brief 速度コンポーネント
 * 
 * @details
 * エンティティの速度ベクトルを保持します。
 * 
 * @author 山内陽
 */
struct Velocity : IComponent {
    DirectX::XMFLOAT3 velocity{ 0.0f, 0.0f, 0.0f };  ///< 速度ベクトル
    
    /**
     * @brief 速度を加算
     * @param[in] x X方向の速度
     * @param[in] y Y方向の速度
     * @param[in] z Z方向の速度
     */
    void AddVelocity(float x, float y, float z) {
        velocity.x += x;
        velocity.y += y;
        velocity.z += z;
    }
};

/**
 * @brief スコアコンポーネント（マクロ版）
 */
DEFINE_DATA_COMPONENT(Score,
    int points = 0;  ///< 獲得ポイント
    
    /**
     * @brief ポイントを加算
     * @param[in] p 加算するポイント
     */
    void AddPoints(int p) {
        points += p;
    }
    
    /**
     * @brief スコアをリセット
     */
    void Reset() {
        points = 0;
    }
);

/**
 * @brief 名前コンポーネント（マクロ版）
 */
DEFINE_DATA_COMPONENT(Name,
    const char* name = "Unnamed";  ///< エンティティ名
);

/**
 * @brief タグコンポーネント（データなし）
 * @details エンティティの種類を識別するためのマーカー
 */
struct PlayerTag : IComponent {};  ///< プレイヤータグ
struct EnemyTag : IComponent {};   ///< 敵タグ
struct BulletTag : IComponent {};  ///< 弾タグ

// ========================================================
// カテゴリ2: シンプルなBehaviour（1つの機能）
// ========================================================
/**
 * @brief シンプルなBehaviour
 * @details 1つの明確な動作を実装
 * 学習ポイント：OnUpdateの使い方
 */

/**
 * @struct Bouncer
 * @brief 上下に跳ねる（バウンス）Behaviour
 * 
 * @details
 * sin波を使ってエンティティを上下に跳ねさせます。
 * OnStartで初期位置を記録し、OnUpdateで位置を更新します。
 * 
 * @author 山内陽
 */
struct Bouncer : Behaviour {
    float speed = 2.0f;      ///< 跳ねる速度
    float amplitude = 2.0f;  ///< 振幅
    float time = 0.0f;       ///< 経過時間（内部管理）
    float startY = 0.0f;     ///< 開始位置（内部管理）
    
    /**
     * @brief 初期化処理
     * @param[in,out] w ワールド参照
     * @param[in] self 自身のエンティティ
     */
    void OnStart(World& w, Entity self) override {
        // 最初に1回だけ呼ばれる
        auto* t = w.TryGet<Transform>(self);
        if (t) {
            startY = t->position.y; // 開始位置を記録
        }
    }
    
    /**
     * @brief 毎フレーム更新処理
     * @param[in,out] w ワールド参照
     * @param[in] self 自身のエンティティ
     * @param[in] dt デルタタイム（秒）
     */
    void OnUpdate(World& w, Entity self, float dt) override {
        // 毎フレーム呼ばれる
        time += dt * speed;
        
        auto* t = w.TryGet<Transform>(self);
        if (t) {
            // sin波で上下に跳ねる
            t->position.y = startY + sinf(time) * amplitude;
        }
    }
};

/**
 * @struct MoveForward
 * @brief 前に進むBehaviour
 * 
 * @details
 * Z軸方向（前方）に一定速度で移動します。
 * 範囲外に出たら自動的に削除されます。
 * 
 * @author 山内陽
 */
struct MoveForward : Behaviour {
    float speed = 2.0f; ///< 前進速度（単位/秒）
    
    /**
     * @brief 毎フレーム更新処理
     * @param[in,out] w ワールド参照
     * @param[in] self 自身のエンティティ
     * @param[in] dt デルタタイム（秒）
     */
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

/**
 * @struct PulseScale
 * @brief 拡大縮小（パルス）Behaviour
 * 
 * @details
 * sin波を使ってエンティティのスケールを周期的に変化させます。
 * 
 * @author 山内陽
 */
struct PulseScale : Behaviour {
    float speed = 3.0f;           ///< パルス速度
    float minScale = 0.5f;        ///< 最小スケール
    float maxScale = 1.5f;        ///< 最大スケール
    float time = 0.0f;            ///< 経過時間（内部管理）
    
    /**
     * @brief 毎フレーム更新処理
     * @param[in,out] w ワールド参照
     * @param[in] self 自身のエンティティ
     * @param[in] dt デルタタイム（秒）
     */
    void OnUpdate(World& w, Entity self, float dt) override {
        time += dt * speed;
        
        auto* t = w.TryGet<Transform>(self);
        if (!t) return;
        
        // sin波でスケールを変化
        float scale = minScale + (maxScale - minScale) * (sinf(time) * 0.5f + 0.5f);
        t->scale = DirectX::XMFLOAT3{ scale, scale, scale };
    }
};

/**
 * @struct ColorCycle
 * @brief 色を変化（サイクル）Behaviour
 * 
 * @details
 * 時間経過で色相を変化させ、虹色にサイクルします。
 * 
 * @author 山内陽
 */
struct ColorCycle : Behaviour {
    float speed = 1.0f;  ///< 色変化の速度
    float time = 0.0f;   ///< 経過時間（内部管理）
    
    /**
     * @brief 毎フレーム更新処理
     * @param[in,out] w ワールド参照
     * @param[in] self 自身のエンティティ
     * @param[in] dt デルタタイム（秒）
     */
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
// カテゴリ3: 複雑なBehaviour（複数の機能）
// ========================================================
/**
 * @brief 複雑なBehaviour
 * @details 複数のコンポーネントを組み合わせる
 * 学習ポイント：コンポーネント間の連携
 */

/**
 * @struct DestroyOnDeath
 * @brief 体力が0になったら削除するBehaviour
 * 
 * @details
 * Healthコンポーネントを監視し、体力が0以下になったら
 * エンティティを削除します。
 * 
 * @author 山内陽
 */
struct DestroyOnDeath : Behaviour {
    /**
     * @brief 毎フレーム更新処理
     * @param[in,out] w ワールド参照
     * @param[in] self 自身のエンティティ
     * @param[in] dt デルタタイム（秒）
     */
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

/**
 * @struct RandomWalk
 * @brief ランダムに動き回るBehaviour
 * 
 * @details
 * 一定時間ごとにランダムな方向を選び、その方向に移動します。
 * 範囲外に出ないように制限されます。
 * 
 * @author 山内陽
 */
struct RandomWalk : Behaviour {
    float speed = 2.0f;             ///< 移動速度
    float changeInterval = 2.0f;    ///< 方向転換の間隔（秒）
    float timer = 0.0f;             ///< タイマー（内部管理）
    DirectX::XMFLOAT3 direction{ 1.0f, 0.0f, 0.0f }; ///< 現在の方向
    
    /**
     * @brief 初期化処理
     * @param[in,out] w ワールド参照
     * @param[in] self 自身のエンティティ
     */
    void OnStart(World& w, Entity self) override {
        // 最初にランダムな方向を選ぶ
        ChooseRandomDirection();
    }
    
    /**
     * @brief 毎フレーム更新処理
     * @param[in,out] w ワールド参照
     * @param[in] self 自身のエンティティ
     * @param[in] dt デルタタイム（秒）
     */
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
    /**
     * @brief ランダムな方向を選択
     */
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
    
    /**
     * @brief 位置を範囲内に制限
     * @param[in,out] t Transform参照
     */
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

/**
 * @struct LifeTime
 * @brief 時間経過で削除するBehaviour
 * 
 * @details
 * 指定された時間が経過したらエンティティを削除します。
 * 一時的なエフェクトなどに使用します。
 * 
 * @author 山内陽
 */
struct LifeTime : Behaviour {
    float remainingTime = 5.0f; ///< 残り時間（秒）
    
    /**
     * @brief 毎フレーム更新処理
     * @param[in,out] w ワールド参照
     * @param[in] self 自身のエンティティ
     * @param[in] dt デルタタイム（秒）
     */
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
/**
 * @brief マクロを使った簡潔な定義
 * @details DEFINE_BEHAVIOURマクロで短く書ける
 * 学習ポイント：ボイラープレートの削減
 */

/**
 * @brief 回転しながら色を変える（マクロ版）
 */
DEFINE_BEHAVIOUR(SpinAndColor,
    float rotSpeed = 90.0f;    ///< 回転速度
    float colorSpeed = 1.0f;   ///< 色変化速度
    float time = 0.0f;         ///< 経過時間
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

/**
 * @brief 円運動を行う（マクロ版）
 */
DEFINE_BEHAVIOUR(CircularMotion,
    float radius = 3.0f;   ///< 円の半径
    float speed = 1.0f;    ///< 回転速度
    float angle = 0.0f;    ///< 現在の角度
    float centerY = 0.0f;  ///< 中心のY座標
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

// 例1: 上下に跳ねる赤いキューブ
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
