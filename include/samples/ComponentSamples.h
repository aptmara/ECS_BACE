/**
 * @file ComponentSamples.h
 * @brief サンプルコンポーネント集
 * @author 山内陽
 * @date 2025
 * @version 4.0
 *
 * @details
 * このファイルは学習用のサンプルコンポーネントとBehaviourを定義します。
 * ECSアーキテクチャの使い方を学ぶための実例として活用してください。
 */
#pragma once

#include "components/Component.h"
#include "ecs/World.h"
#include "components/Transform.h"
#include "components/MeshRenderer.h"
#include <DirectXMath.h>
#include <cmath>
#include <cstdlib>
#include "util/Random.h"

// ========================================================
// サンプル集1: データコンポーネント
// ========================================================
/**
 * @brief データコンポーネント
 * @details ゲーム内のオブジェクトに関する情報を保持するコンポーネントです。
 * データのみを持ち、ロジックは持ちません(Behaviourとは異なります)。
 */

/**
 * @struct Health
 * @brief 体力管理コンポーネント
 *
 * @details
 * エンティティの現在の体力と最大体力を管理します。
 * ダメージを受けたり、回復したりする機能を提供します。
 *
 * @par 使用例
 * @code
 * Health hp;
 * hp.current = 50.0f;
 * hp.max = 100.0f;
 * world.Add<Health>(entity, hp);
 * 
 * // ダメージを受ける
 * auto* health = world.TryGet<Health>(entity);
 * if (health) {
 *     health->TakeDamage(10.0f);
 *     if (health->IsDead()) {
 *         // 死亡処理
 *     }
 * }
 * @endcode
 *
 * @author 山内陽
 */
struct Health : IComponent {
    float current = 100.0f;  ///< 現在の体力
    float max = 100.0f;      ///< 最大体力

    /**
     * @brief ダメージを受ける
     * @param[in] damage ダメージ量
     * 
     * @details
     * 体力を減少させます。0未満にはなりません。
     */
    void TakeDamage(float damage) {
        current -= damage;
        if (current < 0.0f) current = 0.0f;
    }

    /**
     * @brief 回復する
     * @param[in] amount 回復量
     * 
     * @details
     * 体力を増加させます。最大値を超えることはありません。
     */
    void Heal(float amount) {
        current += amount;
        if (current > max) current = max;
    }

    /**
     * @brief 死亡しているか確認
     * @return true 死亡している, false 生存している
     */
    bool IsDead() const {
        return current <= 0.0f;
    }
};

/**
 * @struct Velocity
 * @brief 速度ベクトルコンポーネント
 *
 * @details
 * エンティティの移動速度を保持します。
 * 物理演算や移動処理に使用します。
 *
 * @author 山内陽
 */
struct Velocity : IComponent {
    DirectX::XMFLOAT3 velocity{ 0.0f, 0.0f, 0.0f };  ///< 速度ベクトル

    /**
     * @brief 速度を加算
     * @param[in] x X軸方向の速度
     * @param[in] y Y軸方向の速度
     * @param[in] z Z軸方向の速度
     */
    void AddVelocity(float x, float y, float z) {
        velocity.x += x;
        velocity.y += y;
        velocity.z += z;
    }
};

/**
 * @brief スコア管理コンポーネント
 * 
 * @details
 * ゲームのスコア(得点)を保持します。
 */
