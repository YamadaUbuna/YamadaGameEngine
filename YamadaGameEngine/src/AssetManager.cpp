#include "include\pch.h"
#include "include/AssetManager.h"
#include "include/Renderer.h"

#include "include/d3dx12.h"
#include <algorithm>
#include <stdexcept>


#pragma comment(lib, "DirectXTex.lib")


bool AssetManager::LoadModel(const std::string& id, const std::wstring& filepath)
{
    // モデルがすでに存在している場合はロードしない
    if (m_models.find(id) != m_models.end())
        return false; // すでに存在する

    // ModelDataを生成
    auto modelData = std::make_unique<ModelDataContainer>();

    // FBX読み込み
    if (!LoadModel(filepath, *modelData))
        return false;

    //モデルのメッシュをgpuに送る
    for (const auto& mesh : modelData->GetMeshes()) {
        UploadMeshToGPU(mesh.get()); // unique_ptr から参照を取得して渡す
    }

    // モデルデータを登録
    m_models[id] = std::move(modelData);

    return true;
}

std::wstring AssetManager::LoadTexture(const std::filesystem::path& fullPath)
{
    // ファイル名だけ取り出して自前の相対パスに変換
    std::wstring fileName = fullPath.filename().wstring();
    std::wstring filepath = L"Resource/Textures/" + fileName;

    // すでに読み込まれていれば返す
    auto it = m_textures.find(filepath);
    if (it != m_textures.end())
        return it->first;

    TextureResource texRes;

    // PSDならTGAに変換
    if (filepath.ends_with(L".psd"))
    {
        filepath.replace(filepath.size() - 4, 4, L".tga");
        fileName = filepath.substr(filepath.find_last_of(L"\\") + 1); // ファイル名更新
    }

    // 拡張子判定（小文字に統一）
    std::wstring ext = filepath.substr(filepath.find_last_of(L'.') + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);

    HRESULT hr = S_OK;
    DirectX::ScratchImage image;

    if (ext == L"png")
    {
        hr = DirectX::LoadFromWICFile(filepath.c_str(), DirectX::WIC_FLAGS_FORCE_RGB, nullptr, image);
    }
    else if (ext == L"tga")
    {
        hr = DirectX::LoadFromTGAFile(filepath.c_str(), nullptr, image);
    }
    else
    {
        OutputDebugStringW((L"Unsupported texture format: " + filepath + L"\n").c_str());
        return L"";
    }

    if (FAILED(hr))
    {
        OutputDebugStringW((L"Failed to load texture: " + filepath + L"\n").c_str());
        return L"";
    }

    // GPU にアップロード
    hr = CreateTextureOnGPU(image.GetImages(), image.GetImageCount(), image.GetMetadata(), texRes.resource);
    if (FAILED(hr)) return L"";

    // SRV を作成
    texRes.srv = CreateShaderResourceView(texRes.resource);

    // マップに保存
    m_textures[filepath] = std::make_unique<TextureResource>(std::move(texRes));

    return filepath; // MaterialComponent にセットするID
}

const TextureResource* AssetManager::GetTexture(const std::wstring& id) const
{
    auto it = m_textures.find(id);
    if (it == m_textures.end()) {
        throw std::runtime_error("そんなてくすちゃないですけど");
        return nullptr;
    }
    return it->second.get();
}

ModelDataContainer* AssetManager::GetModelForUpdate(const std::string& id) 
{
    auto it = m_models.find(id);
    if (it == m_models.end()) {
        throw std::runtime_error("そんなもでるないですけど");
        return nullptr;
    }
    return it->second.get();

}

