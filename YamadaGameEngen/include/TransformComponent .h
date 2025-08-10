#pragma once

#include "include/IComponent.h"
#include <DirectXMath.h>

class TransformComponent :public IComponent {
public:
	TransformComponent() = default;
	~TransformComponent() = default;

	// Getters
	DirectX::XMFLOAT3 GetPosition() const { return position; }
	DirectX::XMFLOAT3 GetRotation() const { return rotation; }
	DirectX::XMFLOAT3 GetScale() const { return scale; }

	// Setters
	void SetPosition(const DirectX::XMFLOAT3& pos) { position = pos; }
	void SetRotation(const DirectX::XMFLOAT3& rot) { rotation = rot; }
	void SetScale(const DirectX::XMFLOAT3& scl) { scale = scl; }

    // �O���x�N�g���iZ���j
    DirectX::XMFLOAT3 GetForward() const
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
    DirectX::XMFLOAT3 GetUp() const
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
    DirectX::XMFLOAT3 GetRight() const
    {
        using namespace DirectX;

        XMVECTOR rot = XMLoadFloat3(&rotation);
        XMMATRIX rotMat = XMMatrixRotationRollPitchYawFromVector(rot);

        XMVECTOR right = XMVector3TransformNormal(XMVectorSet(1, 0, 0, 0), rotMat);

        XMFLOAT3 result;
        XMStoreFloat3(&result, right);
        return result;
    }

private:
	// Position, rotation, scale
	DirectX::XMFLOAT3 position{ 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 rotation{ 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 scale{ 1.0f, 1.0f, 1.0f };
};