DEFINE_DATA_COMPONENT(Score,
    int points = 0;  ///< 現在のスコア

    /**
     * @brief ポイントを加算
     * @param[in] p 加算するスコア
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
 * @brief 名前コンポーネント
 * 
 * @details
 * エンティティの名前を保持します。
 * デバッグ表示などに使用します。
 */
DEFINE_DATA_COMPONENT(Name,
    const char* name = "Unnamed";  ///< エンティティの名前
);

/**
 * @brief タグコンポーネント
 * @details エンティティの種類を識別するためのマーカーです。
 * データは持たず、エンティティが特定の種類であることを示すために使用します。
 */
struct PlayerTag : IComponent {};  ///< プレイヤータグ
struct EnemyTag : IComponent {};   ///< 敵タグ
struct BulletTag : IComponent {};  ///< 弾丸タグ

// ========================================================
// サンプル集2: シンプルなBehaviour
// ========================================================
/**
 * @brief シンプルなBehaviour
 * @details 1つのシンプルな動きを実装します。
 * 学習用として、OnUpdateの書き方を学べます。
 */

/**
 * @struct Bouncer
 * @brief 上下に跳ねる動きを持つBehaviour
 *
 * @details
 * sin関数を使って滑らかに上下へ跳ねる動きを実現します。
 * OnStartで初期位置を記録し、OnUpdateで現在位置を更新します。
 *
 * @par 使用例
 * @code
 * Entity cube = world.Create()
 *     .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
 *     .With<MeshRenderer>(DirectX::XMFLOAT3{0, 1, 0})
 *     .With<Bouncer>()
 *     .Build();
 * @endcode
 *
 * @author 山内陽
 */
struct Bouncer : Behaviour {
    float speed = 2.0f;      ///< 跳ねる速度
    float amplitude = 2.0f;  ///< 振幅(跳ねる高さ)
    float time = 0.0f;       ///< 内部時間(触らなくてOK)
    float startY = 0.0f;     ///< 開始時のY座標(内部使用)

    /**
     * @brief 初回起動時の処理
     * @param[in,out] w ワールド参照
     * @param[in] self このコンポーネントが付いているエンティティ
     */
    void OnStart(World& w, Entity self) override {
        // 最初の実行時に現在位置を記録
        auto* t = w.TryGet<Transform>(self);
        if (t) {
            startY = t->position.y; // 開始時のY座標を記録
        }
    }

    /**
     * @brief 毎フレーム更新処理
     * @param[in,out] w ワールド参照
     * @param[in] self このコンポーネントが付いているエンティティ
     * @param[in] dt デルタタイム(前フレームからの経過時間)
     */
    void OnUpdate(World& w, Entity self, float dt) override {
        // 毎フレーム時間を進める
        time += dt * speed;

        auto* t = w.TryGet<Transform>(self);
        if (t) {
            // sin関数で上下に跳ねる
            t->position.y = startY + sinf(time) * amplitude;
        }
    }
};

/**
 * @struct MoveForward
 * @brief 前方に移動するBehaviour
 *
 * @details
 * Z軸に沿って前方向に移動します。画面奥へ一定速度で進みます。
 * 一定範囲を超えたら自動的に削除されます。
 *
 * @author 山内陽
 */
struct MoveForward : Behaviour {
    float speed = 2.0f; ///< 移動速度(単位/秒)

    /**
     * @brief 毎フレーム更新処理
     * @param[in,out] w ワールド参照
     * @param[in] self このコンポーネントが付いているエンティティ
     * @param[in] dt デルタタイム(前フレームからの経過時間)
     */
    void OnUpdate(World& w, Entity self, float dt) override {
        auto* t = w.TryGet<Transform>(self);
        if (!t) return;

        // Z軸に沿って前方向に移動
        t->position.z += speed * dt;

        // 遠くまで行ったら自動削除(メモリリーク防止) - 原因付き
        if (t->position.z > 20.0f) {
            w.DestroyEntityWithCause(self, World::Cause::LifetimeExpired);
        }
    }
};

/**
 * @struct PulseScale
 * @brief 大きさが脈打つように変化するBehaviour
 *
 * @details
 * sin関数を使ってエンティティのスケールを周期的に変化させ、脈打つような視覚効果を実現します。
 *
 * @author 山内陽
 */
struct PulseScale : Behaviour {
    float speed = 3.0f;           ///< 脈打つ速度
    float minScale = 0.5f;        ///< 最小スケール
    float maxScale = 1.5f;        ///< 最大スケール
    float time = 0.0f;            ///< 内部時間(触らなくてOK)

    /**
     * @brief 毎フレーム更新処理
     * @param[in,out] w ワールド参照
     * @param[in] self このコンポーネントが付いているエンティティ
     * @param[in] dt デルタタイム(前フレームからの経過時間)
     */
    void OnUpdate(World& w, Entity self, float dt) override {
        time += dt * speed;

        auto* t = w.TryGet<Transform>(self);
        if (!t) return;

        // sin関数でスケールを周期変化
        float scale = minScale + (maxScale - minScale) * (sinf(time) * 0.5f + 0.5f);
        t->scale = DirectX::XMFLOAT3{ scale, scale, scale };
    }
};

/**
 * @struct ColorCycle
 * @brief 色を周期的に変化させるBehaviour
 *
 * @details
 * 時間経過に応じて色相を周期的に変化させ、虹色にサイクルします。
 *
 * @author 山内陽
 */
struct ColorCycle : Behaviour {
    float speed = 1.0f;  ///< 色相変化の速度
    float time = 0.0f;   ///< 内部時間(触らなくてOK)

    /**
     * @brief デフォルトコンストラクタ
     */
    ColorCycle() = default;

    /**
     * @brief コンストラクタ
     * @param[in] s 色相変化の速度
     */
    ColorCycle(float s) : speed(s) {}

    /**
     * @brief 毎フレーム更新処理
     * @param[in,out] w ワールド参照
     * @param[in] self このコンポーネントが付いているエンティティ
     * @param[in] dt デルタタイム(前フレームからの経過時間)
     */
    void OnUpdate(World& w, Entity self, float dt) override {
        time += dt * speed;

        auto* mr = w.TryGet<MeshRenderer>(self);
        if (!mr) return;

        // HSV色空間で色相を周期変化(簡易版)
        float hue = fmodf(time, 1.0f);
        mr->color.x = sinf(hue * DirectX::XM_2PI) * 0.5f + 0.5f;
        mr->color.y = sinf((hue + 0.333f) * DirectX::XM_2PI) * 0.5f + 0.5f;
        mr->color.z = sinf((hue + 0.666f) * DirectX::XM_2PI) * 0.5f + 0.5f;
    }
};

// ========================================================
// サンプル集3: 複雑なBehaviour(組み合わせの例)
// ========================================================
/**
 * @brief 複雑なBehaviour
 * @details 複雑なゲームロジックを持つコンポーネントの実装例です。
 * 学習用として、コンポーネント間の連携方法を学べます。
 */

/**
 * @struct DestroyOnDeath
 * @brief 体力が0になったら自動削除するBehaviour
 *
 * @details
 * Healthコンポーネントを監視し、体力が0以下になったら
 * エンティティを自動削除します。
 *
 * @author 山内陽
 */
struct DestroyOnDeath : Behaviour {
    /**
     * @brief 毎フレーム更新処理
     * @param[in,out] w ワールド参照
     * @param[in] self このコンポーネントが付いているエンティティ
     * @param[in] dt デルタタイム(前フレームからの経過時間)
     */
    void OnUpdate(World& w, Entity self, float dt) override {
        // Healthコンポーネントを取得
        auto* health = w.TryGet<Health>(self);
        if (!health) return;

        // 体力が0以下なら自動削除
        if (health->IsDead()) {
            w.DestroyEntity(self);
        }
    }
};

/**
 * @struct RandomWalk
 * @brief ランダムに歩き回るBehaviour
 *
 * @details
 * 一定時間ごとにランダムな方向を選び、その方向へ移動します。
 * 一定範囲内に位置を制限します。
 *
 * @author 山内陽
 */
struct RandomWalk : Behaviour {
    float speed = 2.0f;             ///< 移動速度
    float changeInterval = 2.0f;    ///< 方向変更の間隔(秒)
    float timer = 0.0f;             ///< タイマー(内部使用)
    DirectX::XMFLOAT3 direction{ 1.0f, 0.0f, 0.0f }; ///< 現在の方向

    /**
     * @brief 初回起動時の処理
     * @param[in,out] w ワールド参照
     * @param[in] self このコンポーネントが付いているエンティティ
     */
    void OnStart(World& w, Entity self) override {
        // 最初の実行時にランダムな方向を選択
        ChooseRandomDirection();
    }

    /**
     * @brief 毎フレーム更新処理
     * @param[in,out] w ワールド参照
     * @param[in] self このコンポーネントが付いているエンティティ
     * @param[in] dt デルタタイム(前フレームからの経過時間)
     */
    void OnUpdate(World& w, Entity self, float dt) override {
        timer += dt;

        // 一定時間ごとに方向変更
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

        // 一定範囲内に制限
        ClampPosition(t);
    }

private:
    /**
     * @brief ランダムな方向を選択
     */
    void ChooseRandomDirection() {
        // 球面一様分布から単位ベクトルを取得
        direction = util::Random::UnitVec3();
    }

    /**
     * @brief 現在位置を一定範囲内に制限
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
 * @brief 一定時間経過後に自動削除するBehaviour
 *
 * @details
 * 指定した時間が経過したらエンティティを自動削除します。
 * 一時的なエフェクトや弾丸などに使用します。
 *
 * @author 山内陽
 */
struct LifeTime : Behaviour {
    float remainingTime = 5.0f; ///< 残り寿命(秒)

    /**
     * @brief 毎フレーム更新処理
     * @param[in,out] w ワールド参照
     * @param[in] self このコンポーネントが付いているエンティティ
     * @param[in] dt デルタタイム(前フレームからの経過時間)
     */
    void OnUpdate(World& w, Entity self, float dt) override {
        remainingTime -= dt;

        // 寿命が尽きたら削除（原因付き）
        if (remainingTime <= 0.0f) {
            w.DestroyEntityWithCause(self, World::Cause::LifetimeExpired);
        }
    }
};

// ========================================================
// サンプル集4: マクロを使った簡潔な定義
// ========================================================
/**
 * @brief マクロを使った簡潔な定義
 * @details DEFINE_BEHAVIOURマクロを使って簡潔に定義できます。
 * 学習用として、コード量を減らす方法を学べます。
 */

/**
 * @brief 回転しながら色を変化させるBehaviour
 * 
 * @details
 * 回転とカラーサイクルを組み合わせた複合Behaviourです。
 */
DEFINE_BEHAVIOUR(SpinAndColor,
    float rotSpeed = 90.0f;    ///< 回転速度
    float colorSpeed = 1.0f;   ///< 色相変化速度
    float time = 0.0f;         ///< 内部時間
,
    time += dt * colorSpeed;

    // 回転
    auto* t = w.TryGet<Transform>(self);
    if (t) {
        t->rotation.y += rotSpeed * dt;
    }

    // 色相変化
    auto* mr = w.TryGet<MeshRenderer>(self);
    if (mr) {
        float hue = fmodf(time, 1.0f);
        mr->color.x = sinf(hue * 6.28f) * 0.5f + 0.5f;
        mr->color.y = cosf(hue * 6.28f) * 0.5f + 0.5f;
        mr->color.z = 0.5f;
    }
);

/**
 * @brief 円を描いて移動するBehaviour
 * 
 * @details
 * 円軌道を描きながら移動します。
 */
DEFINE_BEHAVIOUR(CircularMotion,
    float radius = 3.0f;   ///< 円軌道の半径
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
// 作成者: 山内陽
// バージョン: v4.0 - サンプルコンポーネント集
// ========================================================
