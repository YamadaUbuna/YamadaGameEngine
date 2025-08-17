#pragma once

#include "include/d3dx12.h"
#include "include/PipelineManager.h"
#include "include/IComponent.h"

// MaterialComponent�̓}�e���A�����i�F�E�e�N�X�`���E�p�C�v���C���ݒ�j��ێ�����R���|�[�l���g�ł��B
// �x�[�X�J���[��A���x�h�e�N�X�`��ID�A�萔�o�b�t�@�ւ̎Q�Ƃ��Ǘ����܂��B
// �g�p����p�C�v���C���⃋�[�g�V�O�l�`���̎�ނ��ݒ�\�ł��B
// ����̊g�����l�����ă��^���b�N�E���t�l�X�l����`����Ă��܂��B


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

    // �F�p�����[�^
    DirectX::XMFLOAT4 GetBaseColor() const { return baseColor; }
    void SetBaseColor(const DirectX::XMFLOAT4& color) { baseColor = color; }

    // �e�N�X�`��ID
    const std::wstring& GetAlbedoTextureId() const { return albedoTextureId; }
    void SetAlbedoTextureId(const std::wstring& id) { albedoTextureId = id; }

    Microsoft::WRL::ComPtr<ID3D12Resource>& SetColorCB() { return ColorCB; }
    const Microsoft::WRL::ComPtr<ID3D12Resource>& GetColorCB() const { return ColorCB; }

private:

    PipelineType pipelineType;
    RootSignatureType rootSignatureType;

    DirectX::XMFLOAT4 baseColor = { 1,1,1,1 };

    Microsoft::WRL::ComPtr<ID3D12Resource> ColorCB;

    std::wstring albedoTextureId; // AssetManager �ŊǗ�����e�N�X�`��ID

    // ����g���\
    float metallic = 0.0f;
    float roughness = 1.0f;
};
