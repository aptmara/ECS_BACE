#pragma once
#include <cstdint>

// ========================================================
// Entity - ECSのエンティティ（ゲームオブジェクト）
// ========================================================
// 【エンティティとは？】
// ゲーム世界に存在する「モノ」を表す単なるID番号
// エンティティ自体には機能がなく、コンポーネントを取り付けることで機能を持つ
//
// 【例】
// - プレイヤー = Entity(id=1) + Transform + MeshRenderer + PlayerController
// - 敵 = Entity(id=2) + Transform + MeshRenderer + EnemyAI
// - 弾 = Entity(id=3) + Transform + MeshRenderer + Bullet
// ========================================================
struct Entity {
    uint32_t id; // エンティティの識別番号（WorldがユニークなIDを自動割り当て）
};
