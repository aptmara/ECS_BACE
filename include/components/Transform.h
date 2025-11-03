#pragma once
#include <DirectXMath.h>

/**
 * @file Transform.h
 * @brief 位置・回転・スケールコンポーネントの定義
 * @author 山内陽
 * @date 2025
 * @version 4.0
 * 
 * @details
 * このファイルは3D空間におけるエンティティの基本的な変換情報を管理する
 * Transformコンポーネントを定義します。
 */

/**
 * @struct Transform
 * @brief 3D空間におけるエンティティの位置・回転・スケールを管理するデータコンポーネント
 * 
 * @details
 * このコンポーネントは3D空間における「場所」「向き」「大きさ」を表す基本データです。
 * すべての3Dオブジェクトに必要となる空間情報を保持します。
 * 
 * ### 座標系について:
 * - X軸: 右方向が正(カメラから見て)
 * - Y軸: 上方向が正
 * - Z軸: 奥方向が正(カメラの視線方向)
 * 
 * ### 回転の適用順序:
 * Y軸回転 → X軸回転 → Z軸回転の順で適用されます。
 * これはオイラー角の一般的な順序です。
 * 
 * @par 使用例(基本)
 * @code
 * // エンティティを作成しTransformを設定
 * Entity cube = world.Create()
 *     .With<Transform>(
 *         DirectX::XMFLOAT3{0, 5, 0},    // 位置: Y軸上に5単位
 *         DirectX::XMFLOAT3{0, 45, 0},   // 回転: Y軸中心に45度
 *         DirectX::XMFLOAT3{2, 2, 2}     // スケール: 2倍
 *     )
 *     .Build();
 * @endcode
 * 
 * @par 使用例(変更)
 * @code
 * // 既存のTransformを取得して変更
 * auto* transform = world.TryGet<Transform>(cube);
 * if (transform) {
 *     transform->position.y += 1.0f;  // 上に1単位移動
 *     transform->rotation.y += 90.0f; // さらに90度回転
 * }
 * @endcode
 * 
 * @par 使用例(アニメーション)
 * @code
 * // 毎フレーム少しずつ移動
 * world.ForEach<Transform>([&](Entity e, Transform& t) {
 *     t.position.x += 0.1f * deltaTime;  // 右に移動
 *     t.rotation.y += 45.0f * deltaTime; // 回転
 * });
 * @endcode
 * 
 * @note すべての3Dオブジェクトに推奨されるコンポーネントです
 * @warning 回転角度は度数法(0-360度)で指定します(ラジアンではありません)
 * 
 * @see MeshRenderer 描画に使用するコンポーネント
 * @see Rotator 自動回転を行うBehaviourコンポーネント
 * @see Camera カメラの位置と向きの管理
 * 
 * @author 山内陽
 */
struct Transform {
    Transform(DirectX::XMFLOAT3 pos = {0,0,5}, DirectX::XMFLOAT3 rot = {0,0,0}, DirectX::XMFLOAT3 scl = {1,1,1})
        : position(pos), rotation(rot), scale(scl) {}

    /**
     * @var position
     * @brief エンティティの3D空間における位置座標
     * 
     * @details
     * ワールド座標系における絶対位置を表します。
     * - position.x: 左右位置(負の値が左、正の値が右)
     * - position.y: 上下位置(負の値が下、正の値が上)
     * - position.z: 前後位置(負の値が手前、正の値が奥)
     * 
     * デフォルトではカメラから5単位奥(Z軸正方向)に配置されます。
     * これにより、オブジェクトがカメラの視野内に確実に配置されます。
     * 
     * @par 使用例
     * @code
     * // 原点に配置
     * transform.position = DirectX::XMFLOAT3{0, 0, 0};
     * 
     * // 上に移動
     * transform.position.y += 1.0f;
     * 
     * // 円運動
     * float angle = time * 2.0f;
     * transform.position.x = cos(angle) * 5.0f;
     * transform.position.z = sin(angle) * 5.0f;
     * @endcode
     * 
     * @note 単位系は特に定義されていませんが、通常メートル単位として扱います
     */
    DirectX::XMFLOAT3 position{ 0, 0, 5 };
    
    /**
     * @var rotation
     * @brief エンティティの回転角度(度数法)
     * 
     * @details
     * オイラー角による回転を表します。各軸周りの回転角度を度数法で指定します。
     * - rotation.x: ピッチ(上下回転、X軸周り)
     * - rotation.y: ヨー(左右回転、Y軸周り)
     * - rotation.z: ロール(横傾き回転、Z軸周り)
     * 
     * ### 回転の適用順序:
     * Y軸 → X軸 → Z軸の順で回転が適用されます(ヨー→ピッチ→ロール)。
     * 
     * ### ジンバルロックについて:
     * オイラー角を使用する場合、特定の角度でジンバルロック現象が発生する可能性があります。
     * 特にX軸が±90度付近では注意が必要です。
     * 
     * @par 使用例
     * @code
     * // Y軸中心に90度回転(右を向く)
     * transform.rotation = DirectX::XMFLOAT3{0, 90, 0};
     * 
     * // 連続回転
     * transform.rotation.y += 45.0f * deltaTime;
     * 
     * // 360度を超えたら正規化
     * if (transform.rotation.y >= 360.0f) {
     *     transform.rotation.y -= 360.0f;
     * }
     * @endcode
     * 
     * @note 角度は度数法(0-360度)で指定します
     * @warning 大きな回転を行う場合、ジンバルロックに注意してください
     */
    DirectX::XMFLOAT3 rotation{ 0, 0, 0 };
    
    /**
     * @var scale
     * @brief エンティティのスケール(拡大縮小率)
     * 
     * @details
     * 各軸方向の拡大縮小率を指定します。
     * - scale.x: X軸方向のスケール(幅)
     * - scale.y: Y軸方向のスケール(高さ)
     * - scale.z: Z軸方向のスケール(奥行き)
     * 
     * ### スケールの意味:
     * - 1.0: 標準サイズ
     * - 2.0: 2倍のサイズ
     * - 0.5: 半分のサイズ
     * - 負の値: 反転(ミラー効果)
     * 
     * 各軸独立してスケーリング可能です(非等方スケーリング)。
     * 
     * @par 使用例
     * @code
     * // 2倍のサイズ
     * transform.scale = DirectX::XMFLOAT3{2, 2, 2};
     * 
     * // 横長に変形
     * transform.scale = DirectX::XMFLOAT3{2, 1, 1};
     * 
     * // 半分のサイズ
     * transform.scale = DirectX::XMFLOAT3{0.5, 0.5, 0.5};
     * 
     * // X軸方向に反転
     * transform.scale.x = -1.0f;
     * 
     * // 脈動効果
     * float pulse = 1.0f + 0.5f * sin(time * 2.0f);
     * transform.scale = DirectX::XMFLOAT3{pulse, pulse, pulse};
     * @endcode
     * 
     * @note 各軸独立してスケーリング可能です
     * @warning 0を指定すると描画されなくなります
     * @warning 負の値を使用するとメッシュが裏返ります
     */
    DirectX::XMFLOAT3 scale{ 1, 1, 1 };
};
