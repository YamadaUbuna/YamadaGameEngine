#include "include\pch.h"
#include "include/Renderer.h"
#include <stdexcept>
#include <cassert>
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include <tchar.h>
#include "include/d3dx12.h"
#include <include/ModelComponent.h>

// シングルトンインスタンス
Renderer& Renderer::GetInstance() {
    static Renderer instance;
    return instance;
}

inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
        throw std::runtime_error("HRESULT failed.");
    }
}

void Renderer::Initialize(HWND hwnd, int width, int height)
{
    m_hwnd = hwnd;
    m_width = width;
    m_height = height;

    UINT dxgiFactoryFlags = 0;
    HRESULT hr = S_OK;

#if defined(_DEBUG)
    // デバッグレイヤー有効化
    {
        Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif

    //このエンジンが対応するDirectX仕様レベルを列挙
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_12_2, //Shader Model 6.5 + Direct3D 12.2 完全対応ハードも限定されるので注意。Windows SDK build 20170以上必須。
        D3D_FEATURE_LEVEL_12_1, //Shader Model 5.0 + Direct3D 12.1
        D3D_FEATURE_LEVEL_12_0, //Shader Model 5.0 + Direct3D 12.0

        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    //DX12は、まずFactoryを作る方法が楽。
    Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
    CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(factory.GetAddressOf()));

    {
        //ハードウェアアダプタを取得してDeviceを作成
        Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;

        Microsoft::WRL::ComPtr<IDXGIFactory6> factory6;

        if (SUCCEEDED(factory->QueryInterface(IID_PPV_ARGS(factory6.GetAddressOf()))))
        {
            //Factory6に対応しているのでハイスペックアダプタを探す
            for (
                UINT adapterIndex = 0;
                SUCCEEDED(factory6->EnumAdapterByGpuPreference( //ファクトリーからGPU機能の乗るアダプタを一つずつ取得
                    adapterIndex,
                    DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,   //HIGH_PERFORMANCEはD3D完全対応ハード用 低スペック版「DXGI_GPU_PREFERENCE_UNSPECIFIED」
                    IID_PPV_ARGS(adapter.GetAddressOf())));
                    ++adapterIndex)
            {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)    //一応ソフトウェア版（WARPアダプタ）はスキップ
                {
                    continue;
                }

                hr = E_FAIL;

                for (UINT featureLevel = 0; featureLevel < numFeatureLevels; featureLevel++)
                {
                    m_featureLevel = featureLevels[featureLevel];
                    if (SUCCEEDED(hr = D3D12CreateDevice(adapter.Get(), m_featureLevel, _uuidof(ID3D12Device), &m_device)))
                    {
                        break;
                    }

                }

                if (SUCCEEDED(hr))
                    break;
            }
        }
        if (m_device.Get() == nullptr)
        {
            //Factory6に対応出来なかったり、HIGH_PERFORMANCEで取得出来なかった（ハードがちょっと古かった）場合
            //全アダプタで使える物を探す
            for (UINT adapterIndex = 0; SUCCEEDED(factory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
            {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)    //一応ソフトウェア版（WARPアダプタ）はスキップ
                {
                    continue;
                }

                hr = E_FAIL;

                for (UINT featureLevel = 0; featureLevel < numFeatureLevels; featureLevel++)
                {
                    m_featureLevel = featureLevels[featureLevel];
                    if (SUCCEEDED(hr = D3D12CreateDevice(adapter.Get(), m_featureLevel, _uuidof(ID3D12Device), &m_device)))
                    {
                        break;
                    }

                }

                if (SUCCEEDED(hr))
                    break;
            }
        }
    }

    if (FAILED(hr))
    {
        throw std::runtime_error("これは例外です！");
    }

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_commandQueue.GetAddressOf()));

    // --- スワップチェイン作成 ---
    {
        DXGI_SWAP_CHAIN_DESC1 scDesc{};
        scDesc.BufferCount = FrameCount;
        scDesc.Width = width;
        scDesc.Height = height;
        scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        scDesc.SampleDesc.Count = 1;

        Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
        ThrowIfFailed(factory->CreateSwapChainForHwnd(
            m_commandQueue.Get(),
            hwnd,
            &scDesc,
            nullptr, nullptr,
            &swapChain));

        ThrowIfFailed(swapChain.As(&m_swapChain));

        m_currentFrameIndex = m_swapChain->GetCurrentBackBufferIndex();
    }


    // --- RTV ディスクリプタヒープ作成 ---
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
        heapDesc.NumDescriptors = FrameCount;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        ThrowIfFailed(m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_rtvHeap)));

        m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }

    // --- バックバッファ RTV 作成 ---
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // または UNORM_SRGB
        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

        for (UINT i = 0; i < FrameCount; ++i)
        {
            ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])));

            m_device->CreateRenderTargetView(m_renderTargets[i].Get(), &rtvDesc, rtvHandle);
            rtvHandle.Offset(1, m_rtvDescriptorSize);
        }

    }

    // --- コマンドアロケータ & コマンドリスト ---
    {
        ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));

        ThrowIfFailed(m_device->CreateCommandList(
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            m_commandAllocator.Get(),
            nullptr,
            IID_PPV_ARGS(&m_commandList)));
        m_commandList->Close(); // 初期状態で閉じておく

        // --- フェンスとイベント ---
        ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
        m_fenceValue = 1;

        m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (!m_fenceEvent)
            throw std::runtime_error("Failed to create fence event handle.");

        // --- 一時アップロードバッファ配列初期化 ---
        m_tempUploadBuffers.resize(FrameCount);
        m_fenceValues.resize(FrameCount, 0); // フェンス値の初期化

    }

    // --- 深度ステンシルバッファと DSV ヒープ作成 ---
    {
        // DSV用ディスクリプタヒープ作成
        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
        dsvHeapDesc.NumDescriptors = 1;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap));

        // 深度ステンシルバッファのリソース作成
        D3D12_RESOURCE_DESC depthStencilDesc = {};
        depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        depthStencilDesc.Alignment = 0;
        depthStencilDesc.Width = m_width;
        depthStencilDesc.Height = m_height;
        depthStencilDesc.DepthOrArraySize = 1;
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
        depthStencilDesc.SampleDesc.Count = 1;
        depthStencilDesc.SampleDesc.Quality = 0;
        depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
        CD3DX12_RESOURCE_DESC depthDesc = CD3DX12_RESOURCE_DESC::Tex2D(
            DXGI_FORMAT_D32_FLOAT,
            width,
            height,
            1, 0, 1, 0,
            D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
        );

        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = DXGI_FORMAT_D32_FLOAT;
        clearValue.DepthStencil.Depth = 1.0f;
        clearValue.DepthStencil.Stencil = 0;

        m_device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &depthDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &clearValue,
            IID_PPV_ARGS(&m_depthStencilBuffer)
        );

        // DSV作成
        m_device->CreateDepthStencilView(
            m_depthStencilBuffer.Get(),
            nullptr,
            m_dsvHeap->GetCPUDescriptorHandleForHeapStart()
        );

    }

}


