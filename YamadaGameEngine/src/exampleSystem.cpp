#include "include/pch.h"
#include "include/exampleSystem.h"
#include "include/exampleComponent.h"
#include "include/InputManager.h"
#include <debugapi.h>

#include "include/TransformComponent.h"`

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

            TransformComponent* transform = entity->GetComponent<TransformComponent>();
            if(InputManager::GetKeyDown(VK_SPACE)|| InputManager::GetKeyUp(VK_SPACE)) {
                std::stringstream ss;
                ss << "私の名前は " << m_name << " です。\n";
                OutputDebugStringA(ss.str().c_str());
			}
            if (InputManager::GetKey(VK_ESCAPE)) {
                transform->Translate({ 0.001f, 0.0f, 0.0f });
				transform->Rotate({ 0.1f, 0.0f, 0.0f });
                OutputDebugStringA("ESCキーが押されました。\n");
			}
        }
    }
}
