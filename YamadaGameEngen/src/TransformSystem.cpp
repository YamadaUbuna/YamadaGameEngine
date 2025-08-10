#include "include\pch.h"
#include "include/TransformSystem.h"
#include "include/TransformComponent.h"


// 前方ベクトル（Z軸）
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

// 上方向ベクトル（Y軸）
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

// 右方向ベクトル（X軸）
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

    return S * R * T; // 順番注意：Scale → Rotate → Translate
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

            // ワールド行列を更新
			transform->UpdateWorldMatrix(GetWorldMatrix(transform));
            // ここでワールド行列を使用して何か処理を行うことができます
            // 例えば、レンダリングや物理演算など
			// 例: transform->SetWorldMatrix(worldMatrix);
        }
    }
}


