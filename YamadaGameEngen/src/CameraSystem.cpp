#include "include\pch.h"
#include "include/CameraSystem.h"
#include "include/CameraComponent.h"
#include "include/TransformComponent.h"
#include <stdexcept>

void CameraSystem::Initialize(IScene& scene)
{

}

void CameraSystem::Update(IScene& scene, float deltaTime)
{
    IEntity* camEntity = scene.GetMainCameraEntity();
    if(!camEntity) {
        // カメラが設定されていない場合は何もしない
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

    DirectX::XMFLOAT3 forward = transform->GetForward();
    DirectX::XMFLOAT3 target = {
        transform->GetPosition().x + forward.x,
        transform->GetPosition().y + forward.y,
        transform->GetPosition().z + forward.z
    };
    cam->SetTarget(target);
    cam->SetUp(transform->GetUp());
    
}

void CameraSystem::UpdateMatrices()
{
    //viewMatrix = XMMatrixLookAtLH(XMLoadFloat3(&position), XMLoadFloat3(&target), XMLoadFloat3(&up));
    //projMatrix = XMMatrixPerspectiveFovLH(fovY, GetAspectRatio(), nearZ, farZ);

}
