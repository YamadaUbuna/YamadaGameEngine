#include "include\pch.h"
#include "include/AssetManager.h"
#include <stdexcept>
#include <include/Renderer.h>

bool AssetManager::LoadModel(const std::string& id, const std::wstring& filepath)
{
    // モデルがすでに存在している場合はロードしない
    if (m_models.find(id) != m_models.end())
        return false; // すでに存在する

    //auto model = std::make_unique<ModelComponent>();
    //if (!model->LoadFromFile(filepath))
    //    return false;

    //m_models[id] = std::move(model);


    // デバッグ出力
    wchar_t buf[256];
    swprintf_s(buf, sizeof(buf) / sizeof(wchar_t), L"Test LoadModel: %ls\n", filepath.c_str());
    OutputDebugStringW(buf);

    // --- メッシュを生成 ---
    auto mesh = std::make_unique<MeshComponent>();
    auto modelData = std::make_unique<ModelData>();

    // 頂点データ
    std::vector<Vertex> vertices = {
        { { 0.0f,  1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },  // 赤
        { { -1.0f, -1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } }, // 緑
        { { 1.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }   // 青
    };
    mesh->SetVertices(vertices);

    // インデックスデータ
    std::vector<uint32_t> indices = { 0, 2, 1 };
    mesh->SetIndices(indices);

    // GPU バッファ初期化
    InitializeMeshBuffers(mesh.get());

    // モデルにメッシュ追加
    modelData->AddMesh(std::move(mesh));

    // マップに保存
    m_models[id] = std::move(modelData);

    return true;
}

const ModelData* AssetManager::GetModel(const std::string& id) const
{
    auto it = m_models.find(id);
    if (it == m_models.end()) {
        throw std::runtime_error("そんなやつないですけど");
        return nullptr;
    }
    return it->second.get();

}


void AssetManager::InitializeMeshBuffers(MeshComponent* meshComponent) {//indexとvertexの生データをGPUに送る形式に変える

    const UINT vbSize = static_cast<UINT>(meshComponent->GetVertices().size() * sizeof(Vertex));
    const UINT ibSize = static_cast<UINT>(meshComponent->GetIndices().size() * sizeof(uint32_t));

    meshComponent->SetVertexBuffer(Renderer::GetInstance().CreateDefaultBuffer(meshComponent->GetVertices().data(), vbSize, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
    D3D12_VERTEX_BUFFER_VIEW vbView = {};
    vbView.BufferLocation = meshComponent->GetVertexBuffer()->GetGPUVirtualAddress();
    vbView.StrideInBytes = sizeof(Vertex);
    vbView.SizeInBytes = vbSize;
    meshComponent->SetVertexBufferView(vbView);

    meshComponent->SetIndexBuffer(Renderer::GetInstance().CreateDefaultBuffer(meshComponent->GetIndices().data(), ibSize, D3D12_RESOURCE_STATE_INDEX_BUFFER));
    D3D12_INDEX_BUFFER_VIEW ibView = {};
    ibView.BufferLocation = meshComponent->GetIndexBuffer()->GetGPUVirtualAddress();
    ibView.SizeInBytes = ibSize;
    ibView.Format = DXGI_FORMAT_R32_UINT;
    meshComponent->SetIndexBufferView(ibView);
}