void Renderer::RenderBegin()
{
    SignalAndWait(m_currentFrameIndex);

    // コマンドアロケータとリストをリセット
    if (FAILED(m_commandAllocator->Reset())) {
        throw std::runtime_error("Failed to reset command allocator");
    }
    if (FAILED(m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get()))) {
        throw std::runtime_error("Failed to reset command list");
    }

    // Present → RenderTarget に遷移
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_renderTargets[m_currentFrameIndex].Get(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET
    );
    m_commandList->ResourceBarrier(1, &barrier);

    // ビューポート / シザー
    D3D12_VIEWPORT viewport = { 0.0f, 0.0f, (FLOAT)m_width, (FLOAT)m_height, 0.0f, 1.0f };
    D3D12_RECT scissorRect = { 0, 0, (LONG)m_width, (LONG)m_height };
    m_commandList->RSSetViewports(1, &viewport);
    m_commandList->RSSetScissorRects(1, &scissorRect);

    // RTV と DSV ハンドル
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
        m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
        m_currentFrameIndex,
        m_rtvDescriptorSize
    );
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(
        m_dsvHeap->GetCPUDescriptorHandleForHeapStart()
    );

    // 出力ターゲットに RTV と DSV をセット
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

    // デプスバッファクリア
    m_commandList->ClearDepthStencilView(
        dsvHandle,
        D3D12_CLEAR_FLAG_DEPTH,
        1.0f,
        0,
        0, nullptr
    );

    // 画面クリア
    const float clearColor[] = { 0.2f, 0.2f, 0.4f, 1.0f };
    m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
}



