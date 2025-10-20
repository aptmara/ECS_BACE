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
 * コンポーネントの種類:
 * 1. データコンポーネント: データのみを保持（例: Transform, Health）
 * 2. Behaviourコンポーネント: 毎フレーム実行されるロジックを持つ（例: Rotator, PlayerMovement）
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
 * @par 使用例:
 * @code
 * struct Health : IComponent {
 *     float hp = 100.0f;
 *     float maxHp = 100.0f;
 * };
 * @endcode
 * 
 * @author 山内陽
 */
struct IComponent {
    /**
     * @brief 仮想デストラクタ
     * @details ポリモーフィズムを実現します
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
 * 用途:
 * - オブジェクトの移動（位置、回転、スケール）
 * - 時間による変化
 * - アニメーション再生
 * - ゲームロジックの実行
 * 
 * ライフサイクル:
 * 1. OnStart() - エンティティ作成後に1度だけ呼ばれる（初期化用）
 * 2. OnUpdate() - 毎フレーム呼ばれる（dt = 前フレームからの経過時間）
 * 
 * @par 使用例:
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
 * @see IComponent
 * @see World
 * 
 * @author 山内陽
 */
struct Behaviour : IComponent {
    /**
     * @brief エンティティ作成後に1度だけ呼ばれる初期化メソッド
     * 
     * @param[in] w Worldへの参照（コンポーネント取得などに使用）
     * @param[in] self このBehaviourが付いているエンティティ
     * 
     * @details
     * 初期化処理が必要な場合にオーバーライドします
     * （例: 初期位置の設定、初期状態の計算など）。
     * デフォルト実装は何もしません。
     */
    virtual void OnStart(World& w, Entity self) {}
    
    /**
     * @brief 毎フレーム呼ばれる更新メソッド
     * 
     * @param[in,out] w Worldへの参照（コンポーネント取得などに使用）
     * @param[in] self このBehaviourが付いているエンティティ
     * @param[in] dt デルタタイム（前フレームからの経過秒数）
     * 
     * @details
     * ゲームロジックを実装する際にオーバーライドします。
     * dtを使用することで、フレームレートに依存しない処理を実現できます。
     * デフォルト実装は何もしません。
     * 
     * @par 使用例:
     * @code
     * struct MoveForward : Behaviour {
     *     float speed = 5.0f;
     *     
     *     void OnUpdate(World& w, Entity self, float dt) override {
     *         auto* transform = w.TryGet<Transform>(self);
     *         if (transform) {
     *             transform->position.z += speed * dt;
     *         }
     *     }
     * };
     * @endcode
     */
    virtual void OnUpdate(World& w, Entity self, float dt) {}
};

/**
 * @def DEFINE_DATA_COMPONENT
 * @brief データコンポーネントを簡単に定義するマクロ
 * 
 * @param ComponentName コンポーネント名
 * @param ... メンバ変数の定義（複数可）
 * 
 * @details
 * ボイラープレートコードなしで、データのみを持つコンポーネントを1行で定義できます。
 * 
 * @par 使用例:
 * @code
 * DEFINE_DATA_COMPONENT(Health,
 *     float hp = 100.0f;
 *     float maxHp = 100.0f;
 * )
 * 
 * DEFINE_DATA_COMPONENT(Velocity,
 *     DirectX::XMFLOAT3 velocity{0, 0, 0};
 * )
 * 
 * DEFINE_DATA_COMPONENT(Player, )  // タグコンポーネント（データなし）
 * @endcode
 * 
 * @note セミコロンは不要です（マクロ内で自動的に追加されます）
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
 * 
 * @warning DataMembersとUpdateCodeの間にはカンマが必要です
 * 
 * @par 使用例:
 * @code
 * DEFINE_BEHAVIOUR(Bouncer,
 *     float speed = 1.0f;
 *     float time = 0.0f;
 * ,
 *     time += dt * speed;
 *     auto* t = w.TryGet<Transform>(self);
 *     if (t) t->position.y = sinf(time);
 * )
 * 
 * DEFINE_BEHAVIOUR(MoveForward,
 *     float speed = 5.0f;
 * ,
 *     auto* t = w.TryGet<Transform>(self);
 *     if (t) t->position.z += speed * dt;
 * )
 * @endcode
 * 
 * @note OnStart()は定義できません。必要な場合は通常の継承を使用してください。
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
