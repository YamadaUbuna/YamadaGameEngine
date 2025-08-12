#include "include\pch.h"
#include "include/RenderSystem.h"
#include "include/Renderer.h"
#include <stdexcept>

//�����Ɋ֐�����āAMeshComponent�̏����Y��ɂ���B


void RenderSystem::Initialize(IScene& scene)  
{  
    // �������������K�v�ȏꍇ�͂����ɋL�q  
    const std::vector<std::unique_ptr<IEntity>>& entities = scene.GetEntities();  
    for (const std::unique_ptr<IEntity>& entity : entities) {  
        const ModelComponent* modelComponent = entity->GetComponent<ModelComponent>();
        const MaterialComponent* materialComponent = entity->GetComponent<MaterialComponent>();
        const TransformComponent* transformComponent = entity->GetComponent<TransformComponent>();

        if (!modelComponent || !materialComponent || !transformComponent) {  
            // �K�v�ȃR���|�[�l���g���Ȃ��ꍇ�̓X�L�b�v  
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
            // �`��ɕK�v�ȏ�񂪑����Ă��Ȃ��̂ŃX�L�b�v
            continue;
        }

        Renderer::GetInstance().DrawMesh(*model, *material, *transform, *camera,scene.GetAssetManager());

        //RenderComponent* renderComponent = entity->GetComponent<RenderComponent>();
        //if (renderComponent) {
        //    // ���̕`��i���ۂ�DirectX���ŕ`��j
        //    
        //    char buffer[128];
        //    sprintf_s(buffer, "Draw Entity with color RGBA(%f, %f, %f, %f)\n",
        //        renderComponent->r, renderComponent->g, renderComponent->b, renderComponent->a);
        //    OutputDebugStringA(buffer);
        //}
    }
}



