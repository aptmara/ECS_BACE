/**
 * @file EnemySpawner.h
 * @brief �G�X�|�[���V�X�e��
 * @author �R���z
 * @date 2025
 * @version 1.0
 * 
 * @details
 * �����_���Ȍ`��ƐF�̓G�������I�ɃX�|�[������V�X�e���ł��B
 */
#pragma once

#include "pch.h"
#include "components/Component.h"
#include "components/Transform.h"
#include "components/MeshRenderer.h"
#include "components/Rotator.h"
#include "ecs/World.h"
#include <DirectXMath.h>
#include "util/Random.h"

/**
 * @struct EnemyTag
 * @brief �G�����ʂ���^�O�R���|�[�l���g
 * 
 * @details
 * �G���e�B�e�B���G�ł��邱�Ƃ������}�[�J�[�ł��B
 * �Փ˔����ForEach�����œG���i�荞�ނ��߂Ɏg�p���܂��B
 * 
 * @author �R���z
 */
struct EnemyTag : IComponent {};

/**
 * @struct EnemyMovement
 * @brief �G�̈ړ�Behaviour
 * 
 * @details
 * �G���������Ɉړ������A��ʊO�ɏo���玩���I�ɍ폜���܂��B
 * 
 * @par �g�p��
 * @code
 * Entity enemy = world.Create()
 *     .With<Transform>(DirectX::XMFLOAT3{0, 5, 0})
 *     .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
 *     .With<EnemyTag>()
 *     .With<EnemyMovement>()
 *     .Build();
 * @endcode
 * 
 * @author �R���z
 */
struct EnemyMovement : Behaviour {
    float speed = 2.0f;  ///< �������ւ̈ړ����x
    float destroyY = -10.0f;  ///< ���̍����ȉ��ɂȂ�����폜
    
    /**
     * @brief ���t���[���X�V����
     * @param[in,out] w ���[���h�Q��
     * @param[in] self ���g�̃G���e�B�e�B
     * @param[in] dt �f���^�^�C��(�b�P��)
     */
    void OnUpdate(World& w, Entity self, float dt) override {
        auto* t = w.TryGet<Transform>(self);
        if (!t) return;
        
        // �������Ɉړ�
        t->position.y -= speed * dt;
        
        // ��ʉ��ɏo����폜
        if (t->position.y < destroyY) {
            w.DestroyEntityWithCause(self, World::Cause::LifetimeExpired);
        }
    }
};

/**
 * @struct EnemySpawner
 * @brief �G�����I�ɃX�|�[������Behaviour
 * 
 * @details
 * �w�肳�ꂽ�Ԋu�Ń����_���Ȍ`��E�F�E�ʒu�̓G�𐶐����܂��B
 * 
 * ### �X�|�[�������G�̓���:
 * - �����_���Ȍ`��(Cube, Sphere, Cylinder, Cone, Capsule)
 * - �����_���ȐF
 * - �����_����X���W
 * - �����_���ȉ�]���x
 * 
 * @par �g�p��
 * @code
 * // �V�[������������1�����X�|�[�i�[��z�u
 * Entity spawner = world.Create()
 *     .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
 *     .With<EnemySpawner>()
 *     .Build();
 * @endcode
 * 
 * @note ���̃R���|�[�l���g��1�̃G���e�B�e�B��1�����t���邱�Ƃ𐄏�
 * 
 * @author �R���z
 */
struct EnemySpawner : Behaviour {
    float spawnInterval = 1.5f;  ///< �X�|�[���Ԋu(�b)
    float timer = 0.0f;          ///< �����^�C�}�[
    float spawnY = 10.0f;        ///< �X�|�[���ʒu��Y���W
    float spawnRangeX = 8.0f;    ///< �X�|�[���͈͂̕�(-spawnRangeX ~ +spawnRangeX)
    
    /**
     * @brief ����N�����̏���
     * @param[in,out] w ���[���h�Q��
     * @param[in] self ���g�̃G���e�B�e�B
     */
    void OnStart(World& w, Entity self) override {
        // ���i�������������ŃV�[�h�i����̂݁j
        util::Random::SeedTime();
    }
    
