#pragma once
#include "include/ISystem.h"
#include "include/MeshComponent.h"
#include "include/MaterialComponent.h"
#include "include/TransformComponent.h"
#include "include/ModelComponent.h"

#include <DirectXMath.h>

class RenderSystem :
    public ISystem
{
public:
    void Initialize(IScene& scene) override;
    void Update(IScene& scene, float deltaTime) override;

private:
};

