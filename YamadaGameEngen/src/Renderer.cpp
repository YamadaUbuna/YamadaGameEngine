#include "include\pch.h"
#include "include/Renderer.h"
#include <stdexcept>
#include <cassert>
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include <tchar.h>
#include "include/d3dx12.h"

// シングルトンインスタンス
Renderer& Renderer::GetInstance() {
    static Renderer instance;
    return instance;
}

void Renderer::Initialize(HWND hwnd, int width, int height)
{
    m_hwnd = hwnd;
    m_width = width;
    m_height = height;

    UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    // デバッグレイヤー有効化
    {
        Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
            debugController->EnableDebugLayer();
        }
    }
#endif

    // DXGIファクトリ作成
    Microsoft::WRL::ComPtr<IDXGIFactory6> factory;
    if (FAILED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)))) {
        throw std::runtime_error("Failed to create DXGI Factory.");
    }

    // デバイス作成（ハードウェアアダプタを選ぶ）
    {
        Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
        for (UINT i = 0; factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
                continue; // ソフトウェアアダプタはスキップ
            }

            // デバイス作成できるか試す
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)))) {
                break;
            }
        }

        if (!m_device) {
            throw std::runtime_error("Failed to create D3D12 Device.");
        }
    }

    // コマンドキュー作成
    {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

        if (FAILED(m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue)))) {
            throw std::runtime_error("Failed to create command queue.");
        }
    }

    // スワップチェイン作成
    {
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.BufferCount = FrameCount;
        swapChainDesc.Width = width;
        swapChainDesc.Height = height;
        swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.SampleDesc.Count = 1;

        Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
        if (FAILED(factory->CreateSwapChainForHwnd(
            m_commandQueue.Get(),        // コマンドキュー
            hwnd,                        // ウィンドウハンドル
            &swapChainDesc,
            nullptr, nullptr,
            &swapChain)))
        {
            throw std::runtime_error("Failed to create swap chain.");
        }

        // IDXGISwapChain1 → IDXGISwapChain3 にキャスト
        if (FAILED(swapChain.As(&m_swapChain))) {
            throw std::runtime_error("Failed to cast swap chain.");
        }

        m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
    }

    // ディスクリプタヒープ（RTV用）作成
    {
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = FrameCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        if (FAILED(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)))) {
            throw std::runtime_error("Failed to create RTV descriptor heap.");
        }

        m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }

    // バックバッファ用のRTVを作成
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

        for (UINT i = 0; i < FrameCount; ++i) {
            if (FAILED(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])))) {
                throw std::runtime_error("Failed to get swap chain buffer.");
            }

            m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, m_rtvDescriptorSize);
        }
    }

    // コマンドアロケータ
    m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));

    // コマンドリスト
    m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList));
    m_commandList->Close(); // 初期状態は閉じておく

    // フェンスとイベント
    m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
    m_fenceValue = 1;
    m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

}

void Renderer::RenderBegin()
{
    // コマンドアロケータとリストをリセット
    m_commandAllocator->Reset();
    m_commandList->Reset(m_commandAllocator.Get(), nullptr);

    // バリア: Present → RenderTarget
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_renderTargets[m_frameIndex].Get(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET
    );
    m_commandList->ResourceBarrier(1, &barrier);

    // ビューポート・シザー
    D3D12_VIEWPORT viewport = { 0.0f, 0.0f, (float)m_width, (float)m_height, 0.0f, 1.0f };
    D3D12_RECT scissorRect = { 0, 0, m_width, m_height };
    m_commandList->RSSetViewports(1, &viewport);
    m_commandList->RSSetScissorRects(1, &scissorRect);

    // レンダーターゲットをセット
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
        m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
        m_frameIndex,
        m_rtvDescriptorSize
    );
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    // 画面クリア
    const float clearColor[] = { 0.2f, 0.2f, 0.4f, 1.0f }; // 適当な色
    m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
}

