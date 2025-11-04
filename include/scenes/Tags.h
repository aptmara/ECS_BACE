#pragma once

#include "pch.h"

/**
 * @struct Player
 * @brief プレイヤータグ
 * @details プレイヤーエンティティを識別するためのマーカー
 */
struct Player : IComponent {};

/**
 * @struct Enemy
 * @brief 敵タグ
 * @details 敵エンティティを識別するためのマーカー
 */
struct Enemy : IComponent {};