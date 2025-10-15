#pragma once
#include "ecs/Entity.h"
#include "components/Component.h"
#include <unordered_map>
#include <typeindex>
#include <vector>
#include <functional>
#include <type_traits>
#include <cassert>

/**
 * @file World.h
 * @brief ECSワールド管理システムとエンティティビルダーの定義
 * @author 山内陽
 * @date 2024
 * @version 5.0
 * 
 * @details
 * このファイルは、ECSアーキテクチャの中核となるWorldクラスと、
 * エンティティを簡単に作成するためのEntityBuilderクラスを定義します。
 */

class World; ///< 前方宣言

/**
 * @class EntityBuilder
 * @brief エンティティ作成を簡単にするビルダーパターンクラス
 * 
 * @details
 * メソッドチェーンを使って、複数のコンポーネントを持つエンティティを
 * 簡潔に作成できます。Worldクラスと連携して動作します。
 * 
 * @par 使用例:
 * @code
 * Entity player = world.Create()
 *     .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
 *     .With<MeshRenderer>(DirectX::XMFLOAT3{0, 1, 0})  // 緑色
 *     .With<Rotator>(45.0f)
 *     .Build();
 * @endcode
 * 
 * @note Build()は省略可能です（暗黙的にEntityに変換されます）
 * @see World エンティティとコンポーネントを管理するクラス
 * 
 * @author 山内陽
 */
class EntityBuilder {
public:
    /**
     * @brief コンストラクタ
     * @param[in] world Worldクラスのポインタ
     * @param[in] entity 作成されたエンティティ
     */
    EntityBuilder(World* world, Entity entity) : world_(world), entity_(entity) {}
    
    /**
     * @brief コンポーネントを追加する（メソッドチェーン対応）
     * 
     * @tparam T 追加するコンポーネントの型
     * @tparam Args コンストラクタ引数の型（可変長）
     * @param[in] args コンポーネントのコンストラクタに渡す引数
     * @return EntityBuilder& メソッドチェーン用の自身への参照
     * 
     * @par 使用例:
     * @code
     * world.Create()
     *     .With<Transform>(pos, rot, scale)  // 3つの引数を渡す
     *     .With<MeshRenderer>(color)         // 1つの引数を渡す
     *     .With<Player>()                    // 引数なし
     *     .Build();
     * @endcode
     */
    template<typename T, typename... Args>
    EntityBuilder& With(Args&&... args);
    
    /**
     * @brief エンティティを確定して返す
     * @return Entity 作成されたエンティティ
     * 
     * @note この関数を呼ばなくても、暗黙的にEntityに変換されます
     */
    Entity Build() { return entity_; }
    
    /**
     * @brief Entityへの暗黙的型変換演算子
     * @return Entity 作成されたエンティティ
     * 
     * @details Build()を呼ばずに、直接Entity型の変数に代入できます
     * 
     * @par 使用例:
     * @code
     * // Build()なしで直接代入
     * Entity e = world.Create().With<Transform>();
     * @endcode
     */
    operator Entity() const { return entity_; }

private:
    World* world_;    ///< Worldクラスへのポインタ
    Entity entity_;   ///< 作成されたエンティティ
};

/**
 * @class World
 * @brief ECSワールド管理クラス - エンティティとコンポーネントのすべてを管理
 * 
 * @details
 * Worldクラスは、ゲーム世界の「管理者」です。
 * 以下の機能を提供します：
 * - エンティティの作成・削除
 * - コンポーネントの追加・削除・取得
 * - 全Behaviourコンポーネントの更新
 * 
 * ### 初学者向けガイド:
 * Worldは「ゲーム世界そのもの」と考えてください。
 * - CreateEntity() でゲームオブジェクトを作る
 * - Add<コンポーネント>() で機能を追加
 * - TryGet<コンポーネント>() で機能を取得
 * - Tick() で全オブジェクトを更新
 * 
 * @par 基本的な使い方:
 * @code
 * World world;
 * 
 * // エンティティ作成
 * Entity player = world.CreateEntity();
 * 
 * // コンポーネント追加
 * world.Add<Transform>(player, Transform{...});
 * world.Add<MeshRenderer>(player, MeshRenderer{...});
 * 
 * // コンポーネント取得
 * auto* transform = world.TryGet<Transform>(player);
 * if (transform) {
 *     transform->position.x += 1.0f;
 * }
 * 
 * // 毎フレーム更新
 * world.Tick(deltaTime);
 * @endcode
 * 
 * @par ビルダーパターン（推奨）:
 * @code
 * Entity player = world.Create()
 *     .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
 *     .With<MeshRenderer>(DirectX::XMFLOAT3{0, 1, 0})
 *     .With<Rotator>(45.0f)
 *     .Build();
 * @endcode
 * 
 * @see Entity エンティティ構造体
 * @see IComponent コンポーネント基底クラス
 * @see Behaviour 動的コンポーネント基底クラス
 * 
 * @author 山内陽
 */
