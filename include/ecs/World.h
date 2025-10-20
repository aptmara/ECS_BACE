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
 * ECSアーキテクチャの中核となるWorldクラスと、
 * エンティティを便利に作成するためのEntityBuilderクラスを定義します。
 */

class World; ///< 前方宣言

/**
 * @class EntityBuilder
 * @brief エンティティ作成用のビルダーパターンクラス
 * 
 * @details
 * メソッドチェーンを使用して、複数のコンポーネントを持つエンティティを
 * 流暢に作成できます。Worldクラスと連携して動作します。
 * 
 * @par 使用例:
 * @code
 * Entity player = world.Create()
 *     .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
 *     .With<MeshRenderer>(DirectX::XMFLOAT3{0, 1, 0})
 *     .With<Rotator>(45.0f)
 *     .Build();
 * @endcode
 * 
 * @note Build()は省略可能です（暗黙的にEntityへ変換されます）
 * @see World
 * 
 * @author 山内陽
 */
class EntityBuilder {
public:
    /**
     * @brief コンストラクタ
     * @param[in] world Worldインスタンスへのポインタ
     * @param[in] entity 作成されたエンティティ
     */
    EntityBuilder(World* world, Entity entity) : world_(world), entity_(entity) {}
    
    /**
     * @brief メソッドチェーンでコンポーネントを追加
     * 
     * @tparam T 追加するコンポーネントの型
     * @tparam Args コンストラクタ引数の型（可変長）
     * @param[in] args コンポーネントのコンストラクタに転送する引数
     * @return EntityBuilder& メソッドチェーン用の自身への参照
     */
    template<typename T, typename... Args>
    EntityBuilder& With(Args&&... args);
    
    /**
     * @brief エンティティを確定して返す
     * @return Entity 作成されたエンティティ
     */
    Entity Build() { return entity_; }
    
    /**
     * @brief Entityへの暗黙的型変換演算子
     * @return Entity 作成されたエンティティ
     */
    operator Entity() const { return entity_; }

private:
    World* world_;    ///< Worldインスタンスへのポインタ
    Entity entity_;   ///< 作成されたエンティティ
};

/**
 * @class World
 * @brief ECSワールド管理クラス
 * 
 * @details
 * ECSアーキテクチャにおけるすべてのエンティティとコンポーネントを管理します。
 * 
 * 機能:
 * - エンティティの作成/破棄
 * - コンポーネントの追加/削除/取得
 * - Behaviourコンポーネントの更新
 * 
 * @par 基本的な使用方法:
 * @code
 * World world;
 * 
 * Entity player = world.CreateEntity();
 * world.Add<Transform>(player, Transform{...});
 * world.Add<MeshRenderer>(player, MeshRenderer{...});
 * 
 * auto* transform = world.TryGet<Transform>(player);
 * if (transform) {
 *     transform->position.x += 1.0f;
 * }
 * 
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
 * @see Entity
 * @see IComponent
 * @see Behaviour
 * 
 * @author 山内陽
 */
class World {
public:
    /**
     * @brief 新しいエンティティを作成
     * @return Entity 一意なIDを持つ新規作成されたエンティティ
     * 
     * @details
     * 一意なIDを持つ新しいエンティティを作成します。初期状態ではコンポーネントは付いていません。
     * 
     * @note より便利なエンティティ作成にはCreate()（ビルダー版）の使用を検討してください
     */
    Entity CreateEntity() {
        Entity e{ ++nextId_ };
        alive_[e.id] = true;
        return e;
    }
    
    /**
     * @brief ビルダーパターンで新しいエンティティを作成
     * @return EntityBuilder メソッドチェーン用のビルダーオブジェクト
     * 
     * @details
     * 流暢なコンポーネント追加を可能にするEntityBuilderを返します。
     * 
     * @see EntityBuilder
     */
    EntityBuilder Create() {
        return EntityBuilder(this, CreateEntity());
    }

    /**
     * @brief エンティティが生存しているか確認
     * 
     * @param[in] e 確認するエンティティ
     * @return true 生存している, false 破棄済み
     */
    bool IsAlive(Entity e) const {
        auto it = alive_.find(e.id);
        return it != alive_.end() && it->second;
    }