HRESULT AssetManager::CreateTextureOnGPU(
    const DirectX::Image* images,
    size_t nImages,
    const DirectX::TexMetadata& metadata,
    Microsoft::WRL::ComPtr<ID3D12Resource>& outResource)
{
    auto device = Renderer::GetInstance().GetDevice();
    auto commandQueue = Renderer::GetInstance().GetCommandQueue();

    // --- コマンドアロケータ & コマンドリスト ---
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdAlloc;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> cmdList;

    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmdAlloc));
    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cmdAlloc.Get(), nullptr, IID_PPV_ARGS(&cmdList));


    // --- GPU用テクスチャリソース作成 ---
    D3D12_RESOURCE_DESC texDesc = {};
    texDesc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
    texDesc.Width = static_cast<UINT>(metadata.width);
    texDesc.Height = static_cast<UINT>(metadata.height);
    texDesc.DepthOrArraySize = static_cast<UINT16>(metadata.arraySize);
    texDesc.MipLevels = static_cast<UINT16>(metadata.mipLevels);
    texDesc.Format = metadata.format;
    texDesc.SampleDesc.Count = 1;
    texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);

    HRESULT hr = device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &texDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&outResource)
    );
    if (FAILED(hr)) return hr;

    // --- アップロードバッファ作成 ---
    const UINT subresourceCount = static_cast<UINT>(nImages);
    const UINT64 uploadBufferSize = GetRequiredIntermediateSize(outResource.Get(), 0, subresourceCount);

    Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;
    CD3DX12_HEAP_PROPERTIES uploadHeapProps(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC uploadDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

    hr = device->CreateCommittedResource(
        &uploadHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &uploadDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&uploadBuffer)
    );
    if (FAILED(hr)) return hr;

    // --- SubresourceData 配列作成 ---
    std::vector<D3D12_SUBRESOURCE_DATA> subresources(subresourceCount);
    for (size_t i = 0; i < nImages; ++i)
    {
        subresources[i].pData = images[i].pixels;
        subresources[i].RowPitch = images[i].rowPitch;
        subresources[i].SlicePitch = images[i].slicePitch;
    }

    // --- GPUへデータ転送 ---
    UpdateSubresources(
        cmdList.Get(),
        outResource.Get(),
        uploadBuffer.Get(),
        0, 0, subresourceCount,
        subresources.data()
    );

    // --- 状態遷移: COPY_DEST -> PIXEL_SHADER_RESOURCE ---
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        outResource.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
    );
    cmdList->ResourceBarrier(1, &barrier);

    // コマンドリストを閉じて実行
    cmdList->Close();
    ID3D12CommandList* lists[] = { cmdList.Get() };
    commandQueue->ExecuteCommandLists(_countof(lists), lists);

    // 一時的にGPU完了待ち（frameIndex不要）
    ComPtr<ID3D12Fence> tempFence;
    UINT64 tempFenceValue = 1;
    device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&tempFence));

    HANDLE eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    commandQueue->Signal(tempFence.Get(), tempFenceValue);
    tempFence->SetEventOnCompletion(tempFenceValue, eventHandle);
    WaitForSingleObject(eventHandle, INFINITE);
    CloseHandle(eventHandle);


    return S_OK;
}


D3D12_GPU_DESCRIPTOR_HANDLE AssetManager::CreateShaderResourceView(Microsoft::WRL::ComPtr<ID3D12Resource>& resource)
{
    auto& renderer = Renderer::GetInstance();
    auto device = renderer.GetDevice();
    auto srvHeap = renderer.GetSrvHeap();
    auto descriptorSize = renderer.GetSrvDescriptorSize();

    UINT index = renderer.AllocateSrvIndex();

    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();
    cpuHandle.ptr += index * descriptorSize;

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = resource->GetDesc().Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = resource->GetDesc().MipLevels;

    device->CreateShaderResourceView(resource.Get(), &srvDesc, cpuHandle);

    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
    gpuHandle.ptr += index * descriptorSize;

    return gpuHandle;
}


void AssetManager::UploadMeshToGPU(MeshComponent* meshComponent) {//indexとvertexの生データをGPUに送る形式に変える

    const UINT vbSize = static_cast<UINT>(meshComponent->GetVertices().size() * sizeof(FbxVertex));
    const UINT ibSize = static_cast<UINT>(meshComponent->GetIndices().size() * sizeof(uint32_t));

    meshComponent->SetVertexBuffer(Renderer::GetInstance().CreateDefaultBuffer(meshComponent->GetVertices().data(), vbSize, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
    D3D12_VERTEX_BUFFER_VIEW vbView = {};
    vbView.BufferLocation = meshComponent->GetVertexBuffer()->GetGPUVirtualAddress();
    vbView.StrideInBytes = sizeof(FbxVertex);
    vbView.SizeInBytes = vbSize;
    meshComponent->SetVertexBufferView(vbView);

    meshComponent->SetIndexBuffer(Renderer::GetInstance().CreateDefaultBuffer(meshComponent->GetIndices().data(), ibSize, D3D12_RESOURCE_STATE_INDEX_BUFFER));
    D3D12_INDEX_BUFFER_VIEW ibView = {};
    ibView.BufferLocation = meshComponent->GetIndexBuffer()->GetGPUVirtualAddress();
    ibView.SizeInBytes = ibSize;
    ibView.Format = DXGI_FORMAT_R32_UINT;
    meshComponent->SetIndexBufferView(ibView);
}

bool AssetManager::LoadModel(const std::wstring& filepath, ModelDataContainer& outModel)
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
        importer->Destroy();
        sdkManager->Destroy();
        return false;
    }

    FbxScene* scene = FbxScene::Create(sdkManager, "scene");
    importer->Import(scene);

    FbxGeometryConverter converter(sdkManager);
    converter.SplitMeshesPerMaterial(scene, true);
    converter.Triangulate(scene, true);
    importer->Destroy();

    FbxNode* rootNode = scene->GetRootNode();
    if (rootNode)
    {
        for (int i = 0; i < rootNode->GetChildCount(); i++)
        {
            ParseNode(rootNode->GetChild(i), outModel);
        }
    }

    sdkManager->Destroy();
    return true;
}

