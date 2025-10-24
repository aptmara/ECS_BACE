````markdown
# GitHub Copilot �J�X�^���w�� - HEW_GAME �v���W�F�N�g

���̃t�@�C���́AGitHub Copilot ���{�v���W�F�N�g�i**HEW_GAME**�j�ŃR�[�h�𐶐��E�C������ۂɏ]���ׂ� **�Z�p�K��E�^�p�t���[** ���`���܂��B  
**��K�͕ύX�O�ɂ͕K���u�v���W�F�N�g�Ǎ� �� �v�旧�� �� ���r���[���F�v���o�邱�ƁB**

---

## �v���W�F�N�g�T�v

- **�v���W�F�N�g��**: HEW_GAME
- **�ړI**: Entity Component System (ECS) �����p�����`�[���Q�[���J��
- **����**: C++14�i����j
- **�v���b�g�t�H�[��**: Windows / DirectX 11
- **�A�[�L�e�N�`��**: Entity Component System
- **�J���X�^�C��**: Git/GitHub�iDraft PR + ���F�Q�[�g�^�p�j

---

## �d�v�Ȑ��񎖍��iC++ �W���j

### �g�p�\�iC++14�j
```cpp
// auto �^���_
auto entity = world.CreateEntity();

// �����_��
world.ForEach<Transform>([](Entity e, Transform& t) {
    t.position.x += 1.0f;
});

// �X�}�[�g�|�C���^
std::unique_ptr<Component> component;
std::shared_ptr<Resource> resource;

// �͈� for
for (const auto& entity : entities) { }

// ���������X�g
DirectX::XMFLOAT3 position{0.0f, 0.0f, 0.0f};
````

### �g�p�֎~�iC++17 �ȍ~�j

```cpp
// std::optional�iC++17�j
std::optional<Transform> GetTransform(Entity e);  // NG

// if constexpr�iC++17�j
if constexpr (std::is_same_v<T, Transform>) { }   // NG

// std::filesystem�iC++17�j
std::filesystem::path filePath;                   // NG

// �\���������iC++17�j
auto [x, y, z] = GetPosition();                   // NG

// �C�����C���ϐ��iC++17�j
inline constexpr int MAX_ENTITIES = 1000;         // NG
```

**�Ώ����j�iC++14 �݊��j**

```cpp
// ��̓|�C���^�ő�ցi���L����World�j
Transform* GetTransform(Entity e) { return world.TryGet<Transform>(e); }

// �^����̓e���v���[�g���ꉻ
template<typename T> void Process(T& component);

// ������Win32 API or boost::filesystem�i���p�Ȃ�j���g�p
#include <windows.h>
```

---

## ECS �A�[�L�e�N�`���̌���

### Entity�i���ʎq�̂݁j

```cpp
struct Entity {
    uint32_t id;   // �G���e�B�e�BID
    uint32_t gen;  // ����ԍ��i�폜/�ė��p�Ǘ��j
};
```

**�֎~**: Entity �Ƀ��W�b�N���Ԃ��������Ȃ��B

### Component�i2 ��ށj

1. **�f�[�^�R���|�[�l���g�iIComponent �p���j**: �f�[�^�ێ��{�y���w���p�̂�

```cpp
struct Health : IComponent {
    float current = 100.0f;
    float max = 100.0f;

    void TakeDamage(float dmg) {
        current -= dmg;
        if (current < 0.0f) current = 0.0f;
    }
    bool IsDead() const { return current <= 0.0f; }
};
```

2. **Behaviour �R���|�[�l���g�iBehaviour �p���j**: ���t���[���X�V����郍�W�b�N

```cpp
struct Rotator : Behaviour {
    float speedDegY = 45.0f;
    void OnUpdate(World& w, Entity self, float dt) override {
        auto* t = w.TryGet<Transform>(self);
        if (t) t->rotation.y += speedDegY * dt;
    }
};
```

### System �����p�^�[��

* **Behaviour �p�^�[���i�����j**: �e Entity �ɐU�镑���𑕒�
* **ForEach �p�^�[��**: �f�[�^�w���Ɉꊇ����

```cpp
void UpdateMovementSystem(World& world, float dt) {
    world.ForEach<Transform, Velocity>([dt](Entity, Transform& t, Velocity& v) {
        t.position.x += v.velocity.x * dt;
        t.position.y += v.velocity.y * dt;
        t.position.z += v.velocity.z * dt;
    });
}
```

---

## �R�[�f�B���O�K��

| �v�f      | �K��                     | ��                                          |
| ------- | ---------------------- | ------------------------------------------ |
| �N���X/�\���� | PascalCase             | `Transform`, `MeshRenderer`, `World`       |
| �֐�      | PascalCase             | `CreateEntity()`, `TryGet()`, `OnUpdate()` |
| �ϐ�      | camelCase              | `deltaTime`, `entityId`, `speed`           |
| �����o�ϐ�   | camelCase + `_` �T�t�B�b�N�X | `world_`, `nextId_`                        |
| �萔      | UPPER_SNAKE_CASE       | `MAX_ENTITIES`, `DEFAULT_SPEED`            |

```cpp
class PlayerController : public Behaviour {
public:
    float speed = 5.0f;
    void OnUpdate(World& w, Entity self, float dt) override {
        auto* transform = w.TryGet<Transform>(self);
        if (transform) transform->position.x += speed * dt;
    }
private:
    InputSystem* input_ = nullptr;
};
```

---

## World �N���X�̎g�p

### �G���e�B�e�B�쐬�i�r���_�[�p�^�[�������j

```cpp
Entity player = world.Create()
    .With<Transform>(DirectX::XMFLOAT3{0,0,0})
    .With<MeshRenderer>(DirectX::XMFLOAT3{0,1,0})
    .With<Rotator>(45.0f)
    .With<PlayerTag>()
    .Build();
```

### �R���|�[�l���g����

```cpp
// ���S�擾
if (auto* t = world.TryGet<Transform>(entity)) { t->position.x += 1.0f; }

// �ǉ�/���݊m�F/�폜
world.Add<Health>(entity, Health{100.0f, 100.0f});
if (world.Has<Transform>(entity)) { /* ... */ }
world.Remove<Health>(entity);

// �폜�i�����t�����O�j
world.DestroyEntityWithCause(entity, World::Cause::Collision);
```

### ForEach ���p

```cpp
world.ForEach<Transform>([](Entity, Transform& t) { t.position.y += 0.1f; });
world.ForEach<PlayerTag, Transform>([](Entity, PlayerTag&, Transform& t) { /* �v���C���̂� */ });
```

---

## DirectXMath �̎g�p

**�ێ��� `XMFLOAT3/4`�A�v�Z�� `XMVECTOR`**

```cpp
void MoveTowards(Transform& t, const DirectX::XMFLOAT3& target, float speed) {
    using namespace DirectX;
    XMVECTOR pos = XMLoadFloat3(&t.position);
    XMVECTOR tgt = XMLoadFloat3(&target);
    XMVECTOR dir = XMVectorSubtract(tgt, pos);
    dir = XMVector3Normalize(dir);
    XMVECTOR move = XMVectorScale(dir, speed);
    XMStoreFloat3(&t.position, XMVectorAdd(pos, move));
}
```

---

## �h�L�������e�[�V�����K��iDoxygen�j

```cpp
/**
 * @file MyComponent.h
 * @brief �R���|�[�l���g�̐���
 * @author ...
 * @date 2025
 * @version 6.0
 */
```

�֐��E�\���̂ɂ� `@brief`, `@details`, `@note`, `@warning`, `@param`, `@return` ��K�X�t�^�B

---

## �֎~�����i�A�[�L�j��/�񐄏��p�^�[���j

* Entity �Ƀ��W�b�N/��Ԃ�ǉ����Ȃ�
* �O���[�o���ŃG���e�B�e�B�Ǘ����Ȃ��iWorld �Ǘ��j
* �R���|�[�l���g����ʃR���|�[�l���g�ւ�**���ڐ��|�C���^�ێ�**�֎~�iWorld �o�R�Ŏ擾�j
* Update ���œ����I�ɑ�ʃX�|�[��/�j�����Ȃ��i**Enqueue** ���g�p�j
* ���t���[���̕s�v�ȓ��I�m�ۂ�����A�����o�ė��p

---

## �f�o�b�O�ƃ��O�i�K��j

```cpp
#include "app/DebugLog.h"

// ���x����
DEBUGLOG("Entity created: ID=%u", entity.id);
DEBUGLOG_WARNING("Transform not found on entity %u", entity.id);
DEBUGLOG_ERROR("Failed to load resource: %s", resourceName.c_str());

// _DEBUG �߂ł̏����t�����O
#ifdef _DEBUG
DEBUGLOG("Delta time = %f", deltaTime);
#endif
```

**�Z�L�����e�B/�^�p**

* PII/�V�[�N���b�g�i��/�g�[�N��/���[���j���o�͂��Ȃ�
* �X���b�g�����O�F���ꃁ�b�Z�[�W�� 1 �b�� 1 ��܂Łi�Ăяo�����Ő���j
* `INFO`, `WARN`, `ERROR` �ȊO�� `_DEBUG` �r���h����

---

## �}�N�����p

### DEFINE_DATA_COMPONENT

```cpp
DEFINE_DATA_COMPONENT(Score,
    int points = 0;
    void AddPoints(int p) { points += p; }
    void Reset() { points = 0; }
);
```

### DEFINE_BEHAVIOUR

```cpp
DEFINE_BEHAVIOUR(CircularMotion,
    float radius = 3.0f;
    float speed = 1.0f;
    float angle = 0.0f;
,
    angle += speed * dt;
    if (auto* t = w.TryGet<Transform>(self)) {
        t->position.x = cosf(angle) * radius;
        t->position.z = sinf(angle) * radius;
    }
);
```

---

## �ύX�v��Ə��F�t���[�iLarge Change Gate�j

**�ړI**: �e���̑傫���C���ɑ΂��A**����O�́u�v�恨���r���[�����F�v��K�{��**���A�݌v����⍷���߂���h�~�B

### ��K�͕ύX�̔����i�ȉ��̂����ꂩ�j

* �ύX�s���i���Z+�폜�j > **300**
* **�V�K�t�@�C�� 3 �ȏ�** �܂��� **���J API/�R���|�[�l���g�̃V�O�l�`���ύX**
* **Core �̈�**�i`include/ecs/*`, `include/components/*`�j�ɐG���
* **�X���b�h���f��/���C�t�T�C�N��**�։e���i�X�|�[��/�j��/�X�V����/�����j
* **�r���h�ݒ�/�ˑ��ǉ�** ���܂�

### ���s�菇

1. **�R�[�h�x�[�X�Ǎ�**�i�֘A `.h/.cpp`�A�Ăяo���֌W�A�ˑ��j
2. **�v��쐬�iPLAN.md �����j** ? �{���e���v���g�p
3. **�h���t�g PR �쐬�iDraft �w��j** ? PR �{���� PLAN.md ��\�t�A���r���[�A�w��
4. **���F�Q�[�g** ? ���r���[�A�� `APPROVED: <���� or �`�[��>` �R�����g��t����܂ŁA**�����̓X�P���g��/�X�^�u�ŏ���**
5. **�����J�n** ? �v�悩�����ꍇ�� PLAN.md �X�V �� **�ď��F** �K�{

### �֎~����

* `APPROVED:` �R�����g **�O** �ɑ�K�͉��ς��}�[�W/Push
* Core �̈�̖��f�ҏW�iHotfix �� `HOTFIX:` �`�P�b�g + �ŏ������Ɍ���j

---

## PLAN.md �e���v���[�g�i�\�t�p�j

```md
# PLAN.md ? �ύX�v��

## �T�v
- �^�C�g��: <�ύX��>
- �ړI: <���[�U���l/���\/�ێ琫>
- �X�R�[�v: <�ΏۃV�[��/�V�X�e��/�R���|�[�l���g>

## �e���͈�
- ���� API/ABI: <�L/�� + �ڍ�>
- �ύX�\��t�@�C��:
  - include/...
  - src/...

## �݌v���j
- �A�[�L�e�N�`��: <ECS �����ɉ���������>
- �f�[�^�t���[/�X�V����: <OnStart/OnUpdate/ForEach>
- ��ֈĔ�r: <��A/��B/�s�̗p���R>

## ���s���[�h�Ɗɘa��
- ����/����: <EnqueueSpawn/Destroy�A�C�e���[�^�������h�~>
- ���C�t�T�C�N��: <Entity ����/�Q�Ǝ���>
- ���l���萫: <���K���O�̒����`�F�b�N/NaN�΍�>

## �}�C���X�g�[��
- M1: �X�P���g�������i�e�X�g�ʉ߁j
- M2: �@�\A
- M3: �@�\B/���׎���

## �v��/����
- ���\�w�W: <�t���[�����ԁAForEach ��������>
- �e�X�g: <�P��/����/�V�[����A>

## ���[���o�b�N
- �菇: <Revert/Feature Flag>

## ���r���[�˗�
- ���r���[�A: <�S����>
- ���F�g�[�N��: `APPROVED: <����>`
```

---

## Copilot �O�i�^�X�N�i��K�͕ύX���͕K�{�j

* �v���W�F�N�g�𑖍����A�֘A�t�@�C��/�Ăяo���֌W/�ˑ����
* ��L **PLAN.md** �����������i�e���v������j
* Draft PR �{���� PLAN.md �𖄂ߍ��ރe�L�X�g���o��
* ���F�҂��R�����g�������}��:
  `READY FOR REVIEW ? Reply with "APPROVED: <name>" to proceed.`
* `APPROVED:` ���t���܂�**�����R�[�h�����̓X�P���g��/�C���^�t�F�[�X�݂̂Ɍ���**

---

## �ύX�����̈�i�v���F�j

* `include/ecs/World.h`
* `include/ecs/Entity.h`
* `include/components/Component.h`
* `include/components/Transform.h`
* `include/components/MeshRenderer.h`

**�K��**: ��L�ɐG��� PR �� **�������� PLAN.md �K�{**�B`APPROVED:` �t�^�O�� Draft �̂܂܁B

---

## C++14 ��փp�^�[���i�֐��u���b�N�j

```cpp
// Optional �̑�ցi�ؗp�����j
template<typename T>
T* TryGetBorrowed(World& w, Entity e) { return w.TryGet<T>(e); }

struct MaybeTransform { Transform* ptr; bool has; };
inline MaybeTransform GetMaybeTransform(World& w, Entity e) {
    Transform* t = w.TryGet<Transform>(e);
    return { t, t != nullptr };
}
```

---

## DirectXMath ���S�K�[�h�i���S�`�j

```cpp
static inline void MoveTowardsSafe(Transform& t, const DirectX::XMFLOAT3& target, float speed, float dt) {
    using namespace DirectX;
    XMVECTOR pos = XMLoadFloat3(&t.position);
    XMVECTOR tgt = XMLoadFloat3(&target);
    XMVECTOR delta = XMVectorSubtract(tgt, pos);

    // �[�������
    XMVECTOR lenSq = XMVector3LengthSq(delta);
    if (XMVectorGetX(lenSq) < 1e-12f) return;

    // �P�ʌn: speed[m/s] * dt[s]
    XMVECTOR dir = XMVector3Normalize(delta);
    XMVECTOR step = XMVectorScale(dir, speed * dt);
    XMStoreFloat3(&t.position, XMVectorAdd(pos, step));
}
```

---

## �`�[���J�����[��

### �t�@�C���ҏW�̗D�揇��

* **�R�A�i�G��Ȃ�/�v���F�j**: �O�߁u�ύX�����̈�v�Q��
* **���R�ɕҏW**: `include/scenes/`, `include/components/Custom*.h`, `src/`
* **�v���k**: `include/graphics/`, `include/input/`, `include/app/`

### Git �R�~�b�g���b�Z�[�W

```bash
# �ǂ���i�^�{���e�j
git commit -m "feat: Add player shooting system"
git commit -m "fix: Resolve collision detection NaN at zero-length"
git commit -m "docs: Update README with component guide"
git commit -m "perf: Optimize render loop"
git commit -m "refactor: Restructure component stores"
```

---

## �Q�l�t�@�C��

* `include/samples/ComponentSamples.h` ? �R���|�[�l���g������
* `include/samples/SampleScenes.h` ? �V�[��������
* `include/scenes/MiniGame.h` ? ���K�̓Q�[������
* `include/ecs/World.h` ? ���p�K�C�h/�C���^�t�F�[�X

---

## �R�[�h�����`�F�b�N���X�g�iCopilot �p�j

* [ ] C++14 �����iC++17 �@�\�͕s�g�p�j
* [ ] ECS �̕����iEntity / Component / System�j
* [ ] �R���|�[�l���g�� `IComponent` �܂��� `Behaviour` ���p��
* [ ] �G���e�B�e�B�쐬�̓r���_�[�p�^�[���i�����j
* [ ] �|�C���^�擾�� `TryGet` ��p���� null �`�F�b�N
* [ ] �����K��iPascalCase / camelCase / `_` �ڔ����j����
* [ ] Doxygen �R�����g�t�^
* [ ] DirectXMath �̌^�iXMFLOAT3 ���j�ƌv�Z�菇�̕���
* [ ] �O���[�o���Ǘ�������AWorld �Ǘ�
* [ ] ���[�N�����iWorld �����L�������j/ ���C�t�T�C�N�����S
* [ ] **��K�͕ύX���� PLAN.md + Draft PR + `APPROVED:` ���F���m�F**

---

## �쐬�ҁE���^���

* **�쐬��**: �R���z
* **�ŏI�X�V**: 2025
* **�o�[�W����**: v6.0 ? �`�[���Q�[���J���t���[�����[�N�iHEW_GAME �Ή��j

---