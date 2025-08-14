#pragma once
#include <unordered_map>
#include "include/ModelData.h"
#include <fbxsdk.h>
#include <DirectXTex.h>

//・ModelManager（あるいはAssetManager）
//モデル（ModelComponentや複数メッシュのセット）を読み込み・管理・キャッシュするクラス
//ファイル読み込みはここが一括で行い、IDや名前でモデルを保持
//すでに読み込まれている場合は再読み込みせず、既存のモデルを返す

class ModelComponent; // 前方宣言

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
    std::string LoadTexture(const std::string& filepath);
    const TextureResource* GetTexture(const std::string& id) const;

    // 登録済みモデルをIDから取得（読み取り専用）
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
    // ID -> ModelData のマップ（ModelDataはメッシュなどを持つ実体クラス）
    std::unordered_map<std::string, std::unique_ptr<ModelDataContainer>> m_models;
    std::unordered_map<std::string, std::unique_ptr<TextureResource>> m_textures;
    std::unordered_map<std::string, std::unique_ptr<MaterialComponent>> m_materials;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_srvHeap; // SRV用ヒープ
    void UploadMeshToGPU(MeshComponent* meshComponent);
    
};