class World {
public:
    /**
     * @brief エンティティを作成する（基本版）
     * @return Entity 新しく作成されたエンティティ
     * 
     * @details
     * 一意なIDを持つ新しいエンティティを作成します。
     * この段階ではコンポーネントは何も付いていません。
     * 
     * @note 通常はCreate()（ビルダー版）を使う方が便利です
     * 
     * @par 使用例:
     * @code
     * Entity e = world.CreateEntity();
     * world.Add<Transform>(e, Transform{...});
     * world.Add<MeshRenderer>(e, MeshRenderer{...});
     * @endcode
     */
    Entity CreateEntity() {
        Entity e{ ++nextId_ };
        alive_[e.id] = true;
        return e;
    }
    
    /**
     * @brief エンティティを作成する（ビルダー版）- おすすめ！
     * @return EntityBuilder ビルダーオブジェクト
     * 
     * @details
     * メソッドチェーンでコンポーネントを追加できる便利な方法です。
     * 初学者にも読みやすく、おすすめの方法です。
     * 
     * @par 使用例:
     * @code
     * Entity player = world.Create()
     *     .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
     *     .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
     *     .With<Rotator>(45.0f)
     *     .Build();
     * @endcode
     * 
     * @see EntityBuilder ビルダークラス
     */
    EntityBuilder Create() {
        return EntityBuilder(this, CreateEntity());
    }

    /**
     * @brief エンティティが生存しているか確認
     * 
     * @param[in] e 確認するエンティティ
     * @return true 生存している, false 削除済み
     * 
     * @details
     * エンティティがDestroyEntity()で削除されていないかチェックします。
     * 
     * @par 使用例:
     * @code
     * if (world.IsAlive(entity)) {
     *     // エンティティが有効な場合の処理
     * }
     * @endcode
     */
    bool IsAlive(Entity e) const {
        auto it = alive_.find(e.id);
        return it != alive_.end() && it->second;
    }

    /**
     * @brief エンティティを削除する
     * 
     * @param[in] e 削除するエンティティ
     * 
     * @details
     * エンティティとそれに付いているすべてのコンポーネントを削除します。
     * 削除後、そのエンティティを使用してはいけません。
     * 
     * @warning 削除されたエンティティを使用するとクラッシュする可能性があります
     * 
     * @par 使用例:
     * @code
     * // 敵が倒されたら削除
     * auto* health = world.TryGet<Health>(enemy);
     * if (health && health->hp <= 0) {
     *     world.DestroyEntity(enemy);
     * }
     * @endcode
     */
    void DestroyEntity(Entity e) {
        if (!IsAlive(e)) return;
        alive_[e.id] = false;
        for (auto& er : erasers_) er(e);
        for (size_t i = 0; i < behaviours_.size(); ++i)
            if (behaviours_[i].e.id == e.id) { behaviours_.erase(behaviours_.begin() + i); --i; }
    }

