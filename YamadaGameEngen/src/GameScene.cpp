#include "include/pch.h"
#include "include/GameScene.h"

#include "include/exampleSystem.h"
#include "include/exampleComponent.h"


void GameScene::Initialize()
{

    IEntity* example = CreateEntity();
    example->AddComponent(std::make_unique<exampleComponent>());

    exampleSystem* exampleSys = new exampleSystem();
    AddSystem(exampleSys);
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