    /**
     * @brief エンティティとそのすべてのコンポーネントを破棄
     * 
     * @param[in] e 破棄するエンティティ
     * 
     * @warning 破棄されたエンティティを使用するとクラッシュする可能性があります
     */
    void DestroyEntity(Entity e) {
        if (!IsAlive(e)) return;
        alive_[e.id] = false;
        for (auto& er : erasers_) er(e);
        for (size_t i = 0; i < behaviours_.size(); ++i)
            if (behaviours_[i].e.id == e.id) { behaviours_.erase(behaviours_.begin() + i); --i; }
    }

    /**
     * @brief エンティティにコンポーネントを追加
     * 
     * @tparam T 追加するコンポーネントの型
     * @tparam Args コンストラクタ引数の型（可変長）
     * @param[in] e 対象エンティティ
     * @param[in] args コンポーネントのコンストラクタ引数
     * @return T& 追加されたコンポーネントへの参照
     * 
     * @details
     * コンポーネントがBehaviourを継承している場合、Tick()で自動的に更新されます。
     * 
     * @note エンティティは生存している必要があります
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
     * @brief エンティティからコンポーネントを取得
     * 
     * @tparam T 取得するコンポーネントの型
     * @param[in] e 対象エンティティ
     * @return T* コンポーネントへのポインタ、見つからない場合はnullptr
     * 
     * @warning 使用前に必ずnullptrチェックを行ってください
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
     * @brief エンティティからコンポーネントを削除
     * 
     * @tparam T 削除するコンポーネントの型
     * @param[in] e 対象エンティティ
     * @return true 削除成功, false コンポーネントが存在しなかった
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
     * @brief 指定されたコンポーネントを持つすべてのエンティティに対して関数を実行
     * 
     * @tparam T クエリ対象のコンポーネント型
     * @tparam F 関数の型
     * @param[in] fn 実行する関数（EntityとT&を受け取る）
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
     * @brief すべてのBehaviourコンポーネントを更新
     * 
     * @param[in] dt デルタタイム（前フレームからの経過秒数）
     * 
     * @details
     * すべてのBehaviourコンポーネントのOnUpdate()を呼び出します。毎フレーム呼び出す必要があります。
     * 初回呼び出し時にはOnStart()も実行されます。
     */
    void Tick(float dt) {
        // イテレーション中の削除に対応するためインデックスベースのループを使用
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
     * @brief コンポーネント格納用のインターフェース
     */
    struct IStore {
        virtual ~IStore() = default;
        virtual void Erase(Entity) = 0;
    };

    /**
     * @struct Store
     * @brief 型固有のコンポーネント格納構造
     * @tparam T コンポーネントの型
     */
    template<class T>
    struct Store : IStore {
        std::unordered_map<uint32_t, T> map;
        void Erase(Entity e) override { map.erase(e.id); }
    };

    /// コンポーネント型Tのストアを取得または作成
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

    /// 自動更新のためにBehaviourコンポーネントを登録（C++14互換）
    template<class TDerived>
    typename std::enable_if<std::is_base_of<Behaviour, TDerived>::value>::type
        registerBehaviour(Entity e, TDerived* obj) {
        behaviours_.push_back({ e, obj, false });
    }
    template<class TDerived>
    typename std::enable_if<!std::is_base_of<Behaviour, TDerived>::value>::type
        registerBehaviour(Entity, TDerived*) {}

    /// Behaviourコンポーネントの登録を解除（C++14互換）
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
     */
    struct BEntry {
        Entity e;           ///< エンティティ
        Behaviour* b;       ///< Behaviourへのポインタ
        bool started = false; ///< OnStartが呼ばれたかどうか
    };

    uint32_t nextId_ = 0;  ///< 次のエンティティID
    std::unordered_map<uint32_t, bool> alive_;  ///< エンティティの生存状態
    std::unordered_map<std::type_index, IStore*> stores_;  ///< コンポーネントストア
    std::vector<std::function<void(Entity)>> erasers_;  ///< 削除用関数
    std::vector<BEntry> behaviours_;  ///< Behaviourリスト
    
    friend class EntityBuilder;  ///< EntityBuilderがprivateメンバにアクセスできるようにする
};

/**
 * @brief EntityBuilder::With()の実装
 * @tparam T 追加するコンポーネントの型
 * @tparam Args コンストラクタ引数の型
 * @param[in] args コンストラクタ引数
 * @return EntityBuilder& メソッドチェーン用の自身への参照
 */
template<typename T, typename... Args>
EntityBuilder& EntityBuilder::With(Args&&... args) {
    world_->Add<T>(entity_, std::forward<Args>(args)...);
    return *this;
}
