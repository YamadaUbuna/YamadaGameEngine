#include "include/pch.h"
#include "include/GameScene.h"

#include "include/exampleSystem.h"
#include "include/exampleComponent.h"
#include "include/InputManager.h"

#include "include/RenderSystem.h"
#include "include/CameraComponent.h"


void GameScene::Initialize()
{
    IEntity* example = CreateEntity();
    example->AddComponent(std::make_unique<exampleComponent>());
	//example->AddComponent(std::make_unique<RenderComponent>()); 

    exampleSystem* exampleSys = new exampleSystem();
	//RenderSystem* renderSys = new RenderSystem();
    AddSystem(exampleSys);
	//AddSystem(renderSys);
    for (ISystem* system : m_systems) {
        system->Initialize(*this);
	}
}


void GameScene::Update(float deltaTime)
{
    for (ISystem* system : m_systems) {
        system->Update(*this, deltaTime);
    }
}

void GameScene::Cleanup()
{
}

void GameScene::RemoveEntity(IEntity* entity)
{
    m_entities.erase(
        std::remove_if(m_entities.begin(), m_entities.end(),
            [entity](const std::unique_ptr<IEntity>& e) {
                return e.get() == entity;
            }),
        m_entities.end());
}

void GameScene::RemoveSystem(ISystem* system)
{
    m_systems.erase(
        std::remove(m_systems.begin(), m_systems.end(), system),
        m_systems.end());
}

void GameScene::SetMainCamera(IEntity* camera)
{
    CameraComponent* cam = camera->GetComponent<CameraComponent>();
    if (!cam) {
        throw std::runtime_error("Main camera must have a CameraComponent.");
	}
    m_mainCamera = camera;
}
