#pragma once
#include "include/ISystem.h"

class MaterialSystem :
    public ISystem
{
public:
    void Initialize(IScene& scene) override;
	void Update(IScene& scene, float deltaTime) override;
};