void Renderer::RenderEnd()
{
    // バリア: RenderTarget -> Present
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_renderTargets[m_currentFrameIndex].Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT);
    m_commandList->ResourceBarrier(1, &barrier);

    // コマンドリストを閉じて実行
    m_commandList->Close();
    ID3D12CommandList* lists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(lists), lists);

    // Present 前にフェンスを打つ（このフレームの終わりを示す）
    m_fenceValue++;
    m_commandQueue->Signal(m_fence.Get(), m_fenceValue);

    // このフレームスロットに対応するフェンス値を保存
    m_fenceValues[m_currentFrameIndex] = m_fenceValue;

    // Present
    m_swapChain->Present(1, 0);

    //char buf[256];
    //sprintf_s(buf, sizeof(buf),
    //    "RenderEnd m_currentFrameIndex:0x%p,\n",
    //    m_currentFrameIndex);
    //OutputDebugStringA(buf);

    // 次のバックバッファインデックスを取得
    // ここで取得する m_frameIndex は次回 RenderBegin で使われるインデックス
    m_currentFrameIndex = m_swapChain->GetCurrentBackBufferIndex();

    // このスロットに紐づいた以前のアップロードバッファを安全に解放できるか確認
    UINT64 fenceForSlot = m_fenceValues[m_currentFrameIndex];
    if (m_fence->GetCompletedValue() >= fenceForSlot) {
        // GPU はこのスロットの全コマンドを終えている -> 解放
        m_tempUploadBuffers[m_currentFrameIndex].clear();
    }
}


void Renderer::DrawMesh(const ModelComponent& modelComp,
    const MaterialComponent& material,
    const TransformComponent& transform,
    const CameraComponent& camera,
    AssetManager& assetManager
)
{
    // ModelComponentからIDを取得
    const std::string& id = modelComp.GetModelId();

    // AssetManagerから実体を取得
    const ModelData* modelData = assetManager.GetModel(id);
    if (!modelData) return; // モデルが見つからなければ描画しない

    // ModelDataのメッシュ群を描画
    for (const auto& meshPtr : modelData->GetMeshes()) {
        const MeshComponent& mesh = *meshPtr;

        // 頂点バッファ・インデックスバッファセット
        D3D12_VERTEX_BUFFER_VIEW vbView = mesh.GetVertexBufferView();
        m_commandList->IASetVertexBuffers(0, 1, &vbView);

        D3D12_INDEX_BUFFER_VIEW ibView = mesh.GetIndexBufferView();
        m_commandList->IASetIndexBuffer(&ibView);

        m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // パイプラインステートとルートシグネチャ設定
        ID3D12PipelineState* pso = PipelineManager::GetInstance().GetPipelineState(material.GetPipelineType());
        ID3D12RootSignature* rootSig = PipelineManager::GetInstance().GetRootSignature(material.GetRootSignatureType());
        m_commandList->SetPipelineState(pso);
        m_commandList->SetGraphicsRootSignature(rootSig);

        // 定数バッファの更新とセット
        UpdateConstantBuffers(&transform, &camera);
        m_commandList->SetGraphicsRootConstantBufferView(2, m_worldCB->GetGPUVirtualAddress());
        m_commandList->SetGraphicsRootConstantBufferView(0, m_viewCB->GetGPUVirtualAddress());
        m_commandList->SetGraphicsRootConstantBufferView(1, m_projCB->GetGPUVirtualAddress());

        // 描画命令
        m_commandList->DrawIndexedInstanced(static_cast<UINT>(mesh.GetIndices().size()), 1, 0, 0, 0);
    }
}

void Renderer::Terminate()
{
    for (UINT i = 0; i < FrameCount; i++) {
        SignalAndWait(i);
    }

    for (auto& rt : m_renderTargets) {
        rt.Reset();
    }

    m_rtvHeap.Reset();

    m_tempUploadBuffers.clear();

    m_commandList.Reset();
    m_commandAllocator.Reset();
    m_pipelineState.Reset();
    m_commandQueue.Reset();
    m_fence.Reset();
}


