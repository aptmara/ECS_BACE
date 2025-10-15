#pragma once
#include <cstdint>

/**
 * @file Entity.h
 * @brief ECSアーキテクチャのエンティティ定義
 * @author 山内陽
 * @date 2024
 * @version 5.0
 * 
 * @details
 * このファイルはEntity Component System（ECS）アーキテクチャにおける
 * エンティティの基本定義を提供します。
 */

/**
 * @struct Entity
 * @brief ゲーム世界に存在するオブジェクトを表す一意な識別子
 * 
 * @details
 * ECSアーキテクチャにおけるエンティティは、ゲーム世界の「物」を表す
 * 単なるID番号です。エンティティ自体には機能がなく、コンポーネントを
 * 組み合わせることで機能を持たせます。
 * 
 * ### エンティティの特徴:
 * - **軽量**: uint32_t型のID番号のみを保持
 * - **一意**: WorldによってユニークなIDが割り当てられる
 * - **柔軟**: コンポーネントの組み合わせで様々な機能を実現
 * 
 * ### コンポーネント指向の考え方:
 * 従来のオブジェクト指向では、継承によって機能を追加しますが、
 * ECSではエンティティにコンポーネントを追加することで機能を実現します。
 * 
 * @par 従来のオブジェクト指向（継承）:
 * @code
 * // ダメな例: 継承による機能追加
 * class Player : public Character {
 *     // プレイヤー専用の処理をごちゃ混ぜ
 * };
 * @endcode
 * 
 * @par ECS（コンポーネント）:
 * @code
 * // 良い例: コンポーネントによる機能追加
 * Entity player = world.Create()
 *     .With<Transform>()      // 位置の機能
 *     .With<MeshRenderer>()   // 見た目の機能
 *     .With<PlayerInput>()    // 入力の機能
 *     .With<Health>()         // 体力の機能
 *     .Build();
 * @endcode
 * 
 * ### 具体例:
 * @code
 * // プレイヤーエンティティの例
 * Entity player = world.Create()
 *     .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
 *     .With<MeshRenderer>(DirectX::XMFLOAT3{0, 1, 0})  // 緑色
 *     .With<Player>()  // プレイヤータグ
 *     .Build();
 * 
 * // 敵エンティティの例
 * Entity enemy = world.Create()
 *     .With<Transform>(DirectX::XMFLOAT3{5, 0, 0})
 *     .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})  // 赤色
 *     .With<Enemy>()   // 敵タグ
 *     .With<Health>()  // 体力
 *     .Build();
 * 
 * // 弾エンティティの例
 * Entity bullet = world.Create()
 *     .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
 *     .With<MeshRenderer>(DirectX::XMFLOAT3{1, 1, 0})  // 黄色
 *     .With<Bullet>()  // 弾タグ
 *     .Build();
 * @endcode
 * 
 * @note エンティティのIDは自動的に割り当てられるため、直接操作する必要はありません
 * @warning エンティティを削除する際は、必ずWorld::DestroyEntity()を使用してください
 * 
 * @see World エンティティとコンポーネントを管理するクラス
 * @see IComponent コンポーネントの基底クラス
 * @see Transform 位置・回転・スケールコンポーネント
 * 
 * @author 山内陽
 */
struct Entity {
    /**
     * @var id
     * @brief エンティティの一意識別番号
     * 
     * @details
     * Worldクラスによって自動的に割り当てられる一意なID番号です。
     * このIDを使って、Worldからコンポーネントを取得・追加・削除します。
     * 
     * @note ID 0は無効なエンティティを表す特殊値として使用される場合があります
     * @warning このIDを直接変更しないでください
     * 
     * @par 使用例:
     * @code
     * Entity entity = world.CreateEntity();
     * std::cout << "Entity ID: " << entity.id << std::endl;
     * 
     * // IDを使ってコンポーネントを取得
     * auto* transform = world.TryGet<Transform>(entity);
     * @endcode
     */
    uint32_t id;
};
