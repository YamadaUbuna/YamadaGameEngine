#pragma once
#include "include/ISystem.h"

// キューブ生成システム。
// シーン内のCubeGeneratorComponentを持つエンティティを更新し、
// 一定条件で新しいキューブエンティティを生成・初期化する。

class CubeGeneratorSystem :
    public ISystem
{
public:
    void Initialize(IScene& scene) override;
    void Update(IScene& scene, float deltaTime) override;

private:
    float m_spawnTimer = 0.0f;  // 経過時間を保持
};

