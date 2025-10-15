#pragma once

/**
 * @file Component.h
 * @brief ECSコンポーネントシステムの基底クラスとマクロ定義
 * @author 山内陽
 * @date 2024
 * @version 5.0
 * 
 * @details
 * このファイルは、ECSアーキテクチャにおけるコンポーネントの基底クラスと、
 * コンポーネントを簡単に定義するためのマクロを提供します。
 * 
 * ### コンポーネントとは:
 * ゲームオブジェクトに付ける「部品」のこと。
 * 例: 「位置」「見た目」「動き」などを別々のコンポーネントとして管理
 * 
 * ### 2種類のコンポーネント:
 * 1. **データコンポーネント**: データのみを保持（例: Transform, Health）
 * 2. **Behaviourコンポーネント**: 毎フレーム実行される処理を持つ（例: Rotator, PlayerMovement）
 */

class World;   ///< 前方宣言: Worldクラス
struct Entity; ///< 前方宣言: Entity構造体

/**
 * @interface IComponent
 * @brief すべてのコンポーネントの基底インターフェース
 * 
 * @details
 * このクラスは、型情報を保持するための共通の親クラスです。
 * 実際のコンポーネントはこのクラスを継承して作成します。
 * 
 * @note 初学者は特に意識する必要はありません。
 *       継承することで、Worldがコンポーネントを管理できるようになります。
 * 
 * @par 使用例:
 * @code
 * // データコンポーネントの例
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
     * @details ポリモーフィズムを可能にするため、仮想デストラクタを定義
     */
    virtual ~IComponent() = default;
};

/**
 * @class Behaviour
 * @brief 毎フレーム更新される動的コンポーネントの基底クラス
 * 
 * @details
 * このクラスを継承することで、ゲームロジックを持つコンポーネントを
 * 作成できます。OnUpdate()メソッドが毎フレーム自動的に呼ばれます。
 * 
 * ### 用途:
 * - オブジェクトを動かす（移動、回転、拡大縮小）
 * - 時間経過で何かを変化させる
 * - アニメーションを再生する
 * - ゲームロジックを実行する
 * 
 * ### ライフサイクル:
 * 1. OnStart() - エンティティが作成された直後に1度だけ呼ばれる（初期化用）
 * 2. OnUpdate() - 毎フレーム呼ばれる（dt = 前フレームからの経過時間）
 * 
 * @par 使用例:
 * @code
 * // 自動回転コンポーネント
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
 * @see IComponent 基底インターフェース
 * @see World コンポーネントを管理するクラス
 * 
 * @author 山内陽
 */
struct Behaviour : IComponent {
    /**
     * @brief エンティティ作成直後に1度だけ呼ばれる初期化メソッド
     * 
     * @param[in] w ワールドへの参照（コンポーネント取得などに使用）
     * @param[in] self このBehaviourが付いているエンティティ
     * 
     * @details
     * 初期化処理が必要な場合にオーバーライドします。
     * 例: 初期位置の設定、初期状態の計算など
     * 
     * @note デフォルト実装は何もしません
     * 
     * @par 使用例:
     * @code
     * struct MyBehaviour : Behaviour {
     *     void OnStart(World& w, Entity self) override {
     *         // 初期化処理
     *         auto* transform = w.TryGet<Transform>(self);
     *         if (transform) {
     *             transform->position.y = 5.0f;  // 初期位置を設定
     *         }
     *     }
     * };
     * @endcode
     */
    virtual void OnStart(World& w, Entity self) {}
    
    /**
     * @brief 毎フレーム呼ばれる更新メソッド
     * 
     * @param[in,out] w ワールドへの参照（コンポーネント取得などに使用）
     * @param[in] self このBehaviourが付いているエンティティ
     * @param[in] dt デルタタイム（前フレームからの経過秒数）
     * 
     * @details
     * ゲームロジックを実装する際にオーバーライドします。
     * dtを使うことで、フレームレートに依存しない処理を実現できます。
     * 
     * @note デフォルト実装は何もしません
     * 
     * @par 使用例:
     * @code
     * struct MoveForward : Behaviour {
     *     float speed = 5.0f;
     *     
     *     void OnUpdate(World& w, Entity self, float dt) override {
     *         auto* transform = w.TryGet<Transform>(self);
     *         if (transform) {
     *             // dtを使ってフレームレート非依存な移動
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
 * @param ComponentName コンポーネントの名前
 * @param ... メンバ変数の定義（複数可）
 * 
 * @details
 * データのみを持つコンポーネントを1行で定義できます。
 * ボイラープレートコードを書かずに済むため、初学者でも簡単に使えます。
 * 
 * @par 使用例:
 * @code
 * // 体力コンポーネント
 * DEFINE_DATA_COMPONENT(Health,
 *     float hp = 100.0f;
 *     float maxHp = 100.0f;
 * )
 * 
 * // 速度コンポーネント
 * DEFINE_DATA_COMPONENT(Velocity,
 *     DirectX::XMFLOAT3 velocity{0, 0, 0};
 * )
 * 
 * // タグコンポーネント（データなし）
 * DEFINE_DATA_COMPONENT(Player, )
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
 * @param BehaviourName コンポーネントの名前
 * @param DataMembers メンバ変数の定義
 * @param UpdateCode OnUpdate()内で実行するコード
 * 
 * @details
 * 動的なコンポーネントを簡潔に定義できます。
 * OnUpdate()の実装を直接書けるため、コードが読みやすくなります。
 * 
 * @warning DataMembersとUpdateCodeの間にはカンマが必要です
 * 
 * @par 使用例:
 * @code
 * // 上下に揺れるコンポーネント
 * DEFINE_BEHAVIOUR(Bouncer,
 *     float speed = 1.0f;
 *     float time = 0.0f;
 * ,
 *     time += dt * speed;
 *     auto* t = w.TryGet<Transform>(self);
 *     if (t) t->position.y = sinf(time);
 * )
 * 
 * // 前進するコンポーネント
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
