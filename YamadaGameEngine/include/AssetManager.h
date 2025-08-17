#pragma once
#include "include/ModelData.h"

#include <unordered_map>
#include <fbxsdk.h>
#include <DirectXTex.h>
#include <filesystem>

// AssetManager�N���X�́A3D���f���A�e�N�X�`���A�}�e���A����ǂݍ��݁E�Ǘ�����N���X�ł��B
// ���f���E�e�N�X�`���̓ǂݍ��݁AGPU�ւ̓]���A���\�[�X�擾�E����Ȃǂ��s���܂��B
// DirectX 12�����FBX SDK�𗘗p���Ă���ASRV�q�[�v�Ǘ����܂܂�Ă��܂��B
// ID��p�������\�[�X�̎擾�ƍX�V�AFBX�m�[�h�̉�͂��Ή����Ă��܂��B


class ModelComponent;

struct TextureResource
{
    Microsoft::WRL::ComPtr<ID3D12Resource> resource; // GPU�p���\�[�X
    D3D12_GPU_DESCRIPTOR_HANDLE srv{};               // SRV �n���h��
    std::string path;                                // �ǂݍ��񂾃t�@�C���p�X
};

class AssetManager {
public:
    // ���f����ǂݍ���œo�^
    bool LoadModel(const std::string& id, const std::wstring& filepath);

    // �e�N�X�`���ǂݍ���
    std::wstring LoadTexture(const std::filesystem::path& fullPath);
    const TextureResource* GetTexture(const std::wstring& id) const;

    // �o�^�ς݃��f����ID����擾�i�ǂݎ���p�j
    const ModelDataContainer* GetModel(const std::string& id) const
    {
        auto it = m_models.find(id);
        if (it == m_models.end()) {
            throw std::runtime_error("����Ȃ��ł�Ȃ��ł�����");
            return nullptr;
        }
        return it->second.get();
    };
    ModelDataContainer* GetModelForUpdate(const std::string& id);

    HRESULT CreateTextureOnGPU(
        const DirectX::Image* images,
        size_t nImages,
        const DirectX::TexMetadata& metadata,
        Microsoft::WRL::ComPtr<ID3D12Resource>& outResource);

    D3D12_GPU_DESCRIPTOR_HANDLE CreateShaderResourceView(Microsoft::WRL::ComPtr<ID3D12Resource>& resource);

    bool LoadModel(const std::wstring& filepath, ModelDataContainer& outModel);
    void ParseNode(FbxNode* node, ModelDataContainer& outModel);
    void ParseMesh(FbxNode* node, FbxMesh* mesh, ModelDataContainer& outModel);
    std::string ParseMaterial(FbxSurfaceMaterial* fbxMaterial);

    const MaterialComponent* GetMaterial(const std::string& id) const {
        auto it = m_materials.find(id);
        return (it != m_materials.end()) ? it->second.get() : nullptr;
    }

    MaterialComponent* GetMaterialForUpdate(const std::string& id)
    {
        auto it = m_materials.find(id);
        return (it != m_materials.end()) ? it->second.get() : nullptr;
    }

    void Cleanup()
    {
        // ���f���A�e�N�X�`���A�}�e���A�������
        m_models.clear();
        m_textures.clear();
        m_materials.clear();

        // SRV�q�[�v��ComPtr�Ȃ̂Ŏ����������邪�O�̂��߃��Z�b�g
        m_srvHeap.Reset();
    }

private:
    // ID -> ModelData �̃}�b�v�iModelData�̓��b�V���Ȃǂ������̃N���X�j
    std::unordered_map<std::string, std::unique_ptr<ModelDataContainer>> m_models;
    std::unordered_map<std::wstring, std::unique_ptr<TextureResource>> m_textures;
    std::unordered_map<std::string, std::unique_ptr<MaterialComponent>> m_materials;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_srvHeap; // SRV�p�q�[�v
    void UploadMeshToGPU(MeshComponent* meshComponent);
    
};

