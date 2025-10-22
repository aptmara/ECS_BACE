#pragma once
#include <cstdint>
#include <functional>

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
 * - **軽量**: id と世代番号 generation を保持
 * - **安全性**: 世代番号により古いハンドルを無効化し use-after-free を防止
 * - **柔軟性**: コンポーネントの組み合わせで機能を定義
 * 
 * @note 同一フレームでのID再利用による不具合を避けるため、世代番号を導入しています
 */
struct Entity {
    uint32_t id;   ///< エンティティID
    uint32_t gen;  ///< 世代番号（破棄の度にインクリメント）

    // 比較演算子（id と generation の両方を比較）
    bool operator==(const Entity& other) const { return id == other.id && gen == other.gen; }
    bool operator!=(const Entity& other) const { return !(*this == other); }
    bool operator<(const Entity& other) const { return id < other.id || (id == other.id && gen < other.gen); }
};

// 構造体の外にハッシュの特殊化を追加
namespace std {
    template <>
    struct hash<Entity> {
        size_t operator()(const Entity& e) const {
            // 簡易結合ハッシュ(id と gen)
            return (static_cast<size_t>(e.id) << 32) ^ static_cast<size_t>(e.gen);
        }
    };
}
