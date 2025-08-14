#include "include/pch.h"
#include "include/GameScene.h"

//System
#include "include/exampleSystem.h"
#include "include/RenderSystem.h"
#include "include/CameraSystem.h"
#include "include/TransformSystem.h"

#include "include/exampleEntity.h"
#include "include/CameraEntity.h"
#include "include/Cube.h"



void GameScene::Initialize()
{
    //‚±‚±‚ÅScene‚ÅŽg‚¤Model‚à“o˜^‚·‚é

    assetManager.LoadModel("hero", L"Resource/cubedayo.fbx");

    IEntity* test = CreateEntity<exampleEntity>();
	IEntity* cube = CreateEntity<cubeEntity>();
	IEntity* cameraEntity = CreateEntity<CameraEntity>();


    exampleSystem* exampleSys = new exampleSystem();
	TransformSystem* transformSys = new TransformSystem();
	RenderSystem* renderSys = new RenderSystem();
	CameraSystem* cameraSys = new CameraSystem();
    AddSystem(exampleSys);
	AddSystem(transformSys);
    AddSystem(cameraSys);
    AddSystem(renderSys);

	m_mainCamera = cameraEntity; // ƒƒCƒ“ƒJƒƒ‰‚ðÝ’è

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

void GameScene::SetMainCameraEntity(IEntity* camera)
{
    if (!camera) {
        throw std::runtime_error("Main camera cannot be null.");
    }
    CameraComponent* cam = camera->GetComponent<CameraComponent>();
    if (!cam) {
        throw std::runtime_error("Main camera must have a CameraComponent.");
    }
	m_mainCamera = camera;
}