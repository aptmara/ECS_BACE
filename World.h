#pragma once
#include "Entity.h"
#include "Component.h"
#include <unordered_map>
#include <typeindex>
#include <vector>
#include <functional>
#include <type_traits>
#include <cassert>

// ========================================================
// World - ECSワールド管理クラス
// ========================================================
class World {
public:
    // エンティティの作成
    Entity CreateEntity() {
        Entity e{ ++nextId_ };
        alive_[e.id] = true;
        return e;
    }

    // エンティティが生存しているか確認
    bool IsAlive(Entity e) const {
        auto it = alive_.find(e.id);
        return it != alive_.end() && it->second;
    }

    // エンティティの破棄
    void DestroyEntity(Entity e) {
        if (!IsAlive(e)) return;
        alive_[e.id] = false;
        for (auto& er : erasers_) er(e);
        for (size_t i = 0; i < behaviours_.size(); ++i)
            if (behaviours_[i].e.id == e.id) { behaviours_.erase(behaviours_.begin() + i); --i; }
    }

    // コンポーネントの追加
    template<class T, class...Args>
    T& Add(Entity e, Args&&...args) {
        assert(IsAlive(e) && "Add on dead entity");
        auto& s = getStore<T>();
        T& obj = s.map[e.id] = T{ std::forward<Args>(args)... };
        registerBehaviour<T>(e, &obj);
        return obj;
    }

    // コンポーネントの取得（nullptrの可能性あり）
    template<class T>
    T* TryGet(Entity e) {
        auto itS = stores_.find(std::type_index(typeid(T)));
        if (itS == stores_.end()) return nullptr;
        auto* s = static_cast<Store<T>*>(itS->second);
        auto it = s->map.find(e.id);
        if (it == s->map.end()) return nullptr;
        return &it->second;
    }

    // コンポーネントの削除
    template<class T>
    bool Remove(Entity e) {
        auto itS = stores_.find(std::type_index(typeid(T)));
        if (itS == stores_.end()) return false;
        auto* s = static_cast<Store<T>*>(itS->second);
        unregisterBehaviour<T>(e);
        return s->map.erase(e.id) > 0;
    }

    // 全コンポーネントに対して処理を実行
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

    // 全Behaviourの更新
    void Tick(float dt) {
        for (auto& it : behaviours_) {
            if (!IsAlive(it.e)) continue;
            if (!it.started) { it.b->OnStart(*this, it.e); it.started = true; }
            it.b->OnUpdate(*this, it.e, dt);
        }
    }

private:
    // コンポーネント格納用ストア基底インターフェース
    struct IStore {
        virtual ~IStore() = default;
        virtual void Erase(Entity) = 0;
    };

    // 型ごとのコンポーネントストア
    template<class T>
    struct Store : IStore {
        std::unordered_map<uint32_t, T> map;
        void Erase(Entity e) override { map.erase(e.id); }
    };

    // ストアの取得（存在しなければ作成）
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

    // Behaviourの登録（C++14互換 - if constexpr の代わり）
    template<class TDerived>
    typename std::enable_if<std::is_base_of<Behaviour, TDerived>::value>::type
        registerBehaviour(Entity e, TDerived* obj) {
        behaviours_.push_back({ e, obj, false });
    }
    template<class TDerived>
    typename std::enable_if<!std::is_base_of<Behaviour, TDerived>::value>::type
        registerBehaviour(Entity, TDerived*) {}

    // Behaviourの登録解除（C++14互換）
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

    // Behaviour管理用エントリ
    struct BEntry {
        Entity e;
        Behaviour* b;
        bool started = false;
    };

    // メンバ変数
    uint32_t nextId_ = 0;
    std::unordered_map<uint32_t, bool> alive_;
    std::unordered_map<std::type_index, IStore*> stores_;
    std::vector<std::function<void(Entity)>> erasers_;
    std::vector<BEntry> behaviours_;
};
