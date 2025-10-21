#pragma once
#include "ecs/Entity.h"
#include "components/Component.h"
#include <unordered_map>
#include <typeindex>
#include <vector>
#include <functional>
#include <type_traits>
#include <stdexcept>
#include <cstdio>

#ifdef _DEBUG
#include <cassert>
#endif

/**
 * @file World.h
 * @brief ECSワールド管理システムとエンティティビルダーの定義
 * @author 山内陽
 * @date 2025
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
 * 直感的に作成できます。Worldクラスと連携して動作します。
 * 
 * @par 使用例
 * @code
 * Entity player = world.Create()
 *     .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
 *     .With<MeshRenderer>(DirectX::XMFLOAT3{0, 1, 0})
 *     .With<Rotator>(45.0f)
 *     .Build();
 * @endcode
 * 
 * @note Build()は省略可能です(暗黙的にEntityへ変換されます)
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
     * @tparam Args コンストラクタ引数の型(可変長)
     * @param[in] args コンポーネントのコンストラクタに転送する引数
     * @return EntityBuilder& メソッドチェーン用の自身への参照
     * 
     * @details
     * 指定したコンポーネントを作成し、エンティティに追加します。
     * メソッドチェーンで複数のコンポーネントを連続して追加できます。
     * 
     * @par 使用例
     * @code
     * world.Create()
     *     .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
     *     .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
     *     .Build();
     * @endcode
     */
    template<typename T, typename... Args>
    EntityBuilder& With(Args&&... args);
    
    /**
     * @brief エンティティを確定して返す
     * @return Entity 作成されたエンティティ
     * 
     * @details
     * ビルダーパターンを完了し、作成されたエンティティを返します。
     * 省略可能で、暗黙的にEntityに変換されます。
     */
    Entity Build() { return entity_; }
    
    /**
     * @brief Entityへの暗黙的型変換演算子
     * @return Entity 作成されたエンティティ
     * 
     * @details
     * Build()を呼ばなくても、自動的にEntityに変換されます。
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
 * ### 主な機能:
 * - エンティティの作成/破棄
 * - コンポーネントの追加/削除/取得
 * - Behaviourコンポーネントの更新
 * 
 * @par 基本的な使用方法
 * @code
 * World world;
 * 
 * // エンティティを作成してコンポーネントを追加
 * Entity player = world.CreateEntity();
 * world.Add<Transform>(player, Transform{...});
 * world.Add<MeshRenderer>(player, MeshRenderer{...});
 * 
 * // コンポーネントを取得して操作
 * auto* transform = world.TryGet<Transform>(player);
 * if (transform) {
 *     transform->position.x += 1.0f;
 * }
 * 
 * // 毎フレーム更新
 * world.Tick(deltaTime);
 * @endcode
 * 
 * @par ビルダーパターン(推奨)
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
     * @brief デストラクタ
     * @details 確保したコンポーネントストアのメモリを解放します
     */
    ~World() {
        for (auto& pair : stores_) {
            delete pair.second;
        }
    }
    
    /**
     * @brief 新しいエンティティを作成
     * @return Entity 一意なIDを持つ新規作成されたエンティティ
     * 
     * @details
     * 一意なIDを持つ新しいエンティティを作成します。初期状態ではコンポーネントは付いていません。
     * 削除されたエンティティのIDを再利用してメモリ効率を向上させます。
     * 
     * @note より便利なエンティティ作成にはCreate()(ビルダー)の使用を推奨してください
     */
    Entity CreateEntity() {
        uint32_t id;
        if (!freeIds_.empty()) {
            // 再利用可能なIDがあればそれを使う
            id = freeIds_.back();
            freeIds_.pop_back();
        } else {
            // なければ新規ID
            id = ++nextId_;
        }
        alive_[id] = true;
        return Entity{id};
    }
    
    /**
     * @brief ビルダーパターンで新しいエンティティを作成
     * @return EntityBuilder メソッドチェーン用のビルダーオブジェクト
     * 
     * @details
     * 直感的なコンポーネント追加を可能にするEntityBuilderを返します。
     * メソッドチェーンで複数のコンポーネントを連続して追加できます。
     * 
     * @par 使用例
     * @code
     * Entity enemy = world.Create()
     *     .With<Transform>(DirectX::XMFLOAT3{5, 0, 0})
     *     .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
     *     .With<Enemy>()
     *     .Build();
     * @endcode
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
     * 
     * @details
     * エンティティがまだ有効かどうかを確認します。
     * 破棄されたエンティティへのアクセスを防ぐために使用します。
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
     * @details
     * 指定されたエンティティとそれに関連するすべてのコンポーネントを削除します。
     * Behaviourコンポーネントも自動的に登録解除されます。
     * IDは再利用用にプールされます。
     * 
     * @warning 破棄されたエンティティを使用するとクラッシュする可能性があります
     */
    void DestroyEntity(Entity e) {
        if (!IsAlive(e)) return;
        alive_[e.id] = false;
        
        // 全コンポーネントを削除
        for (auto& er : erasers_) er(e);
        
        // Behaviourリストから削除（すべての該当エントリを削除）
        for (size_t i = 0; i < behaviours_.size(); ) {
            if (behaviours_[i].e.id == e.id) {
                behaviours_.erase(behaviours_.begin() + i);
                // インデックスを進めない（削除により次の要素がi番目に来る）
            } else {
                ++i;
            }
        }
        
        // ID再利用用に保存
        freeIds_.push_back(e.id);
    }

    /**
     * @brief エンティティにコンポーネントを追加
     * 
     * @tparam T 追加するコンポーネントの型
     * @tparam Args コンストラクタ引数の型(可変長)
     * @param[in] e 対象エンティティ
     * @param[in] args コンポーネントのコンストラクタ引数
     * @return T& 追加されたコンポーネントへの参照
     * @throws std::runtime_error エンティティが死んでいる場合、またはコンポーネントが既に存在する場合（デバッグビルド）
     * 
     * @details
     * 指定したコンポーネントをエンティティに追加します。
     * コンポーネントがBehaviourを継承している場合、Tick()で自動的に更新されます。
     * 
     * @par 使用例
     * @code
     * Entity player = world.CreateEntity();
     * world.Add<Transform>(player, Transform{
     *     DirectX::XMFLOAT3{0, 0, 0},  // 位置
     *     DirectX::XMFLOAT3{0, 0, 0},  // 回転
     *     DirectX::XMFLOAT3{1, 1, 1}   // スケール
     * });
     * @endcode
     * 
     * @note エンティティは生存している必要があります
     * @warning デバッグビルドでは既存コンポーネントへの追加時に例外をスローします
     */
    template<class T, class...Args>
    T& Add(Entity e, Args&&...args) {
        if (!IsAlive(e)) {
            throw std::runtime_error("Attempting to add component to dead entity");
        }
        
        auto& s = getStore<T>();
        
#ifdef _DEBUG
        // デバッグモードでは重複チェック
        if (s.map.find(e.id) != s.map.end()) {
            throw std::runtime_error("Component already exists on entity");
        }
#endif
        
        T& obj = s.map[e.id] = T{ std::forward<Args>(args)... };
        registerBehaviour<T>(e, &obj);
        return obj;
    }

    /**
     * @brief エンティティが指定したコンポーネントを持っているか確認
     * 
     * @tparam T 確認するコンポーネントの型
     * @param[in] e 対象エンティティ
     * @return true 持っている, false 持っていない
     * 
     * @details
     * コンポーネントの存在確認を明示的に行えます。
     * TryGet()のnullptrチェックより意図が明確になります。
     * 
     * @par 使用例
     * @code
     * if (world.Has<Transform>(entity)) {
     *     // Transformを持っている場合の処理
     *     auto* transform = world.TryGet<Transform>(entity);
     *     transform->position.x += 1.0f;
     * }
     * 
     * // より簡潔な書き方
     * if (world.Has<Health>(enemy) && world.Has<Transform>(enemy)) {
     *     // 両方持っている場合の処理
     * }
     * @endcode
     */
    template<class T>
    bool Has(Entity e) const {
        auto itS = stores_.find(std::type_index(typeid(T)));
        if (itS == stores_.end()) return false;
        auto* s = static_cast<const Store<T>*>(itS->second);
        return s->map.find(e.id) != s->map.end();
    }

    /**
     * @brief エンティティからコンポーネントを取得
     * 
     * @tparam T 取得するコンポーネントの型
     * @param[in] e 対象エンティティ
     * @return T* コンポーネントへのポインタ、見つからない場合はnullptr
     * 
     * @details
     * 指定したエンティティから指定したコンポーネントを取得します。
     * コンポーネントが存在しない場合はnullptrを返します。
     * 
     * @par 使用例
     * @code
     * auto* transform = world.TryGet<Transform>(player);
     * if (transform) {
     *     transform->position.x += 1.0f;
     * }
     * @endcode
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
     * @brief エンティティからコンポーネントを取得（const版）
     * 
     * @tparam T 取得するコンポーネントの型
     * @param[in] e 対象エンティティ
     * @return const T* コンポーネントへのポインタ、見つからない場合はnullptr
     * 
     * @details
     * const版のTryGet。読み取り専用アクセス用。
     */
    template<class T>
    const T* TryGet(Entity e) const {
        auto itS = stores_.find(std::type_index(typeid(T)));
        if (itS == stores_.end()) return nullptr;
        auto* s = static_cast<const Store<T>*>(itS->second);
        auto it = s->map.find(e.id);
        if (it == s->map.end()) return nullptr;
        return &it->second;
    }

    /**
     * @brief エンティティからコンポーネントを取得（例外版）
     * 
     * @tparam T 取得するコンポーネントの型
     * @param[in] e 対象エンティティ
     * @return T& コンポーネントへの参照
     * @throws std::runtime_error コンポーネントが存在しない場合
     * 
     * @details
     * 必ず存在するはずのコンポーネントを取得する際に使用します。
     * nullptrチェックが不要になりコードが簡潔になります。
     * 
     * @par 使用例
     * @code
     * // 必ずTransformを持つと分かっている場合
     * Transform& transform = world.Get<Transform>(player);
     * transform.position.x += 1.0f;
     * 
     * // try-catchで例外を処理
     * try {
     *     MeshRenderer& renderer = world.Get<MeshRenderer>(entity);
     *     renderer.color = DirectX::XMFLOAT3{1, 0, 0};
     * } catch (const std::runtime_error& e) {
     *     // コンポーネントが存在しない場合の処理
     *     printf("Error: %s\n", e.what());
     * }
     * @endcode
     * 
     * @warning コンポーネントが存在しない場合は例外がスローされます
     */
    template<class T>
    T& Get(Entity e) {
        T* ptr = TryGet<T>(e);
        if (!ptr) {
            throw std::runtime_error("Component not found on entity");
        }
        return *ptr;
    }

    /**
     * @brief エンティティからコンポーネントを取得（const例外版）
     * 
     * @tparam T 取得するコンポーネントの型
     * @param[in] e 対象エンティティ
     * @return const T& コンポーネントへのconst参照
     * @throws std::runtime_error コンポーネントが存在しない場合
     */
    template<class T>
    const T& Get(Entity e) const {
        const T* ptr = TryGet<T>(e);
        if (!ptr) {
            throw std::runtime_error("Component not found on entity");
        }
        return *ptr;
    }

    /**
     * @brief 指定されたコンポーネントを持つすべてのエンティティに対して関数を実行
     * 
     * @tparam T クエリ対象のコンポーネント型
     * @tparam F 関数の型
     * @param[in] fn 実行する関数(EntityとT&を受け取る)
     * 
     * @details
     * 指定したコンポーネントを持つすべてのエンティティに対して、
     * 提供された関数を実行します。
     * イテレーション中のエンティティ削除に対応しています。
     * 
     * @par 使用例
     * @code
     * // すべてのTransformを持つエンティティを上に移動
     * world.ForEach<Transform>([](Entity e, Transform& t) {
     *     t.position.y += 0.1f;
     * });
     * 
     * // すべての敵のHPを確認（削除も安全)
     * world.ForEach<Enemy>([&](Entity e, Enemy& enemy) {
     *     if (enemy.health <= 0) {
     *         world.DestroyEntity(e);  // ? 安全に削除可能
     *     }
     * });
     * @endcode
     */
    template<class T, class F>
    void ForEach(F&& fn) {
        auto itS = stores_.find(std::type_index(typeid(T)));
        if (itS == stores_.end()) return;
        auto* s = static_cast<Store<T>*>(itS->second);
        
        // IDのリストを先に作成（イテレーション中の削除に対応)
        std::vector<uint32_t> ids;
        ids.reserve(s->map.size());
        for (auto& pair : s->map) {
            ids.push_back(pair.first);
        }
        
        // 安全にイテレート
        for (uint32_t id : ids) {
            Entity e{id};
            if (!IsAlive(e)) continue;
            auto it = s->map.find(id);
            if (it == s->map.end()) continue;
            fn(e, it->second);
        }
    }

    /**
     * @brief 2つのコンポーネントを持つエンティティに対して処理
     * 
     * @tparam T1 1つ目のコンポーネント型
     * @tparam T2 2つ目のコンポーネント型
     * @tparam F 関数の型
     * @param[in] fn 実行する関数(Entity, T1&, T2&を受け取る)
     * 
     * @details
     * 指定した2つのコンポーネントを両方持つエンティティに対して、
     * 提供された関数を実行します。
     * イテレーション中のエンティティ削除に対応しています。
     * 
     * @par 使用例
     * @code
     * // TransformとMeshRendererを両方持つエンティティを処理
     * world.ForEach<Transform, MeshRenderer>(
     *     [](Entity e, Transform& t, MeshRenderer& r) {
     *         // 両方のコンポーネントにアクセス可能
     *         r.color.x = t.position.x / 10.0f;
     *     }
     * );
     * 
     * // 物理演算の例
     * world.ForEach<Transform, Velocity>(
     *     [](Entity e, Transform& t, Velocity& v) {
     *         t.position.x += v.velocity.x * dt;
     *         t.position.y += v.velocity.y * dt;
     *         t.position.z += v.velocity.z * dt;
     *     }
     * );
     * 
     * // 敵の体力チェック
     * world.ForEach<Enemy, Health>([&](Entity e, Enemy& enemy, Health& hp) {
     *     if (hp.IsDead()) {
     *         world.DestroyEntity(e);  // ? 安全に削除可能
     *     }
     * });
     * @endcode
     */
    template<class T1, class T2, class F>
    void ForEach(F&& fn) {
        auto itS1 = stores_.find(std::type_index(typeid(T1)));
        if (itS1 == stores_.end()) return;
        auto* s1 = static_cast<Store<T1>*>(itS1->second);
        
        // IDのリストを先に作成（イテレーション中の削除に対応）
        std::vector<uint32_t> ids;
        ids.reserve(s1->map.size());
        for (auto& pair : s1->map) {
            ids.push_back(pair.first);
        }
        
        // 安全にイテレート
        for (uint32_t id : ids) {
            Entity e{id};
            if (!IsAlive(e)) continue;
            
            auto it1 = s1->map.find(id);
            if (it1 == s1->map.end()) continue;
            
            T2* comp2 = TryGet<T2>(e);
            if (!comp2) continue;
            
            fn(e, it1->second, *comp2);
        }
    }

    /**
     * @brief すべてのBehaviourコンポーネントを更新
     * 
     * @param[in] dt デルタタイム(前フレームからの経過時間)
     * 
     * @details
     * すべてのBehaviourコンポーネントのOnUpdate()を呼び出します。毎フレーム呼び出す必要があります。
     * 初回呼び出し時にはOnStart()も実行されます。
     * OnUpdate内でのエンティティ削除に対応しています。
     * 
     * @par 使用例
     * @code
     * // ゲームループ
     * while (running) {
     *     float deltaTime = CalculateDeltaTime();
     *     
     *     // すべてのBehaviourを更新
     *     world.Tick(deltaTime);
     *     
     *     // 描画処理...
     * }
     * @endcode
     */
    void Tick(float dt) {
        // イテレーション中の削除に対応するためインデックスベースのループを使用
        for (size_t i = 0; i < behaviours_.size(); ) {
            auto& entry = behaviours_[i];
            
            // 死んだエンティティのBehaviourを削除
            if (!IsAlive(entry.e)) {
                behaviours_.erase(behaviours_.begin() + i);
                continue;  // インデックスを進めない
            }
            
            // OnStartとOnUpdateを実行
            if (!entry.started) {
                entry.b->OnStart(*this, entry.e);
                entry.started = true;
            }
            entry.b->OnUpdate(*this, entry.e, dt);
            
            // 再度生存確認（OnUpdate内で削除されたかもしれない）
            if (IsAlive(entry.e)) {
                ++i;  // 生存していればインデックスを進める
            }
            // 削除されていたら自動的に次の要素がi番目に来るのでインデックスを進めない
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
        std::unordered_map<uint32_t, T> map;  ///< EntityID -> コンポーネントのマップ
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

    /// 自動更新のためにBehaviourコンポーネントを登録(C++14互換)
    template<class TDerived>
    typename std::enable_if<std::is_base_of<Behaviour, TDerived>::value>::type
        registerBehaviour(Entity e, TDerived* obj) {
        behaviours_.push_back({ e, obj, false });
    }
    template<class TDerived>
    typename std::enable_if<!std::is_base_of<Behaviour, TDerived>::value>::type
        registerBehaviour(Entity, TDerived*) {}

    /// Behaviourコンポーネントの登録を解除(C++14互換)
    template<class TDerived>
    typename std::enable_if<std::is_base_of<Behaviour, TDerived>::value>::type
        unregisterBehaviour(Entity e) {
        // 同じエンティティの複数Behaviourに対応
        for (size_t i = 0; i < behaviours_.size(); ) {
            if (behaviours_[i].e.id == e.id) {
                behaviours_.erase(behaviours_.begin() + i);
                // インデックスを進めない（削除により次の要素がi番目に来る）
            } else {
                ++i;
            }
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
    std::vector<uint32_t> freeIds_;  ///< 再利用可能なID
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