Microsoft::WRL::ComPtr<ID3D12Resource> Renderer::CreateDefaultBuffer(const void* initData, UINT64 byteSize, D3D12_RESOURCE_STATES initialState)
{
    using namespace Microsoft::WRL;

    // 1) Default buffer を作る
    ComPtr<ID3D12Resource> defaultBuffer;
    CD3DX12_HEAP_PROPERTIES defaultHeapProps(D3D12_HEAP_TYPE_DEFAULT);
    CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(byteSize);

    if (FAILED(m_device->CreateCommittedResource(
        &defaultHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&defaultBuffer))))
    {
        throw std::runtime_error("Failed to create default buffer");
    }

    // 2) Upload buffer を作る（ローカル変数）
    ComPtr<ID3D12Resource> uploadBuffer;
    CD3DX12_HEAP_PROPERTIES uploadHeapProps(D3D12_HEAP_TYPE_UPLOAD);
    if (FAILED(m_device->CreateCommittedResource(
        &uploadHeapProps,
        D3D12_HEAP_FLAG_NONE,
        &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&uploadBuffer))))
    {
        throw std::runtime_error("Failed to create upload buffer");
    }

    // 3) データをマップしてコピー
    void* mapped = nullptr;
    CD3DX12_RANGE readRange(0, 0);
    if (FAILED(uploadBuffer->Map(0, &readRange, &mapped))) {
        throw std::runtime_error("Failed to map upload buffer");
    }
    memcpy(mapped, initData, static_cast<size_t>(byteSize));
    uploadBuffer->Unmap(0, nullptr);

    // 4) リソース作成用の一時コマンドアロケータ / リストを作る
    ComPtr<ID3D12CommandAllocator> tempAllocator;
    ComPtr<ID3D12GraphicsCommandList> tempCmdList;

    if (FAILED(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&tempAllocator)))) {
        throw std::runtime_error("Failed to create temp command allocator");
    }
    if (FAILED(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, tempAllocator.Get(), nullptr, IID_PPV_ARGS(&tempCmdList)))) {
        throw std::runtime_error("Failed to create temp command list");
    }

    // 5) CopyBufferRegion で転送
    tempCmdList->CopyBufferRegion(defaultBuffer.Get(), 0, uploadBuffer.Get(), 0, byteSize);

    // 6) コピー後に目的の状態へ遷移（用途に合わせて変更）
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        defaultBuffer.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        initialState // 必要に応じて INDEX_BUFFER 等に変更
    );
    tempCmdList->ResourceBarrier(1, &barrier);

    // 7) 実行して GPU 完了を待つ（ローカルでシグナル→待ち）
    ExecuteCommandListAndWait(m_commandQueue.Get(), tempCmdList.Get(), m_fence.Get(), m_fenceValue);

    // 8) アップロードバッファは GPU が使い終わるまで保持する必要がある
    //    フレームスロットに格納して、RenderEnd で解放判定する仕組みを使う。
    //    ここでは現在のフレームスロットに貯める。
    m_tempUploadBuffers[m_currentFrameIndex].push_back(uploadBuffer);

    // m_fenceValues[m_frameIndex] は上で更新済み（ExecuteCommandListAndWait が fenceValue を進めている）
    m_fenceValues[m_currentFrameIndex] = m_fenceValue;

    return  defaultBuffer;
}


void Renderer::ExecuteCommandListAndWait(ID3D12CommandQueue* commandQueue,
    ID3D12GraphicsCommandList* commandList,
    ID3D12Fence* fence,
    UINT64& fenceValue)
{
    // コマンドリストを閉じる
    commandList->Close();

    // コマンドキューに実行
    ID3D12CommandList* lists[] = { commandList };
    commandQueue->ExecuteCommandLists(_countof(lists), lists);

    // GPU 完了を待機
    fenceValue++;
    commandQueue->Signal(fence, fenceValue);

    if (fence->GetCompletedValue() < fenceValue)
    {
        HANDLE eventHandle = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (!eventHandle)
            throw std::runtime_error("Failed to create fence event");

        fence->SetEventOnCompletion(fenceValue, eventHandle);
        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
}

void Renderer::WaitForGpu(UINT frameIndex)
{
    const UINT64 fenceValue = m_fenceValues[frameIndex];
    if (m_fence->GetCompletedValue() < fenceValue)
    {
        m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent);
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }
}

