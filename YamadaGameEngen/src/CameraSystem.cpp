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
        // �J�������ݒ肳��Ă��Ȃ��ꍇ�͉������Ȃ�
        OutputDebugStringA("MainCamera������܂���");
        return;
	}

    CameraComponent* cam = camEntity->GetComponent<CameraComponent>();
    if (!cam) { 
        //�܂����肦�Ȃ��Ǝv���܂����ǂ�
        throw std::runtime_error("MainCamera�G���e�B�e�B�� CameraComponent �����݂��܂���B");
    }

    TransformComponent* transform = camEntity->GetComponent<TransformComponent>();
    if (!transform) {
        // Camera�������Ă���̂�Transform�������ُ͈̂�
        throw std::runtime_error("CameraComponent �����G���e�B�e�B�� TransformComponent �����݂��܂���B");
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
