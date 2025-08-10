#pragma once
#include "include/ISystem.h"
#include "include/MeshComponent.h"
#include "include/MaterialComponent.h"
#include "include/TransformComponent.h"

#include <DirectXMath.h>

class RenderSystem :
    public ISystem
{
public:
    void Initialize(IScene& scene) override;
    void Update(IScene& scene, float deltaTime) override;



private:
    void InitializeMeshBuffers(MeshComponent* meshComponent);
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(const void* initData, UINT64 byteSize);
    void InitializeTransformBuffers(TransformComponent* transformComponent);
    DirectX::XMMATRIX GetWorldMatrix(TransformComponent* transformComponent) const;
};

