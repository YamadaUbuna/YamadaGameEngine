#pragma once
#include "include/ISystem.h"
#include "include/CameraComponent.h"

class CameraSystem :
    public ISystem
{
public:
    void Initialize(IScene& scene) override;
    void Update(IScene& scene, float deltaTime) override;

private:
    void UpdateMatrices();
};

