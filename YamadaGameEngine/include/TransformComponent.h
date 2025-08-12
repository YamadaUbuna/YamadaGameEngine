#pragma once

#include "include/IComponent.h"
#include <DirectXMath.h>
#include <d3d12.h>
#include <wrl/client.h>

using namespace DirectX;

class TransformComponent :public IComponent {
public:
	TransformComponent() = default;
	~TransformComponent() = default;

	// Getters
	const XMFLOAT3 &GetPosition() const { return position; }
	const XMFLOAT3 &GetRotation() const { return rotation; }
	const XMFLOAT3 &GetScale() const { return scale; }

	const XMFLOAT3 &GetForward() { return forward; }
	const XMFLOAT3 &GetUp() { return up; }
	const XMFLOAT3 &GetRight() { return right; }

	const XMMATRIX &GetWorldMatrix() const{ return worldMatrix; }

	const XMFLOAT4& GetQuaternionRotation() const { return quatRotation; }

	// Setters
	void SetPosition(const XMFLOAT3& pos) { position = pos; }
	void SetRotation(const XMFLOAT3& rot);
	void SetScale(const XMFLOAT3& scl) { scale = scl; }

	void SetQuaternionRotation(const XMFLOAT4& quat);

	void SetForward(const XMFLOAT3& fwd) { forward = fwd; }
	void SetUp(const XMFLOAT3& upVec) { up = upVec; }
	void SetRight(const XMFLOAT3& rgt) { right = rgt; }

	void UpdateWorldMatrix(XMMATRIX worldMat) { worldMatrix = worldMat; }

    ID3D12Resource* GetConstantBuffer() const {
        return constantBuffer.Get();
    }

    void SetConstantBuffer(const Microsoft::WRL::ComPtr<ID3D12Resource>& buffer) {
        constantBuffer = buffer;
    }

	void Translate(const XMFLOAT3& translation);
	void Rotate(const XMFLOAT3& deltaRotation);

private:
	// Position, rotation, scale
	XMFLOAT3 position{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 rotation{ 0.0f, 0.0f, 0.0f };
	XMFLOAT3 scale{ 1.0f, 1.0f, 1.0f };

	XMFLOAT4 quatRotation{ 0.0f, 0.0f, 0.0f, 1.0f }; // �N�H�[�^�j�I����]

	XMFLOAT3 forward{ 0.0f,0.0f,0.0f };
	XMFLOAT3 up{ 0.0f,1.0f,0.0f };
	XMFLOAT3 right{ 1.0f,0.0f,0.0f };

	XMMATRIX worldMatrix{};

    Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer;

	void UpdateAxesAndWorldMatrix();
};