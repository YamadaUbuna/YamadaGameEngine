#include "include/pch.h"
#include "include/exampleSystem.h"
#include "include/exampleComponent.h"
#include "include/InputManager.h"
#include "include/TransformComponent.h"

#include <debugapi.h>

void InitializeProcess(TransformComponent* transform, IScene& scene, ModelComponent* model) {
    transform->SetPosition({ -10.0f,-1.0f,0.0f });

    AssetManager& assetManager = scene.GetAssetManager();
    ModelDataContainer* modelData = assetManager.GetModelForUpdate(model->GetModelId());

    for (auto& meshPtr : modelData->GetMeshes())
    {
        MeshComponent& mesh = *meshPtr;
        MaterialComponent* material = assetManager.GetMaterialForUpdate(mesh.GetMaterialId());

        material->SetBaseColor({ 1,0,0,1 });
    }
}

void exampleSystem::Initialize(IScene& scene)
{
    for (auto& entity : scene.GetEntities()) {
        exampleComponent* example = entity->GetComponent<exampleComponent>();
        TransformComponent* transform = entity->GetComponent<TransformComponent>();
        ModelComponent* model = entity->GetComponent<ModelComponent>();

        if (!transform || !example ||!model) continue;

        InitializeProcess(transform, scene, model);
    }
}

void exampleSystem::Initialize(std::unique_ptr<IEntity>& entity, IScene& scene)
{
    exampleComponent* example = entity->GetComponent<exampleComponent>();
    TransformComponent* transform = entity->GetComponent<TransformComponent>();
    ModelComponent* model = entity->GetComponent<ModelComponent>();

    if (!transform || !example || !model) return;

    InitializeProcess(transform, scene, model);
}

void exampleSystem::Update(IScene& scene, float deltaTime)
{
    for (auto& entity : scene.GetEntities()) {
        exampleComponent* example = entity->GetComponent<exampleComponent>();
        TransformComponent* transform = entity->GetComponent<TransformComponent>();

        if (!transform||!example) continue;

		transform->Translate({ 2.0f * deltaTime, 0.0f, 0.0f }); // ‚¸‚Á‚Æ‰E‚Éi‚Þ

        if (transform->GetPosition().x > 3)
        {
            entity->Destroy();
        }
    }
}
