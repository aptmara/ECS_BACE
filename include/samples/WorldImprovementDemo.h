/**
 * @file WorldImprovementDemo.h
 * @brief World���P�@�\�̃f�����X�g���[�V����
 * @author �R���z
 * @date 2025
 * @version 1.0
 *
 * @details
 * World v5.0�̐V�@�\�iHas, Get, ForEach<T1,T2>�Ȃǁj�̎g�p���񋟂��܂��B
 */
#pragma once

#include "ecs/World.h"
#include "components/Transform.h"
#include "components/MeshRenderer.h"
#include "components/GameTags.h"
#include "components/GameComponents.h"
#include <DirectXMath.h>

namespace WorldImprovementDemo {

// ========================================================
// �V�@�\1: Has() - �R���|�[�l���g�̑��݊m�F
// ========================================================

/**
 * @brief Has()���\�b�h�̎g�p��
 * 
 * @details
 * Has()���g�����ƂŁA�R���|�[�l���g�̑��݃`�F�b�N�������I�ɂȂ�܂��B
 */
inline void DemoHasMethod(World& world) {
    Entity entity = world.Create()
        .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
        .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
        .Build();
    
    // ? �V�������@: Has()�Ŗ����I�Ƀ`�F�b�N
    if (world.Has<Transform>(entity)) {
        auto* transform = world.TryGet<Transform>(entity);
        transform->position.x += 1.0f;
    }
    
    // ? �����R���|�[�l���g�̃`�F�b�N
    if (world.Has<Transform>(entity) && world.Has<MeshRenderer>(entity)) {
        // ���������Ă���ꍇ�̏���
        printf("Entity has both Transform and MeshRenderer\n");
    }
    
    // ? �Â����@�i����͂��邪�Ӑ}���s���m�j
    auto* transform = world.TryGet<Transform>(entity);
    if (transform) {
        transform->position.x += 1.0f;
    }
}

// ========================================================
// �V�@�\2: Get() - ��O�ł̎擾
// ========================================================

/**
 * @brief Get()���\�b�h�̎g�p��
 * 
 * @details
 * �K�����݂���R���|�[�l���g�ɂ� Get() ���g�����Ƃ�
 * nullptr�`�F�b�N���s�v�ɂȂ�܂��B
 */
inline void DemoGetMethod(World& world) {
    Entity player = world.Create()
        .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
        .With<MeshRenderer>(DirectX::XMFLOAT3{0, 1, 0})
        .Build();
    
    // ? �V�������@: Get()��nullptr�`�F�b�N�s�v
    try {
        Transform& transform = world.Get<Transform>(player);
        transform.position.x += 1.0f;
        
        MeshRenderer& renderer = world.Get<MeshRenderer>(player);
        renderer.color = DirectX::XMFLOAT3{1, 1, 1};
        
    } catch (const std::runtime_error& e) {
        printf("Error: %s\n", e.what());
    }
    
    // ? �Â����@�i�璷�j
    auto* transform = world.TryGet<Transform>(player);
    if (transform) {
        transform->position.x += 1.0f;
    }
    auto* renderer = world.TryGet<MeshRenderer>(player);
    if (renderer) {
        renderer->color = DirectX::XMFLOAT3{1, 1, 1};
    }
}

// ========================================================
// �V�@�\3: ForEach<T1, T2> - �����R���|�[�l���g�N�G��
// ========================================================

/**
 * @brief 2�̃R���|�[�l���g�N�G���̎g�p��
 * 
 * @details
 * �����̃R���|�[�l���g�����G���e�B�e�B�ɑ΂��Č����I�ɏ����ł��܂��B
 */
inline void DemoForEachTwoComponents(World& world) {
    // �e�X�g�f�[�^�쐬
    for (int i = 0; i < 5; ++i) {
        world.Create()
            .With<Transform>(DirectX::XMFLOAT3{static_cast<float>(i), 0, 0})
            .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
            .Build();
    }
    
    // ? �V�������@: 2�̃R���|�[�l���g�𓯎��ɏ���
    world.ForEach<Transform, MeshRenderer>(
        [](Entity e, Transform& t, MeshRenderer& r) {
            // �ʒu�ɉ����ĐF��ς���
            r.color.x = t.position.x / 10.0f;
            r.color.y = 1.0f - (t.position.x / 10.0f);
            r.color.z = 0.5f;
        }
    );
    
    // ? �Â����@�i�璷�Ŕ�����j
    world.ForEach<Transform>([&](Entity e, Transform& t) {
        auto* renderer = world.TryGet<MeshRenderer>(e);
        if (renderer) {
            renderer->color.x = t.position.x / 10.0f;
        }
    });
}

// ========================================================
// �V�@�\4: �������Z�̗�iTransform + Velocity�j
// ========================================================

/**
 * @brief �������Z�V�X�e���̗�
 * 
 * @details
 * Transform��Velocity�����G���e�B�e�B�������I�Ɉړ������܂��B
 */
inline void DemoPhysicsSystem(World& world, float dt) {
    // Velocity�R���|�[�l���g�����I�u�W�F�N�g���쐬
    Entity movingObject = world.Create()
        .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
        .With<Velocity>()
        .With<MeshRenderer>(DirectX::XMFLOAT3{0, 1, 1})
        .Build();
    
    // ���x��ݒ�
    world.Get<Velocity>(movingObject).velocity = DirectX::XMFLOAT3{5, 0, 0};
    
    // ? �������Z: Transform��Velocity�����S�G���e�B�e�B���ړ�
    world.ForEach<Transform, Velocity>(
        [dt](Entity e, Transform& t, Velocity& v) {
            t.position.x += v.velocity.x * dt;
            t.position.y += v.velocity.y * dt;
            t.position.z += v.velocity.z * dt;
        }
    );
}

// ========================================================
// �V�@�\5: GetEntityCount / GetComponentCount
// ========================================================

/**
 * @brief �f�o�b�O���擾�̗�
 * 
 * @details
 * �G���e�B�e�B����R���|�[�l���g�����擾���ăp�t�H�[�}���X���͂ł��܂��B
 */
inline void DemoDebugInfo(World& world) {
    // �G���e�B�e�B�����擾
    printf("Total entities: %zu\n", world.GetEntityCount());
    
    // ����R���|�[�l���g�̐����擾
    printf("Entities with Transform: %zu\n", world.GetComponentCount<Transform>());
    printf("Entities with MeshRenderer: %zu\n", world.GetComponentCount<MeshRenderer>());
    printf("Entities with Enemy: %zu\n", world.GetComponentCount<EnemyTag>());
    
    // �G���e�B�e�B���̐����`�F�b�N
    if (world.GetEntityCount() < 1000) {
        printf("Safe to spawn more entities\n");
    } else {
        printf("Warning: Too many entities!\n");
    }
    
#ifdef _DEBUG
    // �f�o�b�O�r���h�ł͏ڍ׏����\��
    world.PrintDebugInfo();
#endif
}

// ========================================================
// �V�@�\6: Reserve - ���O�m�ۂŃp�t�H�[�}���X����
// ========================================================

/**
 * @brief Reserve()�̎g�p��
 * 
 * @details
 * ��ʂ̃G���e�B�e�B�𐶐�����O�Ɏ��O�m�ۂ��邱�Ƃ�
 * �������Ċm�ۂ̃I�[�o�[�w�b�h���팸���܂��B
 */
inline void DemoReserve(World& world) {
    // ? 100�̂̓G�𐶐�����O�Ɏ��O�m��
    world.Reserve(100);
    
    for (int i = 0; i < 100; ++i) {
        world.Create()
            .With<Transform>(DirectX::XMFLOAT3{
                static_cast<float>(i % 10) * 2.0f,
                0,
                static_cast<float>(i / 10) * 2.0f
            })
            .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
            .With<EnemyTag>()
            .Build();
    }
    
    printf("Created 100 enemies efficiently!\n");
}

// ========================================================
// �V�@�\7: ID�ė��p�̃f��
// ========================================================

/**
 * @brief ID�ė��p�̓���m�F
 * 
 * @details
 * �G���e�B�e�B���폜�����ID���ė��p�v�[���ɓ���A
 * �����CreateEntity()�ōė��p����܂��B
 */
inline void DemoIDReuse(World& world) {
    printf("=== ID Reuse Demo ===\n");
    
    // �G���e�B�e�B��3�쐬
    Entity e1 = world.CreateEntity();
    Entity e2 = world.CreateEntity();
    Entity e3 = world.CreateEntity();
    
    printf("Created: ID=%u, %u, %u\n", e1.id, e2.id, e3.id);
    
    // 2�Ԗڂ��폜
    world.DestroyEntity(e2);
    printf("Deleted: ID=%u\n", e2.id);
    
    // �V�����G���e�B�e�B���쐬�i�폜���ꂽID���ė��p�����j
    Entity e4 = world.CreateEntity();
    printf("Created: ID=%u (reused!)\n", e4.id);
    
    printf("=====================\n");
}

// ========================================================
// ���H��: �̗̓V�X�e���Ǝ����폜
// ========================================================

/**
 * @brief �̗̓V�X�e���̎��H��
 * 
 * @details
 * Health��Enemy�����G���e�B�e�B���Ǘ����A
 * �̗͂�0�ɂȂ����玩���폜���܂��B
 */
inline void DemoHealthSystem(World& world) {
    // �G��5�̍쐬
    for (int i = 0; i < 5; ++i) {
        Entity enemy = world.Create()
            .With<Transform>(DirectX::XMFLOAT3{static_cast<float>(i) * 2.0f, 0, 0})
            .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
            .With<EnemyTag>()
            .Build();
        
        // Health��ǉ�
        Health hp;
        hp.current = 100.0f;
        hp.max = 100.0f;
        world.Add<Health>(enemy, hp);
    }
    
    printf("Created %zu enemies with health\n", world.GetComponentCount<EnemyTag>());
    
    // ? �S�Ă̓G�Ƀ_���[�W��^����
    world.ForEach<EnemyTag, Health>([](Entity e, EnemyTag&, Health& hp) {
        hp.TakeDamage(50.0f);
        printf("Enemy %u: HP = %.1f\n", e.id, hp.current);
    });
    
    // ? ���񂾓G���폜
    world.ForEach<EnemyTag, Health>([&](Entity e, EnemyTag&, Health& hp) {
        if (hp.IsDead()) {
            printf("Enemy %u died!\n", e.id);
            world.DestroyEntity(e);
        }
    });
    
    printf("Remaining enemies: %zu\n", world.GetComponentCount<EnemyTag>());
}

// ========================================================
// ��I�ȃf���V�[��
// ========================================================

/**
 * @brief ���ׂĂ̐V�@�\���g�����f���V�[��
 */
inline void RunComprehensiveDemo(World& world) {
    printf("\n========================================\n");
    printf("  World v5.0 Improvement Demo\n");
    printf("========================================\n\n");
    
    // 1. Has()�̃f��
    printf("--- Demo 1: Has() Method ---\n");
    DemoHasMethod(world);
    printf("\n");
    
    // 2. Get()�̃f��
    printf("--- Demo 2: Get() Method ---\n");
    DemoGetMethod(world);
    printf("\n");
    
    // 3. ForEach<T1,T2>�̃f��
    printf("--- Demo 3: ForEach<T1,T2> ---\n");
    DemoForEachTwoComponents(world);
    printf("\n");
    
    // 4. �������Z�̃f��
    printf("--- Demo 4: Physics System ---\n");
    DemoPhysicsSystem(world, 0.016f);
    printf("\n");
    
    // 5. �f�o�b�O���̃f��
    printf("--- Demo 5: Debug Info ---\n");
    DemoDebugInfo(world);
    printf("\n");
    
    // 6. Reserve()�̃f��
    printf("--- Demo 6: Reserve() ---\n");
    DemoReserve(world);
    printf("\n");
    
    // 7. ID�ė��p�̃f��
    printf("--- Demo 7: ID Reuse ---\n");
    DemoIDReuse(world);
    printf("\n");
    
    // 8. �̗̓V�X�e���̃f��
    printf("--- Demo 8: Health System ---\n");
    DemoHealthSystem(world);
    printf("\n");
    
    printf("========================================\n");
    printf("  All Demos Completed!\n");
    printf("========================================\n\n");
}

} // namespace WorldImprovementDemo

// ========================================================
// �g�����̗�
// ========================================================
/*

// App.cpp �܂��� main.cpp �Ŏg����

#include "samples/WorldImprovementDemo.h"

void TestWorldImprovements() {
    World world;
    
    // �ʂ̃f�������s
    WorldImprovementDemo::DemoHasMethod(world);
    WorldImprovementDemo::DemoGetMethod(world);
    WorldImprovementDemo::DemoForEachTwoComponents(world);
    
    // �܂��́A���ׂẴf�������s
    WorldImprovementDemo::RunComprehensiveDemo(world);
}

*/

// ========================================================
// �쐬��: �R���z
// �o�[�W����: v1.0 - World v5.0���P�@�\�f��
// ========================================================
