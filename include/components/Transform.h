#pragma once
#include <DirectXMath.h>

/**
 * @file Transform.h
 * @brief 位置・回転・スケールコンポーネントの定義
 * @author 山内陽
 * @date 2024
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
 * - X軸: 右方向が正（カメラから見て）
 * - Y軸: 上方向が正
 * - Z軸: 奥方向が正（カメラの視線方向）
 * 
 * ### 回転の適用順序:
 * Y軸回転 → X軸回転 → Z軸回転の順で適用されます
 * 
 * ### 使用例:
 * @code
 * // エンティティを作成し、Transformを設定
 * Entity cube = world.Create()
 *     .With<Transform>(
 *         DirectX::XMFLOAT3{0, 5, 0},    // 位置: Y軸上に5単位
 *         DirectX::XMFLOAT3{0, 45, 0},   // 回転: Y軸中心に45度
 *         DirectX::XMFLOAT3{2, 2, 2}     // スケール: 2倍
 *     )
 *     .Build();
 * 
 * // 既存のTransformを取得して変更
 * auto* transform = world.TryGet<Transform>(cube);
 * if (transform) {
 *     transform->position.y += 1.0f;  // 上に1単位移動
 *     transform->rotation.y += 90.0f; // さらに90度回転
 * }
 * @endcode
 * 
 * @note すべての3Dオブジェクトに推奨されるコンポーネントです
 * @warning 回転角度は度数法（0-360度）で指定します（ラジアンではありません）
 * 
 * @see MeshRenderer 描画に使用するコンポーネント
 * @see Rotator 自動回転を行うBehaviourコンポーネント
 * 
 * @author 山内陽
 */
struct Transform {
    /**
     * @var position
     * @brief エンティティの3D空間における位置座標
     * 
     * @details
     * ワールド座標系における絶対位置を表します。
     * - position.x: 左右位置（負の値が左、正の値が右）
     * - position.y: 上下位置（負の値が下、正の値が上）
     * - position.z: 前後位置（負の値が手前、正の値が奥）
     * 
     * デフォルトではカメラから5単位奥（Z軸正方向）に配置されます。
     * 
     * @note 単位系は特に定義されていませんが、通常メートル単位として扱います
     */
    DirectX::XMFLOAT3 position{ 0, 0, 5 };
    
    /**
     * @var rotation
     * @brief エンティティの回転角度（度数法）
     * 
     * @details
     * オイラー角による回転を表します。各軸周りの回転角度を度数法で指定します。
     * - rotation.x: ピッチ（上下回転、X軸周り）
     * - rotation.y: ヨー（左右回転、Y軸周り）
     * - rotation.z: ロール（横転回転、Z軸周り）
     * 
     * 回転の適用順序: Y軸 → X軸 → Z軸
     * 
     * @note 角度は度数法（0-360度）で指定します
     * @warning 大きな回転を行う場合、ジンバルロックに注意してください
     * 
     * @par 使用例:
     * @code
     * transform.rotation = DirectX::XMFLOAT3{0, 90, 0};  // Y軸中心に90度回転
     * @endcode
     */
    DirectX::XMFLOAT3 rotation{ 0, 0, 0 };
    
    /**
     * @var scale
     * @brief エンティティのスケール（拡大縮小率）
     * 
     * @details
     * 各軸方向の拡大縮小率を指定します。
     * - scale.x: X軸方向のスケール（幅）
     * - scale.y: Y軸方向のスケール（高さ）
     * - scale.z: Z軸方向のスケール（奥行き）
     * 
     * 1.0が等倍、2.0が2倍、0.5が半分のサイズになります。
     * 負の値を指定すると反転します。
     * 
     * @note 各軸独立してスケーリング可能です
     * @warning 0を指定すると描画されなくなります
     * 
     * @par 使用例:
     * @code
     * transform.scale = DirectX::XMFLOAT3{2, 1, 1};  // 横幅だけ2倍
     * transform.scale = DirectX::XMFLOAT3{0.5, 0.5, 0.5};  // 全体を半分のサイズに
     * @endcode
     */
    DirectX::XMFLOAT3 scale{ 1, 1, 1 };
};
