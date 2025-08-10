#include "include\pch.h"
#include "include/TransformSystem.h"
#include "include/TransformComponent.h"


// �O���x�N�g���iZ���j
DirectX::XMFLOAT3 Forward(DirectX::XMFLOAT3 rotation)
{
    using namespace DirectX;

    XMVECTOR rot = XMLoadFloat3(&rotation);
    XMMATRIX rotMat = XMMatrixRotationRollPitchYawFromVector(rot);

    XMVECTOR forward = XMVector3TransformNormal(XMVectorSet(0, 0, 1, 0), rotMat);

    XMFLOAT3 result;
    XMStoreFloat3(&result, forward);
    return result;
}

// ������x�N�g���iY���j
DirectX::XMFLOAT3 Up(DirectX::XMFLOAT3 rotation)
{
    using namespace DirectX;

    XMVECTOR rot = XMLoadFloat3(&rotation);
    XMMATRIX rotMat = XMMatrixRotationRollPitchYawFromVector(rot);

    XMVECTOR up = XMVector3TransformNormal(XMVectorSet(0, 1, 0, 0), rotMat);

    XMFLOAT3 result;
    XMStoreFloat3(&result, up);
    return result;
}

// �E�����x�N�g���iX���j
DirectX::XMFLOAT3 GetRight(DirectX::XMFLOAT3 rotation)
{
    using namespace DirectX;

    XMVECTOR rot = XMLoadFloat3(&rotation);
    XMMATRIX rotMat = XMMatrixRotationRollPitchYawFromVector(rot);

    XMVECTOR right = XMVector3TransformNormal(XMVectorSet(1, 0, 0, 0), rotMat);

    XMFLOAT3 result;
    XMStoreFloat3(&result, right);
    return result;
}

DirectX::XMMATRIX GetWorldMatrix(TransformComponent* transform)
{
    using namespace DirectX;

    XMMATRIX S = XMMatrixScaling(transform->GetScale().x, transform->GetScale().y, transform->GetScale().z);
    XMMATRIX R = XMMatrixRotationRollPitchYaw(transform->GetRotation().x, transform->GetRotation().y, transform->GetRotation().z);
    XMMATRIX T = XMMatrixTranslation(transform->GetPosition().x, transform->GetPosition().y, transform->GetPosition().z);

    return S * R * T; // ���Ԓ��ӁFScale �� Rotate �� Translate
}



void TransformSystem::Initialize(IScene& scene)
{

}

void TransformSystem::Update(IScene& scene, float deltaTime)
{
    for (auto& entity : scene.GetEntities()) {
        if (TransformComponent* transform = entity->GetComponent<TransformComponent>()) {

			transform->SetForward(Forward(transform->GetRotation()));
			transform->SetUp(Up(transform->GetRotation()));
            transform->SetRight(GetRight(transform->GetRotation()));

            // ���[���h�s����X�V
			transform->UpdateWorldMatrix(GetWorldMatrix(transform));
            // �����Ń��[���h�s����g�p���ĉ����������s�����Ƃ��ł��܂�
            // �Ⴆ�΁A�����_�����O�╨�����Z�Ȃ�
			// ��: transform->SetWorldMatrix(worldMatrix);
        }
    }
}


