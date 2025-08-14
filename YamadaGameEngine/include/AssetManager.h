#pragma once
#include <unordered_map>
#include "include/ModelData.h"
#include <fbxsdk.h>
#include <DirectXTex.h>

//�EModelManager�i���邢��AssetManager�j
//���f���iModelComponent�╡�����b�V���̃Z�b�g�j��ǂݍ��݁E�Ǘ��E�L���b�V������N���X
//�t�@�C���ǂݍ��݂͂������ꊇ�ōs���AID�▼�O�Ń��f����ێ�
//���łɓǂݍ��܂�Ă���ꍇ�͍ēǂݍ��݂����A�����̃��f����Ԃ�

class ModelComponent; // �O���錾

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
    std::string LoadTexture(const std::string& filepath);
    const TextureResource* GetTexture(const std::string& id) const;

    // �o�^�ς݃��f����ID����擾�i�ǂݎ���p�j
    const ModelDataContainer* GetModel(const std::string& id) const;

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

private:
    // ID -> ModelData �̃}�b�v�iModelData�̓��b�V���Ȃǂ������̃N���X�j
    std::unordered_map<std::string, std::unique_ptr<ModelDataContainer>> m_models;
    std::unordered_map<std::string, std::unique_ptr<TextureResource>> m_textures;
    std::unordered_map<std::string, std::unique_ptr<MaterialComponent>> m_materials;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_srvHeap; // SRV�p�q�[�v
    void UploadMeshToGPU(MeshComponent* meshComponent);
    
};

