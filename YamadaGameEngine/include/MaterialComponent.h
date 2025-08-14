#pragma once

#include "include/d3dx12.h"
#include "include/PipelineManager.h"
#include "include/IComponent.h"


class MaterialComponent : public IComponent
{
public:
    MaterialComponent() = default;
    ~MaterialComponent() = default;

    // Pipeline / RootSignature
    PipelineType GetPipelineType() const { return pipelineType; }
    void SetPipelineType(PipelineType type) { pipelineType = type; }

    RootSignatureType GetRootSignatureType() const { return rootSignatureType; }
    void SetRootSignatureType(RootSignatureType type) { rootSignatureType = type; }

    // 色パラメータ
    DirectX::XMFLOAT4 GetBaseColor() const { return baseColor; }
    void SetBaseColor(const DirectX::XMFLOAT4& color) { baseColor = color; }

    // テクスチャID
    const std::wstring& GetAlbedoTextureId() const { return albedoTextureId; }
    void SetAlbedoTextureId(const std::wstring& id) { albedoTextureId = id; }

private:

    PipelineType pipelineType;
    RootSignatureType rootSignatureType;

    DirectX::XMFLOAT4 baseColor = { 1,1,1,1 };

    std::wstring albedoTextureId; // AssetManager で管理するテクスチャID

    // 今後拡張可能
    float metallic = 0.0f;
    float roughness = 1.0f;
};
