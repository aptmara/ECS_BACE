/**
 * @file StageComponents.h
 * @brief ステージ進行用のタグと状態コンポーネント
 */
#pragma once

#include "components/Component.h"

/**
 * @struct StartTag
 * @brief ステージの開始地点を示すタグ
 */
struct StartTag : IComponent {};

/**
 * @struct GoalTag
 * @brief ステージのゴール地点を示すタグ
 */
struct GoalTag : IComponent {};

/**
 * @struct StageProgress
 * @brief ステージ番号と進行フラグを管理
 */
struct StageProgress : IComponent {
    int currentStage = 1;
    bool requestAdvance = false;
};
