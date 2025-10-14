#pragma once

// ========================================================
// Component - ECSコンポーネントの基底インターフェース
// ========================================================
struct IComponent {
    virtual ~IComponent() = default;
};

// ========================================================
// Behaviour - 更新可能なコンポーネント基底クラス
// ========================================================
class World; // 前方宣言
struct Entity; // 前方宣言

struct Behaviour : IComponent {
    virtual void OnStart(World&, Entity) {}
    virtual void OnUpdate(World&, Entity, float /*dt*/) {}
};
