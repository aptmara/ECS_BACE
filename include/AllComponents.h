/**
 * @file AllComponents.h
 * @brief すべてのコンポーネントを一括インクルード（PCH代替）
 * @author 山内陽
 * @date 2025
 * @version 1.0
 * 
 * @details
 * PCHが使えない環境や、特定のファイルだけで使いたい場合の代替ヘッダーです。
 * 
 * ### 使い方:
 * @code
 * #include "AllComponents.h"  // これだけでOK
 * 
 * // すべてのコンポーネントが使える
 * Entity e = world.Create()
 *     .With<Transform>()
 *     .With<MeshRenderer>()
 *     .With<Rotator>()
 *     .Build();
 * @endcode
 */
#pragma once

// ECS コア
#include "ecs/Entity.h"
#include "ecs/World.h"

// 基本コンポーネント
#include "components/Component.h"
#include "components/Transform.h"
#include "components/MeshRenderer.h"
#include "components/Rotator.h"

// サンプルコンポーネント
#include "samples/ComponentSamples.h"

// アニメーション
#include "animation/Animation.h"

// システム
#include "input/InputSystem.h"
#include "scenes/SceneManager.h"

// 標準ライブラリ（よく使うもの）
#include <vector>
#include <memory>
#include <cmath>
