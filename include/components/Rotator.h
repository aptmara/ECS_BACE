#pragma once
#include "components/Component.h"
#include "ecs/Entity.h"
#include "ecs/World.h"
#include "components/Transform.h"
#include <DirectXMath.h>

/**
 * @file Rotator.h
 * @brief 自動回転Behaviourコンポーネントの定義
 * @author 山内陽
 * @date 2025
 * @version 5.0
 *
 * @details
 * このファイルは、エンティティを自動的に回転させるBehaviourコンポーネントを定義します。
 * Behaviourコンポーネントの実装例として、初学者の学習に最適です。
 */

/**
 * @struct Rotator
 * @brief エンティティを自動的にY軸中心で回転させるBehaviourコンポーネント
 *
 * @details
 * このコンポーネントをエンティティに追加すると、毎フレーム自動的に
 * Y軸（上下軸）を中心に回転します。Behaviourコンポーネントの基本的な
 * 実装例として、学習に最適です。
 *
 * ### Behaviourの仕組み:
 * 1. 毎フレーム、OnUpdate()メソッドが自動的に呼ばれる
 * 2. OnUpdate()内で、自身のTransformコンポーネントを取得
 * 3. Transformのrotation.yに角度を加算して回転
 *
 * ### dt（デルタタイム）の重要性:
 * dtは前フレームからの経過秒数です。これを掛けることで、
 * フレームレートに依存しない安定した動きを実現できます。
 *
 * @par 使用例（基本）:
 * @code
 * // 毎秒45度で回転するキューブを作成
 * Entity cube = world.Create()
 *     .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
 *     .With<MeshRenderer>(DirectX::XMFLOAT3{1, 0, 0})
 *     .With<Rotator>(45.0f)  // 毎秒45度
 *     .Build();
 * @endcode
 *
 * @par 使用例（高速回転）:
 * @code
 * // 毎秒180度で高速回転
 * Entity fastCube = world.Create()
 *     .With<Transform>(DirectX::XMFLOAT3{0, 0, 0})
 *     .With<MeshRenderer>(DirectX::XMFLOAT3{0, 1, 0})
 *     .With<Rotator>(180.0f)
 *     .Build();
 * @endcode
 *
 * @par 使用例（実行時の速度変更）:
 * @code
 * // 実行中に回転速度を変更
 * auto* rotator = world.TryGet<Rotator>(entity);
 * if (rotator) {
 *     rotator->speedDegY = 90.0f;  // 毎秒90度に変更
 * }
 * @endcode
 *
 * @note Transformコンポーネントと組み合わせて使用する必要があります
 * @see Behaviour Behaviourコンポーネントの基底クラス
 * @see Transform 位置・回転・スケールコンポーネント
 *
 * @author 山内陽
 */
struct Rotator : Behaviour {
    /**
     * @var speedDegY
     * @brief Y軸中心の回転速度（度/秒）
     *
     * @details
     * 毎秒何度回転するかを指定します。
     * - 正の値: 時計回り（右回り）
     * - 負の値: 反時計回り（左回り）
     * - 0: 回転しない
     *
     * @par 参考値:
     * - 45.0f: ゆっくり回転（8秒で1周）
     * - 90.0f: 普通の速度（4秒で1周）
     * - 180.0f: 速い回転（2秒で1周）
     * - 360.0f: 高速回転（1秒で1周）
     *
     * @note デフォルトは45.0（毎秒45度）
     */
    float speedDegY = 45.0f;

    /**
     * @brief デフォルトコンストラクタ
     * @details 回転速度を45.0度/秒に設定します
     */
    Rotator() = default;

    /**
     * @brief 回転速度を指定するコンストラクタ
     *
     * @param[in] s 回転速度（度/秒）
     *
     * @par 使用例:
     * @code
     * Rotator slowRotator(30.0f);   // 遅い回転
     * Rotator fastRotator(120.0f);  // 速い回転
     * @endcode
     */
    explicit Rotator(float s) : speedDegY(s) {}

    /**
     * @brief 毎フレーム呼ばれる更新処理
     *
     * @param[in,out] w ワールドへの参照（コンポーネント取得に使用）
     * @param[in] self このコンポーネントが付いているエンティティ
     * @param[in] dt デルタタイム（前フレームからの経過秒数）
     *
     * @details
     * この関数が毎フレーム自動的に呼ばれ、以下の処理を行います：
     * 1. 自身のTransformコンポーネントを取得
     * 2. rotation.yに speedDegY * dt を加算して回転
     * 3. 360度を超えたら正規化（0～360度の範囲に収める）
     *
     * @note dtを掛けることで、フレームレートに依存しない動きを実現
     *
     * @par 処理の詳細:
     * @code
     * // 例: speedDegY = 45.0f, dt = 0.016秒（60FPS）の場合
     * rotation.y += 45.0f * 0.016f = 0.72度加算
     *
     * // 60FPSで動作すると、1秒間に
     * // 60フレーム * 0.72度 = 約43.2度回転（誤差は浮動小数点演算による）
     * @endcode
     */
    void OnUpdate(World& w, Entity self, float dt) override {
        // このエンティティのTransformを取得
        auto* t = w.TryGet<Transform>(self);
        if (!t) return; // Transformがなければ何もしない

        // 回転値を更新（dt = デルタタイム = 前フレームからの経過秒数）
        t->rotation.y += speedDegY * dt;

        // 360度を超えたら正規化（見やすくするため、なくてもOK）
        while (t->rotation.y >= 360.0f) t->rotation.y -= 360.0f;
        while (t->rotation.y < 0.0f) t->rotation.y += 360.0f;
    }
};
