#include "include/pch.h"
#include "include/GameScene.h"

//System
#include "include/exampleSystem.h"
#include "include/RenderSystem.h"
#include "include/CameraSystem.h"
#include "include/TransformSystem.h"
#include "include/CubeGeneratorSystem.h"
#include "include/DropCubeSystem.h"

//Entity
#include "include/exampleEntity.h"
#include "include/CameraEntity.h"
#include "include/GeneratorEntity.h"
#include "include/DropCubeEntity.h"



void GameScene::Initialize()
{
    //ここでSceneで使うModelも登録する
    assetManager.LoadModel("cube", L"Resource/Rubik.fbx");

	IEntity* Generator = CreateEntity<GeneratorEntity>();
	IEntity* cameraEntity = CreateEntity<CameraEntity>();
    IEntity* DropCube = CreateEntity< DropCubeEntity>();

    exampleSystem* exampleSys = new exampleSystem();
	TransformSystem* transformSys = new TransformSystem();
	RenderSystem* renderSys = new RenderSystem();
	CameraSystem* cameraSys = new CameraSystem();
    CubeGeneratorSystem* cubeGeneratorSys = new CubeGeneratorSystem();
    DropCubeSystem* dropCubeSys = new DropCubeSystem();

    AddSystem(cubeGeneratorSys);
    AddSystem(dropCubeSys);
    AddSystem(exampleSys);
	AddSystem(transformSys);
    AddSystem(cameraSys);
    AddSystem(renderSys);

	m_mainCamera = cameraEntity; // メインカメラを設定

    for (ISystem* system : m_systems) {
        system->Initialize(*this);
	}
}

void GameScene::Update(float deltaTime)
{
    for (ISystem* system : m_systems) {
        system->Update(*this, deltaTime);
    }

    RemoveDestroyedEntities();
}

void GameScene::Cleanup()
{
        // まずシステムをすべて解放
    for (ISystem* system : m_systems) {
        delete system;
    }
    m_systems.clear();

    // エンティティは unique_ptr なので clear で自動解放
    m_entities.clear();

    // メインカメラをクリア
    m_mainCamera = nullptr;

    // 必要に応じて AssetManager のリソースも解放
    assetManager.Cleanup();
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

void GameScene::RemoveDestroyedEntities()
{
    m_entities.erase(
        std::remove_if(m_entities.begin(), m_entities.end(),
            [](const std::unique_ptr<IEntity>& e) {
                return e->IsDestroyed();
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