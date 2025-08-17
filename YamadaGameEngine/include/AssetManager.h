#pragma once
#include "include/ModelData.h"

#include <unordered_map>
#include <fbxsdk.h>
#include <DirectXTex.h>
#include <filesystem>

// AssetManagerクラスは、3Dモデル、テクスチャ、マテリアルを読み込み・管理するクラスです。
// モデル・テクスチャの読み込み、GPUへの転送、リソース取得・解放などを行います。
// DirectX 12およびFBX SDKを利用しており、SRVヒープ管理も含まれています。
// IDを用いたリソースの取得と更新、FBXノードの解析も対応しています。


class ModelComponent;

struct TextureResource
{
    Microsoft::WRL::ComPtr<ID3D12Resource> resource; // GPU用リソース
    D3D12_GPU_DESCRIPTOR_HANDLE srv{};               // SRV ハンドル
    std::string path;                                // 読み込んだファイルパス
};

class AssetManager {
public:
    // モデルを読み込んで登録
    bool LoadModel(const std::string& id, const std::wstring& filepath);

    // テクスチャ読み込み
    std::wstring LoadTexture(const std::filesystem::path& fullPath);
    const TextureResource* GetTexture(const std::wstring& id) const;

    // 登録済みモデルをIDから取得（読み取り専用）
    const ModelDataContainer* GetModel(const std::string& id) const
    {
        auto it = m_models.find(id);
        if (it == m_models.end()) {
            throw std::runtime_error("そんなもでるないですけど");
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
        // モデル、テクスチャ、マテリアルを解放
        m_models.clear();
        m_textures.clear();
        m_materials.clear();

        // SRVヒープはComPtrなので自動解放されるが念のためリセット
        m_srvHeap.Reset();
    }

private:
    // ID -> ModelData のマップ（ModelDataはメッシュなどを持つ実体クラス）
    std::unordered_map<std::string, std::unique_ptr<ModelDataContainer>> m_models;
    std::unordered_map<std::wstring, std::unique_ptr<TextureResource>> m_textures;
    std::unordered_map<std::string, std::unique_ptr<MaterialComponent>> m_materials;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_srvHeap; // SRV用ヒープ
    void UploadMeshToGPU(MeshComponent* meshComponent);
    
};

