#include "include\pch.h"
#include "include/AssetManager.h"
#include <stdexcept>
#include <include/Renderer.h>

bool AssetManager::LoadModel(const std::string& id, const std::wstring& filepath)
{
    // ���f�������łɑ��݂��Ă���ꍇ�̓��[�h���Ȃ�
    if (m_models.find(id) != m_models.end())
        return false; // ���łɑ��݂���

    // ModelData�𐶐�
    auto modelData = std::make_unique<ModelData>();

    // FBX�ǂݍ���
    FbxLoader loader;
    if (!loader.LoadModel(filepath, *modelData))
        return false;

    //���f���̃��b�V����gpu�ɑ���
    for (const auto& mesh : modelData->GetMeshes()) {
        UploadMeshToGPU(mesh.get()); // unique_ptr ����Q�Ƃ��擾���ēn��
    }

    // ���f���f�[�^��o�^
    m_models[id] = std::move(modelData);

    return true;
}

const ModelData* AssetManager::GetModel(const std::string& id) const
{
    auto it = m_models.find(id);
    if (it == m_models.end()) {
        throw std::runtime_error("����Ȃ�Ȃ��ł�����");
        return nullptr;
    }
    return it->second.get();

}


void AssetManager::UploadMeshToGPU(MeshComponent* meshComponent) {//index��vertex�̐��f�[�^��GPU�ɑ���`���ɕς���

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

    // wstring �� UTF-8 string �ϊ�
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, filepath.c_str(), (int)filepath.size(), NULL, 0, NULL, NULL);
    std::string filepathUtf8(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, filepath.c_str(), (int)filepath.size(), &filepathUtf8[0], size_needed, NULL, NULL);

    if (!importer->Initialize(filepathUtf8.c_str(), -1, sdkManager->GetIOSettings()))
    {
        // �G���[�n���h�����O
        importer->Destroy();
        sdkManager->Destroy();
        return false;
    }

    FbxScene* scene = FbxScene::Create(sdkManager, "scene");
    importer->Import(scene);

    FbxGeometryConverter converter(sdkManager);
    converter.SplitMeshesPerMaterial(scene, true); //�}�e���A�����ƂɃ��b�V���𕪉�
    converter.Triangulate(scene, true); //�l�p�Ƃ��������Ă邩������Ȃ�FBX��S���O�p�`�����ɕϊ��B
    importer->Destroy();

    FbxNode* rootNode = scene->GetRootNode();
    if (rootNode)
    {
        for (int i = 0; i < rootNode->GetChildCount(); i++)
        {
            ParseNode(rootNode->GetChild(i), outModel);
        }
    }

    // �����ŏ����I��ParseSkeleton(scene), ParseAnimation(scene)�Ƃ��Ă�

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

    // �q�m�[�h���ċA�I�ɏ���
    for (int i = 0; i < node->GetChildCount(); i++)
    {
        ParseNode(node->GetChild(i), outModel);
    }
}

void FbxLoader::ParseMesh(FbxMesh* mesh, ModelData& outModel)
{
    // ���_�A�@���AUV�A�C���f�b�N�X�̒��o�������ōs��
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    int polygonCount = mesh->GetPolygonCount();

    for (int i = 0; i < polygonCount; i++)
    {
        int polygonSize = mesh->GetPolygonSize(i);
        assert(polygonSize == 3); // �O�p�`�̂ݑΉ��i�K�v�ɉ����ĎO�p�`���j

        for (int j = 0; j < polygonSize; j++)
        {
            int ctrlPointIndex = mesh->GetPolygonVertex(i, j);

            FbxVector4 position = mesh->GetControlPointAt(ctrlPointIndex);
            FbxVector4 normal;
            mesh->GetPolygonVertexNormal(i, j, normal);

            // UV�̓��C���[����擾
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

    // GPU�o�b�t�@�������͌Ăяo�����ł�邩�A�����ł���Ă��悢

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
//                return texture->GetFileName(); // �t���p�X�擾
//            }
//        }
//    }
//    return "";
//}
