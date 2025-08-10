#pragma once

#include "include/d3dx12.h"
#include "include/PipelineManager.h"
#include "include/IComponent.h"


class MaterialComponent : public IComponent
{
public:
	MaterialComponent() = default;
    ~MaterialComponent();

	PipelineType GetPipelineType() const { return pipelineType; }
	void SetPipelineType(PipelineType type) { pipelineType = type; }

	RootSignatureType GetRootSignatureType() const { return rootSignatureType; }
	void SetRootSignatureType(RootSignatureType type) { rootSignatureType = type; }

private:

    PipelineType pipelineType;
	RootSignatureType rootSignatureType;

    // 色パラメータ
    DirectX::XMFLOAT4 baseColor = { 1.0f, 1.0f, 1.0f, 1.0f };

    // テクスチャ
    Microsoft::WRL::ComPtr<ID3D12Resource> albedoTexture;
    D3D12_GPU_DESCRIPTOR_HANDLE albedoSrvHandle{};

    // その他（今後拡張可能）
    float metallic = 0.0f;
    float roughness = 1.0f;
};
