#pragma once

// 前方宣言により依存関係を減らし、コンパイル速度を向上させる。
// World クラスは、ゲーム内のエンティティやコンポーネントを管理する「ゲームワールド」を表します。
class World;
// Entity 構造体は、ゲーム内のオブジェクトを一意に識別するための識別子を表します。
struct Entity;

// ECS（エンティティコンポーネントシステム）のすべてのコンポーネントの基底インターフェース。
// このインターフェースを継承することで、すべてのコンポーネントが統一的に扱われます。
struct IComponent {
 virtual ~IComponent() = default; ///< 派生クラスの適切なクリーンアップを保証するための仮想デストラクタ。
};

// ゲームループ中に実行されるロジックを持つコンポーネントの基底クラス。
// このクラスを継承することで、特定の振る舞いを持つコンポーネントを作成できます。
struct Behaviour : IComponent {
 // コンポーネントがエンティティに追加されたとき、またはゲーム開始時に呼び出される。
 // @param w: ゲームワールドへの参照。
 // @param self: このコンポーネントがアタッチされているエンティティ。
 virtual void OnStart(World& w, Entity self) {}

 // 毎フレーム呼び出され、コンポーネントのロジックを更新する。
 // @param w: ゲームワールドへの参照。
 // @param self: このコンポーネントがアタッチされているエンティティ。
 // @param dt: 前フレームからの経過時間（デルタタイム）。時間に基づく計算に使用。
 virtual void OnUpdate(World& w, Entity self, float dt) {}
};

// データ専用コンポーネントを定義するためのマクロ。
// データコンポーネントは状態を保持するが、振る舞いを持たない。
// @param ComponentName: コンポーネントの名前。
// @param ...: コンポーネントのデータメンバー。
#define DEFINE_DATA_COMPONENT(ComponentName, ...) \
 struct ComponentName : IComponent { \
 __VA_ARGS__ \
 };

// 振る舞いコンポーネントを定義するためのマクロ。
// 振る舞いコンポーネントはデータとロジックの両方を含む。
// @param BehaviourName: 振る舞いコンポーネントの名前。
// @param DataMembers: コンポーネントのデータメンバー。
// @param UpdateCode: OnUpdate メソッド内で実行されるロジック。
#define DEFINE_BEHAVIOUR(BehaviourName, DataMembers, UpdateCode) \
 struct BehaviourName : Behaviour { \
 DataMembers \
 void OnUpdate(World& w, Entity self, float dt) override { \
 UpdateCode \
 } \
 };