void AssetManager::ParseNode(FbxNode* node, ModelDataContainer& outModel)
{
    if (!node) return;

    FbxMesh* fbxMesh = node->GetMesh();
    if (fbxMesh) {
        ParseMesh(node, fbxMesh, outModel);
    }

    for (int i = 0; i < node->GetChildCount(); ++i) {
        ParseNode(node->GetChild(i), outModel);
    }
}

void AssetManager::ParseMesh(FbxNode* node, FbxMesh* mesh, ModelDataContainer& outModel)
{
    std::vector<FbxVertex> vertices;
    std::vector<uint32_t> indices;

    int polygonCount = mesh->GetPolygonCount();
    for (int i = 0; i < polygonCount; i++)
    {
        int polygonSize = mesh->GetPolygonSize(i);
        assert(polygonSize == 3);

        for (int j = 0; j < polygonSize; j++)
        {
            int ctrlPointIndex = mesh->GetPolygonVertex(i, j);

            FbxVector4 position = mesh->GetControlPointAt(ctrlPointIndex);
            FbxVector4 normal;
            mesh->GetPolygonVertexNormal(i, j, normal);

            // UV取得
            FbxStringList uvSetNameList;
            mesh->GetUVSetNames(uvSetNameList);
            FbxVector2 uv(0, 0);
            if (uvSetNameList.GetCount() > 0)
            {
                bool unmapped;
                mesh->GetPolygonVertexUV(i, j, uvSetNameList[0], uv, unmapped);
            }

            FbxVertex v;
            v.position = { (float)position[0], (float)position[1], (float)position[2] };
            v.normal = { (float)normal[0],   (float)normal[1],   (float)normal[2] };
            v.uv = { (float)uv[0],       (float)uv[1] };
            vertices.push_back(v);
            indices.push_back(static_cast<uint32_t>(vertices.size() - 1));
        }
    }

    auto meshComponent = std::make_unique<MeshComponent>();
    meshComponent->SetVertices(vertices);
    meshComponent->SetIndices(indices);

    // ===== Material登録 =====
    if (node->GetMaterialCount() > 0)
    {
        FbxSurfaceMaterial* fbxMaterial = node->GetMaterial(0);
        if (fbxMaterial)
        {
            std::string materialId = ParseMaterial(fbxMaterial);
            meshComponent->SetMaterialId(materialId);
        }
    }

    outModel.AddMesh(std::move(meshComponent));
}

std::string AssetManager::ParseMaterial(FbxSurfaceMaterial* fbxMaterial)
{
    if (!fbxMaterial) return {};

    // まずIDを決定
    std::string materialId = fbxMaterial->GetName();
    if (materialId.empty()) {
        throw std::runtime_error("matIDがないよぅ");
    }

    // すでに登録済みなら、そのIDを返して終了
    if (m_materials.find(materialId) != m_materials.end()) {
        return materialId;
    }

    auto material = std::make_unique<MaterialComponent>();

    // Lambert / Phong などで試す色候補
    const char* element_list[] = {
        FbxSurfaceMaterial::sDiffuse,
        FbxSurfaceMaterial::sAmbient,
        FbxSurfaceMaterial::sSpecular
    };

    const char* factor_list[] = {
        FbxSurfaceMaterial::sDiffuseFactor,
        FbxSurfaceMaterial::sAmbientFactor,
        FbxSurfaceMaterial::sSpecularFactor
    };

    bool colorSet = false;

    for (int i = 0; i < 3 && !colorSet; ++i)
    {
        FbxProperty prop = fbxMaterial->FindProperty(element_list[i]);
        if (prop.IsValid())
        {
            FbxDouble3 color = prop.Get<FbxDouble3>();
            double factor = 1.0;

            FbxProperty factorProp = fbxMaterial->FindProperty(factor_list[i]);
            if (factorProp.IsValid())
                factor = factorProp.Get<double>();

            material->SetBaseColor({
                static_cast<float>(color[0] * factor),
                static_cast<float>(color[1] * factor),
                static_cast<float>(color[2] * factor),
                1.0f
                });
            colorSet = true;

            // テクスチャ取得
            int texCount = prop.GetSrcObjectCount<FbxTexture>();
            for (int t = 0; t < texCount; ++t)
            {
                FbxTexture* fbxTex = prop.GetSrcObject<FbxTexture>(t);
                if (!fbxTex) continue;
                FbxFileTexture* fileTex = FbxCast<FbxFileTexture>(fbxTex);
                if (!fileTex) continue;

                std::string texPath = fileTex->GetFileName();
                std::wstring texID = LoadTexture(texPath);
                material->SetAlbedoTextureId(texID);
            }
        }
    }

    // パイプライン・ルートシグネチャ設定（仮のデフォルト）
    material->SetRootSignatureType(RootSignatureType::def);
    material->SetPipelineType(PipelineType::fbx);

    // 登録
    m_materials[materialId] = std::move(material);

    return materialId;
}



