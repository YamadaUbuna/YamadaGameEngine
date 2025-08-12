#include "include\pch.h"
#include "include/RenderSystem.h"
#include "include/Renderer.h"
#include <stdexcept>

//ここに関数作って、MeshComponentの情報を綺麗にする。


void RenderSystem::Initialize(IScene& scene)  
{  
    // 初期化処理が必要な場合はここに記述  
    const std::vector<std::unique_ptr<IEntity>>& entities = scene.GetEntities();  
    for (const std::unique_ptr<IEntity>& entity : entities) {  
        const ModelComponent* modelComponent = entity->GetComponent<ModelComponent>();
        const MaterialComponent* materialComponent = entity->GetComponent<MaterialComponent>();
        const TransformComponent* transformComponent = entity->GetComponent<TransformComponent>();

        if (!modelComponent || !materialComponent || !transformComponent) {  
            // 必要なコンポーネントがない場合はスキップ  
            continue;  
        }  



    }  
}

void RenderSystem::Update(IScene& scene, float deltaTime)
{
    const std::vector<std::unique_ptr<IEntity>>& entities = scene.GetEntities();

    for (const std::unique_ptr<IEntity>& entity : entities) {
        ModelComponent* model = entity->GetComponent<ModelComponent>();
        MaterialComponent* material = entity->GetComponent<MaterialComponent>();
        TransformComponent* transform = entity->GetComponent<TransformComponent>();
		CameraComponent* camera = scene.GetMainCameraEntity()->GetComponent<CameraComponent>();

        if (!model || !material || !transform) {
            // 描画に必要な情報が揃っていないのでスキップ
            continue;
        }

        Renderer::GetInstance().DrawMesh(*model, *material, *transform, *camera,scene.GetAssetManager());

        //RenderComponent* renderComponent = entity->GetComponent<RenderComponent>();
        //if (renderComponent) {
        //    // 仮の描画（実際はDirectX等で描画）
        //    
        //    char buffer[128];
        //    sprintf_s(buffer, "Draw Entity with color RGBA(%f, %f, %f, %f)\n",
        //        renderComponent->r, renderComponent->g, renderComponent->b, renderComponent->a);
        //    OutputDebugStringA(buffer);
        //}
    }
}



