#include "include\pch.h"
#include "include/RenderSystem.h"
#include "include/Renderer.h"
#include <stdexcept>

//ここに関数作って、MeshComponentの情報を綺麗にする。


void RenderSystem::Initialize(IScene& scene)
{
    // 初期化処理が必要な場合はここに記述
    const std::vector<std::unique_ptr<IEntity>>& entities = scene.GetEntities();
    for (const std::unique_ptr<IEntity>& entity : entities) {
        MeshComponent* meshComponent = entity->GetComponent<MeshComponent>();
		MaterialComponent* materialComponent = entity->GetComponent<MaterialComponent>();
		TransformComponent* transformComponent = entity->GetComponent<TransformComponent>();

        //if (renderComponent) {
        //    renderComponent->SetColor(1.0f, 1.0f, 1.0f);
        //    OutputDebugStringA("RenderSystem Initialized.\n");
        //}
        InitializeMeshBuffers(meshComponent);

    }
}

void RenderSystem::Update(IScene& scene, float deltaTime)
{
    const std::vector<std::unique_ptr<IEntity>>& entities = scene.GetEntities();

    for (const std::unique_ptr<IEntity>& entity : entities) {
        MeshComponent* mesh = entity->GetComponent<MeshComponent>();
        MaterialComponent* material = entity->GetComponent<MaterialComponent>();
        TransformComponent* transform = entity->GetComponent<TransformComponent>();

        if (!mesh || !material || !transform) {
            // 描画に必要な情報が揃っていないのでスキップ
            continue;
        }



        Renderer::GetInstance().DrawMesh(*mesh, *material, *transform);

        //RenderComponent* renderComponent = entity->GetComponent<RenderComponent>();
        //if (renderComponent) {
        //    // 仮の描画（実際はDirectX等で描画）
        //    
        //    char buffer[128];
        //    sprintf_s(buffer, "Draw Entity with color RGBA(%f, %f, %f, %f)\n",
        //        renderComponent->r, renderComponent->g, renderComponent->b, renderComponent->a);
        //    OutputDebugStringA(buffer);
        //}
    }
}


Microsoft::WRL::ComPtr<ID3D12Resource> RenderSystem::CreateDefaultBuffer(
    const void* initData,
    UINT64 byteSize)
{
    using namespace Microsoft::WRL;

    ComPtr<ID3D12Resource> defaultBuffer;
    ComPtr<ID3D12Resource> uploadBuffer;

    // 1. Default Heap（GPU専用）
    CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(byteSize);

    HRESULT hr = Renderer::GetInstance().GetDevice()->CreateCommittedResource(
        &defaultHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_COPY_DEST, // 初期状態は COPY_DEST
        nullptr,
        IID_PPV_ARGS(&defaultBuffer));
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create default buffer");
    }

    // 2. Upload Heap（CPUアクセス可能）
    CD3DX12_HEAP_PROPERTIES uploadHeapProps(D3D12_HEAP_TYPE_UPLOAD);
    ComPtr<ID3D12Resource> uploadBufferTemp;
    hr = Renderer::GetInstance().GetDevice()->CreateCommittedResource(
        &uploadHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&uploadBufferTemp));
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to create upload buffer");
    }

    // 3. データをマップしてコピー
    void* mappedData = nullptr;
    CD3DX12_RANGE readRange(0, 0); // 読み取りは行わない
    hr = uploadBufferTemp->Map(0, &readRange, &mappedData);
    if (FAILED(hr)) {
        throw std::runtime_error("Failed to map upload buffer");
    }

    memcpy(mappedData, initData, static_cast<size_t>(byteSize));
    uploadBufferTemp->Unmap(0, nullptr);

    // 4. コマンドリストにコピー命令を出す（即時コマンドリストを使う）
    auto commandList = Renderer::GetInstance().GetCommandList(); // ←取得手段はプロジェクトに合わせてね

    commandList->CopyBufferRegion(defaultBuffer.Get(), 0, uploadBufferTemp.Get(), 0, byteSize);

    // 5. コピー完了後、RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER などに遷移させるのが理想
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        defaultBuffer.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_GENERIC_READ
    );
    commandList->ResourceBarrier(1, &barrier);

    // 注意：uploadBuffer を関数の外で保持しないと、GPUが読み取る前に解放される可能性あり
    // 今回は簡単のため戻さないが、RenderContextなどで管理すると安心

    return defaultBuffer;
}





void RenderSystem::InitializeMeshBuffers(MeshComponent* meshComponent) {//indexとvertexの生データをGPUに送る形式に変える

    const UINT vbSize = static_cast<UINT>(meshComponent->GetVertices().size() * sizeof(Vertex));
    const UINT ibSize = static_cast<UINT>(meshComponent->GetIndices().size() * sizeof(uint32_t));

    meshComponent->SetVertexBuffer(CreateDefaultBuffer(meshComponent->GetVertices().data(), vbSize));
    D3D12_VERTEX_BUFFER_VIEW vbView = {};
    vbView.BufferLocation = meshComponent->GetVertexBuffer()->GetGPUVirtualAddress();
    vbView.StrideInBytes = sizeof(Vertex);
    vbView.SizeInBytes = vbSize;
    meshComponent->SetVertexBufferView(vbView);

    meshComponent->SetIndexBuffer(CreateDefaultBuffer(meshComponent->GetIndices().data(), ibSize));
    D3D12_INDEX_BUFFER_VIEW ibView = {};
    ibView.BufferLocation = meshComponent->GetIndexBuffer()->GetGPUVirtualAddress();
    ibView.SizeInBytes = ibSize;
    ibView.Format = DXGI_FORMAT_R32_UINT;
    meshComponent->SetIndexBufferView(ibView);
}
