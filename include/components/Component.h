#pragma once

/**
 * @file Component.h
 * @brief ECSコンポーネントシステムの基底クラスとマクロ定義
 * @author 山内陽
 * @date 2025
 * @version 5.0
 * 
 * @details
 * ECSアーキテクチャにおけるコンポーネントの基底クラスと、
 * コンポーネントを簡単に定義するためのマクロを提供します。
 * 
 * ### コンポーネントの種類:
 * 1. **データコンポーネント**: データのみを保持(例: Transform, Health)
 * 2. **Behaviourコンポーネント**: 毎フレーム実行されるロジックを持つ(例: Rotator, PlayerMovement)
 * 
 * ### ECSの基本概念:
 * - **Entity(エンティティ)**: ゲームオブジェクトの識別子
 * - **Component(コンポーネント)**: データや機能の塊
 * - **System(システム)**: コンポーネントを処理するロジック
 * 
 * @par データコンポーネントの定義例
 * @code
 * // 方法1: IComponentを継承
 * struct Health : IComponent {
 *     float hp = 100.0f;
 *     float maxHp = 100.0f;
 * };
 * 
 * // 方法2: マクロを使用(推奨)
 * DEFINE_DATA_COMPONENT(Health,
 *     float hp = 100.0f;
 *     float maxHp = 100.0f;
 * )
 * @endcode
 * 
 * @par Behaviourコンポーネントの定義例
 * @code
 * // 方法1: Behaviourを継承
 * struct Rotator : Behaviour {
 *     float speed = 45.0f;
 *     
 *     void OnUpdate(World& w, Entity self, float dt) override {
 *         auto* t = w.TryGet<Transform>(self);
 *         if (t) t->rotation.y += speed * dt;
 *     }
 * };
 * 
 * // 方法2: マクロを使用(推奨)
 * DEFINE_BEHAVIOUR(Rotator,
 *     float speed = 45.0f;
 * ,
 *     auto* t = w.TryGet<Transform>(self);
 *     if (t) t->rotation.y += speed * dt;
 * )
 * @endcode
 * 
 * @author 山内陽
 */

class World;   ///< 前方宣言
struct Entity; ///< 前方宣言

/**
 * @interface IComponent
 * @brief すべてのコンポーネントの基底インターフェース
 * 
 * @details
 * 型情報管理のための共通基底クラスです。
 * すべてのコンポーネントはこのクラスを継承してWorldによる管理を可能にします。
 * 
 * ### 継承関係:
 * - IComponent (この基底クラス)
 *   - データコンポーネント (例: Transform, Health, Velocity)
 *   - Behaviour (更新処理を持つコンポーネント)
 *     - ロジックコンポーネント (例: Rotator, MoveForward, PlayerController)
 * 
 * @par 使用例(データコンポーネント)
 * @code
 * struct Health : IComponent {
 *     float hp = 100.0f;
 *     float maxHp = 100.0f;
 *     
 *     void TakeDamage(float damage) {
 *         hp -= damage;
 *         if (hp < 0.0f) hp = 0.0f;
 *     }
 * };
 * @endcode
 * 
 * @par 使用例(タグコンポーネント)
 * @code
 * // データを持たず、識別のためだけに使用
 * struct PlayerTag : IComponent {};
 * struct EnemyTag : IComponent {};
 * @endcode
 * 
 * @note このクラスは直接使用せず、継承して使用します
 * @see Behaviour
 * @see DEFINE_DATA_COMPONENT
 * 
 * @author 山内陽
 */
struct IComponent {
    /**
     * @brief 仮想デストラクタ
     * @details ポリモーフィズムを実現します。
     */
    virtual ~IComponent() = default;
};