void Renderer::RenderEnd()
{
    // バリア: RenderTarget → Present
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_renderTargets[m_frameIndex].Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT
    );
    m_commandList->ResourceBarrier(1, &barrier);

    // コマンドリストを閉じて実行
    m_commandList->Close();
    ID3D12CommandList* cmdLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

    // スワップチェイン Present
    m_swapChain->Present(1, 0);

    // フレーム同期（簡易的にフェンスで）
    const UINT64 fenceToWaitFor = m_fenceValue;
    m_commandQueue->Signal(m_fence.Get(), fenceToWaitFor);
    m_fenceValue++;

    if (m_fence->GetCompletedValue() < fenceToWaitFor) {
        m_fence->SetEventOnCompletion(fenceToWaitFor, m_fenceEvent);
        WaitForSingleObject(m_fenceEvent, INFINITE);
    }

    // 次のバックバッファインデックスを取得
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

}

void Renderer::Terminate()
{
}

void Renderer::DrawMesh(const MeshComponent& mesh, const MaterialComponent& material, const TransformComponent& transform)
{
    m_commandAllocator->Reset();
    m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get());

    // 必要なバッファをセット
    m_commandList->IASetVertexBuffers(0, 1, &mesh.GetVertexBufferView());
    m_commandList->IASetIndexBuffer(&mesh.GetIndexBufferView());

    // PSO・RootSignatureセット
    ID3D12PipelineState* pso = PipelineManager::GetInstance().GetPipelineState(material.GetPipelineType());
    ID3D12RootSignature* rootSig = PipelineManager::GetInstance().GetRootSignature(material.GetRootSignatureType());
    m_commandList->SetPipelineState(pso);
    m_commandList->SetGraphicsRootSignature(rootSig);

    // CBV（transform）バインド
    m_commandList->SetGraphicsRootConstantBufferView(0, InitializeConstantBufferData());

    // SRV（テクスチャなど）もバインド
    m_commandList->SetGraphicsRootDescriptorTable(1, material->srvHandle);

    // 描画
    m_commandList->DrawIndexedInstanced(mesh.GetIndices().size(), 1, 0, 0, 0);

    // 同様にインデックスバッファも必要なら作る

    // 補足：より効率的な転送方法

    //    この方法は「アップロードヒープ」を使ってるので** シンプルで安全** ですが、

    //    - 大きなモデルや大量のデータを描画する場合は、
    //    - 一時的に `UPLOAD` ヒープにコピー
    //    - 最終的に `DEFAULT` ヒープ（GPU専用）に転送
    //    - コピーコマンドで効率よく処理する

    //    というステップに分けるのがベストプラクティスです。
}

void Renderer::SetCamera(const CameraComponent& camera)
{
}

void Renderer::WaitForPreviousFrame()
{
}

void Renderer::CreateRenderTargetViews()
{
}

void Renderer::CreateRootSignature()
{
}

void Renderer::CreatePipelineState()
{
}

D3D12_GPU_VIRTUAL_ADDRESS Renderer::InitializeConstantBufferData(TransformComponent* transform, CameraComponent* camera)
{
    // 1. 定数バッファ用のリソースを作成
    Microsoft::WRL::ComPtr<ID3D12Resource> constantBuffer;

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resourceDesc.Alignment = 0;
    resourceDesc.Width = (sizeof(ConstantBufferData) + 255) & ~255; // 256バイトアラインメント
    resourceDesc.Height = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    Renderer::GetInstance().GetDevice()->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&constantBuffer)
    );

    // 2. 定数バッファにCPUからアクセスするためのポインタを取得（マップ）
    ConstantBufferData* cbData = nullptr;
    constantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&cbData));

    //// 3. cbDataにデータをコピー
    //*cbData = 
    cbData->world = XMMatrixTranspose(transform->GetWorldMatrix());
    cbData->view = XMMatrixTranspose(camera->GetViewMatrix());
    cbData->projection = XMMatrixTranspose(camera->GetProjectionMatrix());

    // 4. cbvGpuAddressとしてGPU仮想アドレスを取得
    D3D12_GPU_VIRTUAL_ADDRESS cbvGpuAddress = constantBuffer->GetGPUVirtualAddress();

}