void Renderer::SignalAndWait(UINT frameIndex)
{
    m_fenceValues[frameIndex]++;
    m_commandQueue->Signal(m_fence.Get(), m_fenceValues[frameIndex]);
    WaitForGpu(frameIndex);
}

inline UINT AlignTo256(UINT size) {
    return (size + 255u) & ~255u;
}

void Renderer::UpdateConstantBuffers(const TransformComponent* transform, const CameraComponent* camera)
{
    // 基本チェック
    if (!transform) {
        OutputDebugStringA("UpdateConstantBuffers: transform is null\n");
        return;
    }
    if (!camera) {
        OutputDebugStringA("UpdateConstantBuffers: camera is null\n");
        return;
    }

    auto CreateAndMapCB = [&](Microsoft::WRL::ComPtr<ID3D12Resource>& cbResource, const void* srcData, size_t size) -> HRESULT {
        if (!cbResource) {
            D3D12_HEAP_PROPERTIES heapProps = {};
            heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
            heapProps.CreationNodeMask = 1;
            heapProps.VisibleNodeMask = 1;

            D3D12_RESOURCE_DESC resourceDesc = {};
            resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
            resourceDesc.Width = AlignTo256(static_cast<UINT>(size)); // 256バイト境界に揃える
            resourceDesc.Height = 1;
            resourceDesc.DepthOrArraySize = 1;
            resourceDesc.MipLevels = 1;
            resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
            resourceDesc.SampleDesc.Count = 1;
            resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
            resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

            HRESULT hr = Renderer::GetInstance().GetDevice()->CreateCommittedResource(
                &heapProps,
                D3D12_HEAP_FLAG_NONE,
                &resourceDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&cbResource)
            );
            if (FAILED(hr)) {
                OutputDebugStringA("CreateCommittedResource failed in CreateAndMapCB\n");
                return hr;
            }
        }

        // Map の際は読み取り範囲を空にする（upload heap は CPU->GPU 書き込みのみを想定）
        void* mappedPtr = nullptr;
        D3D12_RANGE readRange = { 0, 0 }; // CPU が読み取らないことを示す
        HRESULT hrMap = cbResource->Map(0, &readRange, &mappedPtr);
        if (FAILED(hrMap) || mappedPtr == nullptr) {
            OutputDebugStringA("ID3D12Resource::Map failed in CreateAndMapCB\n");
            return hrMap;
        }

        // 安全にコピー（sizeバイトだけコピーする）
        memcpy(mappedPtr, srcData, size);

        // Upload heap は Unmap してもよいが、継続利用するなら Map のままでも可
        cbResource->Unmap(0, nullptr);

        return S_OK;
        };

    // 行列を取得して転置（DirectXMath の型が XMMATRIX のまま使えるならそのまま渡す）
    DirectX::XMMATRIX world = XMMatrixTranspose(transform->GetWorldMatrix());
    DirectX::XMMATRIX view = XMMatrixTranspose(camera->GetViewMatrix());
    DirectX::XMMATRIX proj = XMMatrixTranspose(camera->GetProjMatrix());

    HRESULT hr = S_OK;
    hr = CreateAndMapCB(m_worldCB, &world, sizeof(world));
    if (FAILED(hr)) {
        OutputDebugStringA("UpdateConstantBuffers: CreateAndMapCB failed for world\n");
        return;
    }

    hr = CreateAndMapCB(m_viewCB, &view, sizeof(view));
    if (FAILED(hr)) {
        OutputDebugStringA("UpdateConstantBuffers: CreateAndMapCB failed for view\n");
        return;
    }

    hr = CreateAndMapCB(m_projCB, &proj, sizeof(proj));
    if (FAILED(hr)) {
        OutputDebugStringA("UpdateConstantBuffers: CreateAndMapCB failed for proj\n");
        return;
    }

}


