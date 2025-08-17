#pragma once
#include "include/ISystem.h"

// 落下キューブの制御システム。
// スペースキーで落下開始、一定位置で停止して処理を行い、新しいキューブ生成やゲームオーバー判定を行う。

class DropCubeSystem :
    public ISystem
{
public:
    void Initialize(IScene& scene) override;
    void Update(IScene& scene, float deltaTime) override;

private:
    bool AreAllCubesInRange(IScene& scene);

    bool hit = false;
};

