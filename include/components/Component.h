#pragma once

class World;   ///< 前方宣言
struct Entity; ///< 前方宣言

struct IComponent {
    virtual ~IComponent() = default;
};

struct Behaviour : IComponent {
    virtual void OnStart(World& w, Entity self) {}
    virtual void OnUpdate(World& w, Entity self, float dt) {}
};

#define DEFINE_DATA_COMPONENT(ComponentName, ...) \
    struct ComponentName : IComponent { \
        __VA_ARGS__ \
    }

#define DEFINE_BEHAVIOUR(BehaviourName, DataMembers, UpdateCode) \
    struct BehaviourName : Behaviour { \
        DataMembers \
        void OnUpdate(World& w, Entity self, float dt) override { \
            UpdateCode \
        } \
    }