    /**
     * @brief コンポーネントを追加する
     * 
     * @tparam T 追加するコンポーネントの型
     * @tparam Args コンストラクタ引数の型（可変長）
     * @param[in] e 対象エンティティ
     * @param[in] args コンポーネントのコンストラクタ引数
     * @return T& 追加されたコンポーネントへの参照
     * 
     * @details
     * 指定したエンティティにコンポーネントを追加します。
     * Behaviourコンポーネントの場合、自動的にTick()で更新されます。
     * 
     * @note エンティティは生存している必要があります
     * 
     * @par 使用例:
     * @code
     * Entity e = world.CreateEntity();
     * world.Add<Transform>(e, Transform{...});
     * world.Add<MeshRenderer>(e, MeshRenderer{DirectX::XMFLOAT3{1, 0, 0}});
     * world.Add<Rotator>(e, Rotator{45.0f});
     * @endcode
     */
    template<class T, class...Args>
    T& Add(Entity e, Args&&...args) {
        assert(IsAlive(e) && "Add on dead entity");
        auto& s = getStore<T>();
        T& obj = s.map[e.id] = T{ std::forward<Args>(args)... };
        registerBehaviour<T>(e, &obj);
        return obj;
    }

    /**
     * @brief コンポーネントを取得する（nullptrの可能性あり）
     * 
     * @tparam T 取得するコンポーネントの型
     * @param[in] e 対象エンティティ
     * @return T* コンポーネントへのポインタ（存在しない場合はnullptr）
     * 
     * @details
     * 指定したエンティティからコンポーネントを取得します。
     * コンポーネントが存在しない場合はnullptrを返すため、必ずチェックが必要です。
     * 
     * @warning 必ずnullptrチェックを行ってください
     * 
     * @par 使用例:
     * @code
     * auto* transform = world.TryGet<Transform>(entity);
     * if (transform) {
     *     transform->position.x += 1.0f;  // 安全に使用
     * }
     * @endcode
     */
    template<class T>
    T* TryGet(Entity e) {
        auto itS = stores_.find(std::type_index(typeid(T)));
        if (itS == stores_.end()) return nullptr;
        auto* s = static_cast<Store<T>*>(itS->second);
        auto it = s->map.find(e.id);
        if (it == s->map.end()) return nullptr;
        return &it->second;
    }

    /**
     * @brief コンポーネントを削除する
     * 
     * @tparam T 削除するコンポーネントの型
     * @param[in] e 対象エンティティ
     * @return true 削除成功, false コンポーネントが存在しなかった
     * 
     * @par 使用例:
     * @code
     * world.Remove<Rotator>(entity);  // 回転を止める
     * @endcode
     */
    template<class T>
    bool Remove(Entity e) {
        auto itS = stores_.find(std::type_index(typeid(T)));
        if (itS == stores_.end()) return false;
        auto* s = static_cast<Store<T>*>(itS->second);
        unregisterBehaviour<T>(e);
        return s->map.erase(e.id) > 0;
    }

    /**
     * @brief 全コンポーネントに対して処理を実行
     * 
     * @tparam T 対象コンポーネントの型
     * @tparam F ラムダ関数の型
     * @param[in] fn 実行する関数（Entity, T&を受け取る）
     * 
     * @details
     * 指定した型のコンポーネントを持つ全エンティティに対して処理を実行します。
     * 
     * @par 使用例:
     * @code
     * // 全オブジェクトを少しずつ上に移動
     * world.ForEach<Transform>([](Entity e, Transform& t) {
     *     t.position.y += 0.01f;
     * });
     * 
     * // 全敵の体力を減らす
     * world.ForEach<Health>([](Entity e, Health& h) {
     *     h.hp -= 10.0f;
     * });
     * @endcode
     */
    template<class T, class F>
    void ForEach(F&& fn) {
        auto itS = stores_.find(std::type_index(typeid(T)));
        if (itS == stores_.end()) return;
        auto* s = static_cast<Store<T>*>(itS->second);
        for (auto it = s->map.begin(); it != s->map.end(); ++it) {
            Entity e{ it->first };
            if (!IsAlive(e)) continue;
            fn(e, it->second);
        }
    }

