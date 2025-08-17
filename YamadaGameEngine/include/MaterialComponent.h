#pragma once

#include "include/d3dx12.h"
#include "include/PipelineManager.h"
#include "include/IComponent.h"

// MaterialComponentはマテリアル情報（色・テクスチャ・パイプライン設定）を保持するコンポーネントです。
// ベースカラーやアルベドテクスチャID、定数バッファへの参照を管理します。
// 使用するパイプラインやルートシグネチャの種類も設定可能です。
// 今後の拡張を考慮してメタリック・ラフネス値も定義されています。


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

    Microsoft::WRL::ComPtr<ID3D12Resource>& SetColorCB() { return ColorCB; }
    const Microsoft::WRL::ComPtr<ID3D12Resource>& GetColorCB() const { return ColorCB; }

private:

    PipelineType pipelineType;
    RootSignatureType rootSignatureType;

    DirectX::XMFLOAT4 baseColor = { 1,1,1,1 };

    Microsoft::WRL::ComPtr<ID3D12Resource> ColorCB;

    std::wstring albedoTextureId; // AssetManager で管理するテクスチャID

    // 今後拡張可能
    float metallic = 0.0f;
    float roughness = 1.0f;
};