    /**
     * @brief ���t���[���X�V����
     * @param[in,out] w ���[���h�Q��
     * @param[in] self ���g�̃G���e�B�e�B
     * @param[in] dt �f���^�^�C��(�b�P��)
     */
    void OnUpdate(World& w, Entity self, float dt) override {
        timer += dt;
        
        // �X�|�[���Ԋu�ɒB������G�𐶐�
        if (timer >= spawnInterval) {
            timer = 0.0f;
            SpawnEnemy(w);
        }
    }
    
private:
    /**
     * @brief �����_���ȓG�𐶐�
     * @param[in,out] w ���[���h�Q��
     */
    void SpawnEnemy(World& w) {
        // �����_����X���W
        float randomX = util::Random::Float(-spawnRangeX, spawnRangeX);
        
        // �����_���Ȍ`��(0-4: Cube, Sphere, Cylinder, Cone, Capsule)
        int shapeIndex = util::Random::Int(0, 4);
        if (shapeIndex >= static_cast<int>(MeshType::Plane)) {
            shapeIndex++;  // Plane���X�L�b�v
        }
        MeshType randomShape = static_cast<MeshType>(shapeIndex);
        
        // �����_���ȐF(����߂̐F)
        DirectX::XMFLOAT3 randomColor = util::Random::ColorBright();
        
        // �����_���ȉ�]���x
        float randomRotSpeed = util::Random::Float(30.0f, 130.0f) * (util::Random::Bool() ? 1.0f : -1.0f);
        
        // �����_���ȃX�P�[��(0.8�`1.5�{)
        float randomScale = util::Random::Float(0.8f, 1.5f);
        
        // Transform�ݒ�
        Transform enemyTransform;
        enemyTransform.position = DirectX::XMFLOAT3{randomX, spawnY, 0.0f};
        enemyTransform.rotation = DirectX::XMFLOAT3{0.0f, 0.0f, 0.0f};
        enemyTransform.scale = DirectX::XMFLOAT3{randomScale, randomScale, randomScale};
        
        // MeshRenderer�ݒ�
        MeshRenderer enemyRenderer;
        enemyRenderer.meshType = randomShape;
        enemyRenderer.color = randomColor;
        
        // �G�G���e�B�e�B���쐬
        Entity enemy = w.Create()
            .With<Transform>(enemyTransform)
            .With<MeshRenderer>(enemyRenderer)
            .With<EnemyTag>()
            .WithCause<EnemyMovement>(World::Cause::Spawner)
            .WithCause<Rotator>(World::Cause::Spawner, randomRotSpeed)
            .Build();
    }
};

/**
 * @struct WaveSpawner
 * @brief �E�F�[�u�`���œG���X�|�[������Behaviour
 * 
 * @details
 * ��莞�Ԃ��Ƃɕ����̓G����x�ɃX�|�[�����܂��B
 * �E�F�[�u�Q�[���ȂǂɎg�p�ł��܂��B
 * 
 * @par �g�p��
 * @code
 * Entity spawner = world.Create()
 *     .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
 *     .With<WaveSpawner>()
 *     .Build();
 * @endcode
 * 
 * @author �R���z
 */
struct WaveSpawner : Behaviour {
    float waveInterval = 5.0f;   ///< �E�F�[�u�Ԋu(�b)
    int enemiesPerWave = 5;      ///< 1�E�F�[�u������̓G��
    float timer = 0.0f;          ///< �����^�C�}�[
    int currentWave = 0;         ///< ���݂̃E�F�[�u�ԍ�
    
    void OnStart(World& w, Entity self) override {
        util::Random::SeedTime();
    }
    
    void OnUpdate(World& w, Entity self, float dt) override {
        timer += dt;
        
        if (timer >= waveInterval) {
            timer = 0.0f;
            currentWave++;
            SpawnWave(w);
        }
    }
    
private:
    void SpawnWave(World& w) {
        for (int i = 0; i < enemiesPerWave; ++i) {
            // �����тɔz�u
            float spacing = 2.5f;
            float startX = -(enemiesPerWave - 1) * spacing * 0.5f;
            float x = startX + i * spacing;
            
            // �����_���Ȍ`��
            int shapeIndex = util::Random::Int(0, 4);
            if (shapeIndex >= static_cast<int>(MeshType::Plane)) {
                shapeIndex++;
            }
            MeshType shape = static_cast<MeshType>(shapeIndex);
            
            // �E�F�[�u���ƂɐF�̃e�[�}��ς���
            DirectX::XMFLOAT3 color;
            switch (currentWave % 3) {
                case 0: // �Ԍn
                    color = DirectX::XMFLOAT3{1.0f, 0.3f, 0.3f};
                    break;
                case 1: // �Όn
                    color = DirectX::XMFLOAT3{0.3f, 1.0f, 0.3f};
                    break;
                default: // �n
                    color = DirectX::XMFLOAT3{0.3f, 0.3f, 1.0f};
                    break;
            }
            
            Transform t;
            t.position = DirectX::XMFLOAT3{x, 10.0f, 0.0f};
            t.scale = DirectX::XMFLOAT3{1.0f, 1.0f, 1.0f};
            
            MeshRenderer mr;
            mr.meshType = shape;
            mr.color = color;
            
            Entity enemy = w.Create()
                .With<Transform>(t)
                .With<MeshRenderer>(mr)
                .With<EnemyTag>()
                .WithCause<EnemyMovement>(World::Cause::WaveTimer)
                .WithCause<Rotator>(World::Cause::WaveTimer, 60.0f)
                .Build();
        }
    }
};