/**
 * @class Behaviour
 * @brief 毎フレーム更新される動的コンポーネントの基底クラス
 * 
 * @details
 * このクラスを継承することで、ゲームロジックを持つコンポーネントを作成できます。
 * OnUpdate()メソッドが毎フレーム自動的に呼び出されます。
 * 
 * ### 用途:
 * - オブジェクトの移動(位置、回転、スケール)
 * - 時間による変化
 * - アニメーション再生
 * - ゲームロジックの実装
 * 
 * ### ライフサイクル:
 * 1. **OnStart()** - エンティティ作成後に1度だけ呼ばれる(初期化用)
 * 2. **OnUpdate()** - 毎フレーム呼ばれる(dt = 前フレームからの経過時間)
 * 
 * ### デルタタイム(dt)の重要性:
 * dtを使用することで、フレームレートに依存しない一定した動きを実現できます。
 * 
 * @par 使用例(基本的な回転)
 * @code
 * struct Rotator : Behaviour {
 *     float speed = 45.0f;  // 毎秒45度回転
 *     
 *     void OnUpdate(World& w, Entity self, float dt) override {
 *         auto* transform = w.TryGet<Transform>(self);
 *         if (transform) {
 *             transform->rotation.y += speed * dt;
 *         }
 *     }
 * };
 * @endcode
 * 
 * @par 使用例(初期化が必要な場合)
 * @code
 * struct Bouncer : Behaviour {
 *     float startY = 0.0f;
 *     float time = 0.0f;
 *     
 *     void OnStart(World& w, Entity self) override {
 *         auto* t = w.TryGet<Transform>(self);
 *         if (t) startY = t->position.y;  // 初期位置を記録
 *     }
 *     
 *     void OnUpdate(World& w, Entity self, float dt) override {
 *         time += dt;
 *         auto* t = w.TryGet<Transform>(self);
 *         if (t) t->position.y = startY + sinf(time) * 2.0f;
 *     }
 * };
 * @endcode
 * 
 * @par 使用例(複数コンポーネントの連携)
 * @code
 * struct DestroyOnDeath : Behaviour {
 *     void OnUpdate(World& w, Entity self, float dt) override {
 *         auto* health = w.TryGet<Health>(self);
 *         if (health && health->hp <= 0.0f) {
 *             w.DestroyEntity(self);  // 体力が0なら削除
 *         }
 *     }
 * };
 * @endcode
 * 
 * @note OnStart()とOnUpdate()は必要に応じてオーバーライドします
 * @see IComponent
 * @see World
 * @see DEFINE_BEHAVIOUR
 * 
 * @author 山内陽
 */
struct Behaviour : IComponent {
    /**
     * @brief エンティティ作成後に1度だけ呼ばれる初期化メソッド
     * 
     * @param[in] w Worldへの参照(コンポーネント取得などに使用)
     * @param[in] self このBehaviourが付いているエンティティ
     * 
     * @details
     * 初期化処理が必要な場合にオーバーライドします。
     * (例: 初期位置の設定、初期状態の記録など)
     * デフォルト実装は何もしません。
     * 
     * @par 使用例
     * @code
     * void OnStart(World& w, Entity self) override {
     *     auto* t = w.TryGet<Transform>(self);
     *     if (t) {
     *         startPosition = t->position;  // 初期位置を記録
     *     }
     * }
     * @endcode
     * 
     * @note このメソッドはWorld::Tick()の最初の呼び出し時に実行されます
     */
    virtual void OnStart(World& w, Entity self) {}
    
    /**
     * @brief 毎フレーム呼ばれる更新メソッド
     * 
     * @param[in,out] w Worldへの参照(コンポーネント取得などに使用)
     * @param[in] self このBehaviourが付いているエンティティ
     * @param[in] dt デルタタイム(前フレームからの経過時間)
     * 
     * @details
     * ゲームロジックを実装する際にオーバーライドします。
     * dtを使用することで、フレームレートに依存しない動きを実現できます。
     * デフォルト実装は何もしません。
     * 
     * ### デルタタイムの計算例:
     * @code
     * // 60FPSの場合: dt ≈ 0.0167秒
     * // 30FPSの場合: dt ≈ 0.0333秒
     * 
     * // dtを使うことでフレームレートに依存しない
     * position.x += speed * dt;  // 常に毎秒speed単位移動
     * @endcode
     * 
     * @par 使用例(前方移動)
     * @code
     * void OnUpdate(World& w, Entity self, float dt) override {
     *     auto* transform = w.TryGet<Transform>(self);
     *     if (transform) {
     *         transform->position.z += 5.0f * dt;  // 毎秒5単位前進
     *     }
     * }
     * @endcode
     * 
     * @par 使用例(色の変化)
     * @code
     * void OnUpdate(World& w, Entity self, float dt) override {
     *     time += dt;
     *     auto* renderer = w.TryGet<MeshRenderer>(self);
     *     if (renderer) {
     *         renderer->color.x = sinf(time) * 0.5f + 0.5f;
     *     }
     * }
     * @endcode
     * 
     * @note このメソッドはWorld::Tick()によって毎フレーム呼び出されます
     */
    virtual void OnUpdate(World& w, Entity self, float dt) {}
};

