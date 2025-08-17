#include "include\pch.h"
#include "include/CubeGeneratorSystem.h"
#include "include/GameScene.h"
#include "include/CubeGeneratorComponent.h"

void CubeGeneratorSystem::Initialize(IScene& scene)
{
	for (auto& entity : scene.GetEntities()) {
		CubeGeneratorComponent* cubeGene = entity->GetComponent<CubeGeneratorComponent>();
		if (!cubeGene) continue;

        cubeGene->SetSpawn(1.0f);
	}
}

// CubeGeneratorSystem.cpp
void CubeGeneratorSystem::Update(IScene& scene, float deltaTime)
{
    std::vector<std::pair<GameScene*, CubeGeneratorComponent*>> spawnList;

    // まず全ての CubeGeneratorComponent をチェック
    for (auto& entity : scene.GetEntities()) {
        if (!entity) continue;

        auto cubeGene = entity->GetComponent<CubeGeneratorComponent>();
        if (!cubeGene) continue;

        cubeGene->UpdateTimer(deltaTime);

        if (cubeGene->ShouldSpawn()) {
            if (auto gameScene = dynamic_cast<GameScene*>(&scene)) {
                spawnList.push_back({ gameScene, cubeGene });
            }
        }
    }
    // チェック完了後にまとめて生成
    for (auto& pair : spawnList) {
        GameScene* gameScene = pair.first;

        // Entity を生成してポインタを取得
        IEntity* cube = gameScene->CreateEntity<exampleEntity>();

        exampleComponent* example = cube->GetComponent<exampleComponent>();
        TransformComponent* transform = cube->GetComponent<TransformComponent>();
        ModelComponent* model = cube->GetComponent<ModelComponent>();

        transform->SetPosition({ -5.0f,-1.0f,0.0f });

        AssetManager& assetManager = scene.GetAssetManager();
        ModelDataContainer* modelData = assetManager.GetModelForUpdate(model->GetModelId());

        for (auto& meshPtr : modelData->GetMeshes())
        {
            MeshComponent& mesh = *meshPtr;
            MaterialComponent* material = assetManager.GetMaterialForUpdate(mesh.GetMaterialId());

            material->SetBaseColor({ 1,0,0,1 });
        }
    }
}

