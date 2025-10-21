#pragma once
#include <cstdint>

/**
 * @file Entity.h
 * @brief ECSアーキテクチャのエンティティ定義
 * @author 山内陽
 * @date 2025
 * @version 5.0
 * 
 * @details
 * Entity Component System(ECS)アーキテクチャにおける
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
 * ### 特徴:
 * - **軽量**: uint32_t型のIDのみを保持
 * - **一意性**: Worldによって一意なIDが割り当てられる
 * - **柔軟性**: コンポーネントの組み合わせで機能を定義
 * 
 * ### ECSの概念:
 * - **Entity(エンティティ)**: オブジェクトの識別子(このクラス)
 * - **Component(コンポーネント)**: データの集まり(Transform, MeshRendererなど)
 * - **System(システム)**: コンポーネントを処理するロジック(RenderSystem, Worldなど)
 * 
 * @par 使用例(プレイヤーの作成)
 * @code
 * Entity player = world.Create()
 *     .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
 *     .With<MeshRenderer>(DirectX::XMFLOAT3{0, 1, 0})
 *     .With<Player>()
 *     .Build();
 * @endcode
 * 
 * @par 使用例(敵の作成)
 * @code
 * Entity enemy = world.Create()
 *     .With<Transform>(DirectX::XMFLOAT3{5, 0, 0})
 *     .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
 *     .With<Enemy>()
 *     .With<Health>()
 *     .Build();
 * @endcode
 * 
 * @par ECSの利点:
 * - **柔軟性**: コンポーネントの追加・削除が容易
 * - **再利用性**: コンポーネントを複数のエンティティで共有
 * - **パフォーマンス**: データ指向設計でキャッシュ効率が良い
 * - **保守性**: 機能が明確に分離される
 * 
 * @note エンティティIDは自動的に割り当てられます。手動操作は不要です
 * @warning エンティティの削除には必ずWorld::DestroyEntity()を使用してください
 * 
 * @see World エンティティとコンポーネントを管理するクラス
 * @see IComponent コンポーネントの基底クラス
 * @see Transform 位置・回転・スケールを持つコンポーネント
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
     * ### IDの管理:
     * - 1から順番に割り当てられる
     * - 同じIDが再利用されることはない(セッション中)
     * - 0は無効なエンティティを表す予約値として使用される場合がある
     * 
     * @par 使用例
     * @code
     * Entity player = world.CreateEntity();
     * printf("Player ID: %u\n", player.id);  // 例: Player ID: 1
     * 
     * // コンポーネントの操作
     * world.Add<Transform>(player, Transform{...});
     * auto* transform = world.TryGet<Transform>(player);
     * @endcode
     * 
     * @note ID 0は無効なエンティティを表す定数値として使用される場合があります
     * @warning このIDを直接変更しないでください
     */
    uint32_t id;
};
