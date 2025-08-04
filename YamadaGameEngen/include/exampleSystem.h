#pragma once
#include "include/ISystem.h"
#include <string>
#include <sstream>


class exampleSystem :
    public ISystem
{
public:
    void Initialize(IScene& scene) override;
    void Update(IScene& scene, float deltaTime) override;
private:
    std::string m_name;
};

