#include "include\pch.h"
#include "include/AssetManager.h"
#include <stdexcept>
#include <include/Renderer.h>

bool AssetManager::LoadModel(const std::string& id, const std::wstring& filepath)
{
    // モデルがすでに存在している場合はロードしない
    if (m_models.find(id) != m_models.end())
        return false; // すでに存在する

    // ModelDataを生成
    auto modelData = std::make_unique<ModelData>();

    // FBX読み込み
    FbxLoader loader;
    if (!loader.LoadModel(filepath, *modelData))
        return false;

    //モデルのメッシュをgpuに送る
    for (const auto& mesh : modelData->GetMeshes()) {
        UploadMeshToGPU(mesh.get()); // unique_ptr から参照を取得して渡す
    }

    // モデルデータを登録
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


void AssetManager::UploadMeshToGPU(MeshComponent* meshComponent) {//indexとvertexの生データをGPUに送る形式に変える

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

bool FbxLoader::LoadModel(const std::wstring& filepath, ModelData& outModel)
{
    FbxManager* sdkManager = FbxManager::Create();
    FbxIOSettings* ios = FbxIOSettings::Create(sdkManager, IOSROOT);
    sdkManager->SetIOSettings(ios);

    FbxImporter* importer = FbxImporter::Create(sdkManager, "");

    // wstring → UTF-8 string 変換
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, filepath.c_str(), (int)filepath.size(), NULL, 0, NULL, NULL);
    std::string filepathUtf8(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, filepath.c_str(), (int)filepath.size(), &filepathUtf8[0], size_needed, NULL, NULL);

    if (!importer->Initialize(filepathUtf8.c_str(), -1, sdkManager->GetIOSettings()))
    {
        // エラーハンドリング
        importer->Destroy();
        sdkManager->Destroy();
        return false;
    }

    FbxScene* scene = FbxScene::Create(sdkManager, "scene");
    importer->Import(scene);

    FbxGeometryConverter converter(sdkManager);
    converter.SplitMeshesPerMaterial(scene, true); //マテリアルごとにメッシュを分解
    converter.Triangulate(scene, true); //四角とか混ざってるかもしれないFBXを全部三角形だけに変換。
    importer->Destroy();

    FbxNode* rootNode = scene->GetRootNode();
    if (rootNode)
    {
        for (int i = 0; i < rootNode->GetChildCount(); i++)
        {
            ParseNode(rootNode->GetChild(i), outModel);
        }
    }

    // ここで将来的にParseSkeleton(scene), ParseAnimation(scene)とか呼ぶ

    sdkManager->Destroy();
    return true;
}

void FbxLoader::ParseNode(FbxNode* node, ModelData& outModel)
{
    if (!node) return;

    FbxMesh* mesh = node->GetMesh();
    if (mesh)
    {
        ParseMesh(mesh, outModel);
    }

    // 子ノードも再帰的に処理
    for (int i = 0; i < node->GetChildCount(); i++)
    {
        ParseNode(node->GetChild(i), outModel);
    }
}

void FbxLoader::ParseMesh(FbxMesh* mesh, ModelData& outModel)
{
    // 頂点、法線、UV、インデックスの抽出をここで行う
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    int polygonCount = mesh->GetPolygonCount();

    for (int i = 0; i < polygonCount; i++)
    {
        int polygonSize = mesh->GetPolygonSize(i);
        assert(polygonSize == 3); // 三角形のみ対応（必要に応じて三角形化）

        for (int j = 0; j < polygonSize; j++)
        {
            int ctrlPointIndex = mesh->GetPolygonVertex(i, j);

            FbxVector4 position = mesh->GetControlPointAt(ctrlPointIndex);
            FbxVector4 normal;
            mesh->GetPolygonVertexNormal(i, j, normal);

            // UVはレイヤーから取得
            FbxStringList uvSetNameList;
            mesh->GetUVSetNames(uvSetNameList);
            FbxVector2 uv(0, 0);
            if (uvSetNameList.GetCount() > 0)
            {
                bool unmapped;
                mesh->GetPolygonVertexUV(i, j, uvSetNameList[0], uv, unmapped);
            }

            Vertex v;
            v.position = { (float)position[0], (float)position[1], (float)position[2] };
            v.normal = { (float)normal[0], (float)normal[1], (float)normal[2] };
            v.uv = { (float)uv[0], (float)uv[1] };
            vertices.push_back(v);
            indices.push_back(static_cast<uint32_t>(vertices.size() - 1));
        }
    }

    auto meshComponent = std::make_unique<MeshComponent>();
    meshComponent->SetVertices(vertices);
    meshComponent->SetIndices(indices);

    // GPUバッファ初期化は呼び出し側でやるか、ここでやってもよい

    outModel.AddMesh(std::move(meshComponent));
}

//std::string FbxLoader::GetTexturePath(FbxSurfaceMaterial* material, const char* propName)
//{
//    FbxProperty prop = material->FindProperty(propName);
//    if (prop.IsValid()) {
//        int textureCount = prop.GetSrcObjectCount<FbxFileTexture>();
//        if (textureCount > 0) {
//            FbxFileTexture* texture = prop.GetSrcObject<FbxFileTexture>(0);
//            if (texture) {
//                return texture->GetFileName(); // フルパス取得
//            }
//        }
//    }
//    return "";
//}
