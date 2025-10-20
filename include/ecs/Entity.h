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
 * Entity Component System（ECS）アーキテクチャにおける
 * エンティティの基本構造を定義します。
 */

/**
 * @struct Entity
 * @brief ゲーム世界のオブジェクトを表す一意な識別子
 * 
 * @details
 * ECSアーキテクチャにおいて、エンティティはオブジェクトを表す一意なID番号です。
 * エンティティ自体には機能がなく、コンポーネントを通じて機能を追加します。
 * 
 * 特徴:
 * - 軽量: uint32_t型のIDのみを保持
 * - 一意: Worldによって一意なIDが割り当てられる
 * - 柔軟: コンポーネントの組み合わせで機能を実現
 * 
 * @par 使用例:
 * @code
 * Entity player = world.Create()
 *     .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
 *     .With<MeshRenderer>(DirectX::XMFLOAT3{0, 1, 0})
 *     .With<Player>()
 *     .Build();
 * 
 * Entity enemy = world.Create()
 *     .With<Transform>(DirectX::XMFLOAT3{5, 0, 0})
 *     .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
 *     .With<Enemy>()
 *     .With<Health>()
 *     .Build();
 * @endcode
 * 
 * @note エンティティIDは自動的に割り当てられます。手動操作は不要です
 * @warning エンティティの削除には必ずWorld::DestroyEntity()を使用してください
 * 
 * @see World
 * @see IComponent
 * @see Transform
 * 
 * @author 山内陽
 */
struct Entity {
    /**
     * @var id
     * @brief エンティティの一意な識別番号
     * 
     * @details
     * Worldクラスによって自動的に割り当てられる一意なID番号です。
     * このIDを使用して、Worldからコンポーネントの取得・追加・削除を行います。
     * 
     * @note ID 0は無効なエンティティを表す特殊値として使用される場合があります
     * @warning このIDを直接変更しないでください
     */
    uint32_t id;
};
