#include "include/pch.h"
#include "include/exampleSystem.h"
#include "include/exampleComponent.h"
#include <debugapi.h>

void exampleSystem::Initialize(IScene& scene)
{
    for (auto& entity : scene.GetEntities()) {
        if (exampleComponent* example = entity->GetComponent<exampleComponent>()) {
            m_name = example->GetMyName();
        }
    }
}

void exampleSystem::Update(IScene& scene, float deltaTime)
{
    for (auto& entity : scene.GetEntities()) {
        if (exampleComponent* example = entity->GetComponent<exampleComponent>()) {
            std::stringstream ss;
            ss << "���̖��O�� " << m_name << " �ł��B";

            OutputDebugStringA(ss.str().c_str());
        }
    }
}
