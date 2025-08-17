#pragma once
#include "include/ISystem.h"

// exampleSystemはexampleComponentを持つエンティティの初期化と更新を担当します。
// 初期化では位置とマテリアルの色を設定し、更新ではエンティティを右方向に移動させます。
// 位置が一定値を超えるとエンティティを破棄します。

class exampleSystem :
    public ISystem
{
public:
    void Initialize(IScene& scene) override;
    void Initialize(std::unique_ptr<IEntity>& entity, IScene& scene);
    void Update(IScene& scene, float deltaTime) override;
};