    /**
     * @brief 全Behaviourの更新（毎フレーム呼ぶ）
     * 
     * @param[in] dt デルタタイム（前フレームからの経過秒数）
     * 
     * @details
     * すべてのBehaviourコンポーネントのOnUpdate()を呼び出します。
     * ゲームループ内で毎フレーム呼び出してください。
     * 
     * @par 使用例:
     * @code
     * // ゲームループ
     * while (running) {
     *     float deltaTime = CalculateDeltaTime();
     *     world.Tick(deltaTime);  // 全Behaviourを更新
     *     Render();
     * }
     * @endcode
     * 
     * @note 初回はOnStart()も呼ばれます
     */
    void Tick(float dt) {
        // イテレーション中の削除に対応するため、インデックスベースのループを使用
        for (size_t i = 0; i < behaviours_.size(); ++i) {
            auto& it = behaviours_[i];
            if (!IsAlive(it.e)) continue;
            if (!it.started) { it.b->OnStart(*this, it.e); it.started = true; }
            it.b->OnUpdate(*this, it.e, dt);
        }
    }

private:
    /**
     * @interface IStore
     * @brief コンポーネント格納用ストアのインターフェース
     * @details 内部実装用（初学者は読み飛ばしてOK）
     */
    struct IStore {
        virtual ~IStore() = default;
        virtual void Erase(Entity) = 0;
    };

    /**
     * @struct Store
     * @brief 型ごとのコンポーネントストア
     * @tparam T コンポーネントの型
     * @details 内部実装用（初学者は読み飛ばしてOK）
     */
    template<class T>
    struct Store : IStore {
        std::unordered_map<uint32_t, T> map;
        void Erase(Entity e) override { map.erase(e.id); }
    };

    /// ストアの取得（存在しなければ作成）
    template<class T>
    Store<T>& getStore() {
        auto key = std::type_index(typeid(T));
        auto it = stores_.find(key);
        if (it == stores_.end()) {
            auto* s = new Store<T>();
            stores_[key] = s;
            erasers_.push_back([s](Entity e) { s->Erase(e); });
            return *s;
        }
        return *static_cast<Store<T>*>(it->second);
    }

    /// Behaviourの登録（C++14互換 - if constexprの代替）
    template<class TDerived>
    typename std::enable_if<std::is_base_of<Behaviour, TDerived>::value>::type
        registerBehaviour(Entity e, TDerived* obj) {
        behaviours_.push_back({ e, obj, false });
    }
    template<class TDerived>
    typename std::enable_if<!std::is_base_of<Behaviour, TDerived>::value>::type
        registerBehaviour(Entity, TDerived*) {}

    /// Behaviourの登録解除（C++14互換）
    template<class TDerived>
    typename std::enable_if<std::is_base_of<Behaviour, TDerived>::value>::type
        unregisterBehaviour(Entity e) {
        for (size_t i = 0; i < behaviours_.size(); ++i) {
            if (behaviours_[i].e.id == e.id) { behaviours_.erase(behaviours_.begin() + i); break; }
        }
    }
    template<class TDerived>
    typename std::enable_if<!std::is_base_of<Behaviour, TDerived>::value>::type
        unregisterBehaviour(Entity) {}

    /**
     * @struct BEntry
     * @brief Behaviour管理用エントリ
     * @details 内部実装用
     */
    struct BEntry {
        Entity e;           ///< エンティティ
        Behaviour* b;       ///< Behaviourへのポインタ
        bool started = false; ///< OnStartが呼ばれたか
    };

    uint32_t nextId_ = 0;  ///< 次のエンティティID
    std::unordered_map<uint32_t, bool> alive_;  ///< エンティティの生存状態
    std::unordered_map<std::type_index, IStore*> stores_;  ///< コンポーネントストア
    std::vector<std::function<void(Entity)>> erasers_;  ///< 削除用関数
    std::vector<BEntry> behaviours_;  ///< Behaviourリスト
    
    friend class EntityBuilder;  ///< EntityBuilderからprivateメンバにアクセス可能
};

/**
 * @brief EntityBuilder::With()の実装
 * @tparam T 追加するコンポーネントの型
 * @tparam Args コンストラクタ引数の型
 * @param[in] args コンポーネントのコンストラクタ引数
 * @return EntityBuilder& メソッドチェーン用の自身への参照
 */
template<typename T, typename... Args>
EntityBuilder& EntityBuilder::With(Args&&... args) {
    world_->Add<T>(entity_, std::forward<Args>(args)...);
    return *this;
}
