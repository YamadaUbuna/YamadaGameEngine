#pragma once
#include "include/IScene.h"

class IScene;

class ISystem {
public:
    virtual ~ISystem() = default;

    // ロジック処理（物理演算、アニメーションなど）
    virtual void Initialize(IScene& scene) = 0;
    virtual void Update(IScene& scene, float deltaTime) = 0;

    // 描画処理（MeshRendererなど）
    //virtual void Render(IScene& scene, Renderer& renderer) {}
};
