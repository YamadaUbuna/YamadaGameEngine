#include "include\pch.h"
#include "include/DropCubeSystem.h"
#include "include/DropCubeComponent.h"
#include "include/InputManager.h"
#include "include/exampleComponent.h"
#include "include/GameScene.h"
#include "include/DropCubeEntity.h"

void DropCubeSystem::Initialize(IScene& scene)
{
    //for (auto& entity : scene.GetEntities()) {
    //    DropCubeComponent* DropCube = entity->GetComponent<DropCubeComponent>();
    //    TransformComponent* transform = entity->GetComponent<TransformComponent>();

    //    if (!transform || !DropCube) continue;
    //}
}

void DropCubeSystem::Update(IScene& scene, float deltaTime)
{

    std::vector<IEntity*> spawnList; // ��Ő������� Entity ���ꎞ�ێ�

    for (auto& entity : scene.GetEntities()) {
        DropCubeComponent* DropCube = entity->GetComponent<DropCubeComponent>();
        TransformComponent* transform = entity->GetComponent<TransformComponent>();

        if (!transform || !DropCube) continue;

        if (InputManager::GetKeyDown(VK_SPACE))
        {
            DropCube->SetDroping(true);
            continue; // ���̃t���[���ł͏������X�L�b�v���āA���t���[�����痎���J�n
        }

        if (DropCube->GetDroping())
        {
            if (transform->GetPosition().y <= -1.0f)
            {
                transform->SetPosition({ transform->GetPosition().x,-1.0f,transform->GetPosition().z });
                DropCube->SetDroping(false);
                entity->AddComponent(std::make_unique<exampleComponent>());
                if (AreAllCubesInRange(scene)) {
                    //�����ɃQ�[���I�[�o�[���̏���
                    entity->Destroy();
                }
                else {
                    spawnList.push_back(entity.get());
                }
            }
            else {
                transform->Translate({ 0.0f, DropCube->GetDropSpeed() * deltaTime, 0.0f });
            }
        }
    }
    for (auto* original : spawnList) {
        if (auto gameScene = dynamic_cast<GameScene*>(&scene)) {
            // ��: exampleEntity �Ɖ���
            IEntity* newCube = gameScene->CreateEntity<DropCubeEntity>();
        }
    }

}

bool DropCubeSystem::AreAllCubesInRange(IScene& scene)
{
    for (auto& entity : scene.GetEntities()) {
        if (!entity) continue;

        DropCubeComponent* DropCube = entity->GetComponent<DropCubeComponent>();
        if (DropCube) continue;

        exampleComponent* example = entity->GetComponent<exampleComponent>();
        TransformComponent* transform = entity->GetComponent<TransformComponent>();
        if (!example || !transform) continue;
        

        float x = transform->GetPosition().x;
        if (x >= -1.0f && x <= 1.0f) {
            return true;
        }
    }
    return false;
}
