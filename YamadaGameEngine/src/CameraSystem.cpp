#include "include\pch.h"
#include "include/CameraSystem.h"
#include "include/CameraComponent.h"
#include "include/TransformComponent.h"
#include "include/Renderer.h"
#include <stdexcept>


void CameraSystem::Initialize(IScene& scene)
{
    for (auto& entity : scene.GetEntities()) {
        if (CameraComponent* cam = entity->GetComponent<CameraComponent>()) {
            float aspectRatio = static_cast<float>(Renderer::GetInstance().GetWindowWidth()) / static_cast<float>(Renderer::GetInstance().GetWindowHeight());
            cam->SetFovY(DirectX::XMConvertToRadians(45.0f));
            cam->SetAspectRatio(aspectRatio);
            cam->SetNearZ(0.1f);
            cam->SetFarZ(1000.0f);
			cam->SetTarget({ 0.0f, 0.0f, 0.0f });
        }
    }
}

void CameraSystem::Update(IScene& scene, float deltaTime)
{
    IEntity* camEntity = scene.GetMainCameraEntity();
    if(!camEntity) {
        // カメラが設定されていない場合何もしない
        OutputDebugStringA("MainCameraがありません");
        return;
	}

    CameraComponent* cam = camEntity->GetComponent<CameraComponent>();
    if (!cam) { 
        //まぁありえないと思いますけども
        throw std::runtime_error("MainCameraエンティティに CameraComponent が存在しません。");
    }

    TransformComponent* transform = camEntity->GetComponent<TransformComponent>();
    if (!transform) {
        // Cameraを持っているのにTransformが無いのは異常
        throw std::runtime_error("CameraComponent を持つエンティティに TransformComponent が存在しません。");
    }

    cam->SetPosition(transform->GetPosition());

    //DirectX::XMFLOAT3 forward = transform->GetForward();
    //DirectX::XMFLOAT3 target = {
    //    transform->GetPosition().x + forward.x,
    //    transform->GetPosition().y + forward.y,
    //    transform->GetPosition().z + forward.z
    //};
    //cam->SetTarget(target);
    cam->SetUp(transform->GetUp());

    UpdateMatrices(cam);
}

void CameraSystem::UpdateMatrices(CameraComponent* cam)
{
    cam->SetViewMatrix(DirectX::XMMatrixLookAtLH(XMLoadFloat3(&cam->GetPosition()), XMLoadFloat3(&cam->GetTarget()), XMLoadFloat3(&cam->GetUp())));
	cam->SetProjMatrix(DirectX::XMMatrixPerspectiveFovLH(cam->GetFovY(), cam->GetAspectRatio(), cam->GetNearZ(), cam->GetFarZ()));
}