/**
 * @def DEFINE_DATA_COMPONENT
 * @brief データコンポーネントを簡単に定義するマクロ
 * 
 * @param ComponentName コンポーネント名
 * @param ... メンバ変数の定義(複数可)
 * 
 * @details
 * ボイラープレートコードなしで、データのみを持つコンポーネントを1行で定義できます。
 * IComponentを自動的に継承します。
 * 
 * ### 使い方:
 * 1. マクロ名の後にコンポーネント名を書く
 * 2. カッコ内にメンバ変数を書く(通常のC++構文)
 * 3. セミコロンは不要(マクロ内で自動追加)
 * 
 * @par 使用例(基本)
 * @code
 * DEFINE_DATA_COMPONENT(Health,
 *     float hp = 100.0f;
 *     float maxHp = 100.0f;
 * )
 * 
 * DEFINE_DATA_COMPONENT(Velocity,
 *     DirectX::XMFLOAT3 velocity{0, 0, 0};
 * )
 * @endcode
 * 
 * @par 使用例(メソッド付き)
 * @code
 * DEFINE_DATA_COMPONENT(Health,
 *     float hp = 100.0f;
 *     float maxHp = 100.0f;
 *     
 *     void TakeDamage(float damage) {
 *         hp -= damage;
 *         if (hp < 0.0f) hp = 0.0f;
 *     }
 *     
 *     bool IsDead() const {
 *         return hp <= 0.0f;
 *     }
 * )
 * @endcode
 * 
 * @par 使用例(タグコンポーネント)
 * @code
 * DEFINE_DATA_COMPONENT(Player, )  // データなし
 * DEFINE_DATA_COMPONENT(Enemy, )   // タグとして使用
 * @endcode
 * 
 * @note セミコロンは不要です(マクロ内で自動的に追加されます)
 * @warning メンバ変数が空の場合でも、カンマとカッコは必要です
 * 
 * @author 山内陽
 */
#define DEFINE_DATA_COMPONENT(ComponentName, ...) \
    struct ComponentName : IComponent { \
        __VA_ARGS__ \
    }

/**
 * @def DEFINE_BEHAVIOUR
 * @brief Behaviourコンポーネントを簡単に定義するマクロ
 * 
 * @param BehaviourName コンポーネント名
 * @param DataMembers メンバ変数の定義
 * @param UpdateCode OnUpdate()内で実行するコード
 * 
 * @details
 * OnUpdate()の実装を直接書くことで、動的なコンポーネントを簡潔に定義できます。
 * Behaviourを自動的に継承し、OnUpdate()メソッドを生成します。
 * 
 * ### 使い方:
 * 1. マクロ名の後にコンポーネント名を書く
 * 2. 第1引数: メンバ変数の定義(通常のC++構文)
 * 3. カンマで区切る(重要!)
 * 4. 第2引数: OnUpdate()の中身のコード
 * 
 * @par 使用例(基本的な回転)
 * @code
 * DEFINE_BEHAVIOUR(Rotator,
 *     float speed = 45.0f;
 * ,
 *     auto* t = w.TryGet<Transform>(self);
 *     if (t) t->rotation.y += speed * dt;
 * )
 * @endcode
 * 
 * @par 使用例(上下に跳ねる)
 * @code
 * DEFINE_BEHAVIOUR(Bouncer,
 *     float speed = 1.0f;
 *     float time = 0.0f;
 * ,
 *     time += dt * speed;
 *     auto* t = w.TryGet<Transform>(self);
 *     if (t) t->position.y = sinf(time);
 * )
 * @endcode
 * 
 * @par 使用例(前方移動)
 * @code
 * DEFINE_BEHAVIOUR(MoveForward,
 *     float speed = 5.0f;
 * ,
 *     auto* t = w.TryGet<Transform>(self);
 *     if (t) {
 *         t->position.z += speed * dt;
 *         if (t->position.z > 20.0f) {
 *             w.DestroyEntity(self);  // 遠くに行ったら削除
 *         }
 *     }
 * )
 * @endcode
 * 
 * @warning DataMembersとUpdateCodeの間には必ずカンマが必要です
 * @note OnStart()は定義できません。必要な場合は通常の継承を使用してください
 * @note UpdateCode内では w(World参照), self(Entity), dt(デルタタイム)が使用できます
 * 
 * @author 山内陽
 */
#define DEFINE_BEHAVIOUR(BehaviourName, DataMembers, UpdateCode) \
    struct BehaviourName : Behaviour { \
        DataMembers \
        void OnUpdate(World& w, Entity self, float dt) override { \
            UpdateCode \
        } \
    }
