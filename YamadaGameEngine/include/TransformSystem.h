#pragma once
#include "include/ISystem.h"

// TransformComponentの回転情報から前方・上・右方向ベクトルを計算し、
// 各エンティティのTransformを更新するシステムです。
// スケール・回転・平行移動を組み合わせたワールド行列も生成します。
// 毎フレーム、全エンティティのTransformを更新して反映します。

class TransformSystem :
    public ISystem
{
public:
    void Initialize(IScene& scene) override;
    void Update(IScene& scene, float deltaTime) override;
};

