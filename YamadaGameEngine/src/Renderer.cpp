#include "include\pch.h"
#include "include/Renderer.h"
#include "include/d3dx12.h"

#include <stdexcept>
#include <cassert>
#include <d3dcompiler.h>
#include <tchar.h>


// �V���O���g���C���X�^���X
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
    // �f�o�b�O���C���[�L����
    {
        Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif

    //���̃G���W�����Ή�����DirectX�d�l���x�����
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_12_2, //Shader Model 6.5 + Direct3D 12.2 ���S�Ή��n�[�h�����肳���̂Œ��ӁBWindows SDK build 20170�ȏ�K�{�B
        D3D_FEATURE_LEVEL_12_1, //Shader Model 5.0 + Direct3D 12.1
        D3D_FEATURE_LEVEL_12_0, //Shader Model 5.0 + Direct3D 12.0

        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);


    Microsoft::WRL::ComPtr<IDXGIFactory4> factory;
    CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(factory.GetAddressOf()));

    {
        //�n�[�h�E�F�A�A�_�v�^���擾����Device���쐬
        Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;

        Microsoft::WRL::ComPtr<IDXGIFactory6> factory6;

        if (SUCCEEDED(factory->QueryInterface(IID_PPV_ARGS(factory6.GetAddressOf()))))
        {
            //Factory6�ɑΉ����Ă���̂Ńn�C�X�y�b�N�A�_�v�^��T��
            for (
                UINT adapterIndex = 0;
                SUCCEEDED(factory6->EnumAdapterByGpuPreference( //�t�@�N�g���[����GPU�@�\�̏��A�_�v�^������擾
                    adapterIndex,
                    DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,   //HIGH_PERFORMANCE��D3D���S�Ή��n�[�h�p ��X�y�b�N�ŁuDXGI_GPU_PREFERENCE_UNSPECIFIED�v
                    IID_PPV_ARGS(adapter.GetAddressOf())));
                    ++adapterIndex)
            {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)    //�ꉞ�\�t�g�E�F�A�ŁiWARP�A�_�v�^�j�̓X�L�b�v
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
            //Factory6�ɑΉ��o���Ȃ�������AHIGH_PERFORMANCE�Ŏ擾�o���Ȃ������i�n�[�h��������ƌÂ������j�ꍇ
            //�S�A�_�v�^�Ŏg���镨��T��
            for (UINT adapterIndex = 0; SUCCEEDED(factory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
            {
                DXGI_ADAPTER_DESC1 desc;
                adapter->GetDesc1(&desc);

                if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)    //�ꉞ�\�t�g�E�F�A�ŁiWARP�A�_�v�^�j�̓X�L�b�v
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
        throw std::runtime_error("����͗�O�ł��I");
    }

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_commandQueue.GetAddressOf()));

    // --- �X���b�v�`�F�C���쐬 ---
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


    // --- RTV �f�B�X�N���v�^�q�[�v�쐬 ---
    {
        D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
        heapDesc.NumDescriptors = FrameCount;
        heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        ThrowIfFailed(m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_rtvHeap)));

        m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }

    // --- �o�b�N�o�b�t�@ RTV �쐬 ---
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // �܂��� UNORM_SRGB
        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

        for (UINT i = 0; i < FrameCount; ++i)
        {
            ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])));

            m_device->CreateRenderTargetView(m_renderTargets[i].Get(), &rtvDesc, rtvHandle);
            rtvHandle.Offset(1, m_rtvDescriptorSize);
        }

    }


    // --- �[�x�X�e���V���o�b�t�@�� DSV �q�[�v�쐬 ---
    {
        // DSV�p�f�B�X�N���v�^�q�[�v�쐬
        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
        dsvHeapDesc.NumDescriptors = 1;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap));

        // �[�x�X�e���V���o�b�t�@�̃��\�[�X�쐬
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

        // DSV�쐬
        m_device->CreateDepthStencilView(
            m_depthStencilBuffer.Get(),
            nullptr,
            m_dsvHeap->GetCPUDescriptorHandleForHeapStart()
        );

    }

    {
        // SRV �p�f�B�X�N���v�^�q�[�v�쐬
        D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
        srvHeapDesc.NumDescriptors = 1024; // �K�v�ɉ�����
        srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        ThrowIfFailed(m_device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap)));

        // SRV �̃T�C�Y�擾�iSRV/UAV/CBV�͓����T�C�Y�j
        m_srvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    // --- �R�}���h�A���P�[�^ & �R�}���h���X�g ---
    {
        ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));

        ThrowIfFailed(m_device->CreateCommandList(
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            m_commandAllocator.Get(),
            nullptr,
            IID_PPV_ARGS(&m_commandList)));
        m_commandList->Close(); // ������Ԃŕ��Ă���

        // --- �t�F���X�ƃC�x���g ---
        ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
        m_fenceValue = 1;

        m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (!m_fenceEvent)
            throw std::runtime_error("Failed to create fence event handle.");

        // --- �ꎞ�A�b�v���[�h�o�b�t�@�z�񏉊��� ---
        m_tempUploadBuffers.resize(FrameCount);
        m_fenceValues.resize(FrameCount, 0); // �t�F���X�l�̏�����

    }
}


void Renderer::RenderBegin()
{
    SignalAndWait(m_currentFrameIndex);

    // �R�}���h�A���P�[�^�ƃ��X�g�����Z�b�g
    if (FAILED(m_commandAllocator->Reset())) {
        throw std::runtime_error("Failed to reset command allocator");
    }
    if (FAILED(m_commandList->Reset(m_commandAllocator.Get(), m_pipelineState.Get()))) {
        throw std::runtime_error("Failed to reset command list");
    }

    // Present �� RenderTarget �ɑJ��
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_renderTargets[m_currentFrameIndex].Get(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET
    );
    m_commandList->ResourceBarrier(1, &barrier);

    // �r���[�|�[�g / �V�U�[
    D3D12_VIEWPORT viewport = { 0.0f, 0.0f, (FLOAT)m_width, (FLOAT)m_height, 0.0f, 1.0f };
    D3D12_RECT scissorRect = { 0, 0, (LONG)m_width, (LONG)m_height };
    m_commandList->RSSetViewports(1, &viewport);
    m_commandList->RSSetScissorRects(1, &scissorRect);

    // RTV �� DSV �n���h��
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
        m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
        m_currentFrameIndex,
        m_rtvDescriptorSize
    );
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(
        m_dsvHeap->GetCPUDescriptorHandleForHeapStart()
    );

    // �o�̓^�[�Q�b�g�� RTV �� DSV ���Z�b�g
    m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

    // �f�v�X�o�b�t�@�N���A
    m_commandList->ClearDepthStencilView(
        dsvHandle,
        D3D12_CLEAR_FLAG_DEPTH,
        1.0f,
        0,
        0, nullptr
    );

    // ��ʃN���A
    const float clearColor[] = { 0.2f, 0.2f, 0.4f, 1.0f };
    m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
}



void Renderer::RenderEnd()
{
    // �o���A: RenderTarget -> Present
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_renderTargets[m_currentFrameIndex].Get(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT);
    m_commandList->ResourceBarrier(1, &barrier);

    // �R�}���h���X�g����Ď��s
    m_commandList->Close();
    ID3D12CommandList* lists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(lists), lists);

    // Present �O�Ƀt�F���X��ł�
    m_fenceValue++;
    m_commandQueue->Signal(m_fence.Get(), m_fenceValue);

    // ���̃t���[���X���b�g�ɑΉ�����t�F���X�l��ۑ�
    m_fenceValues[m_currentFrameIndex] = m_fenceValue;

    // Present
    m_swapChain->Present(1, 0);

    // ���̃o�b�N�o�b�t�@�C���f�b�N�X���擾
    // �����Ŏ擾���� m_frameIndex �͎��� RenderBegin �Ŏg����C���f�b�N�X
    m_currentFrameIndex = m_swapChain->GetCurrentBackBufferIndex();

    // ���̃X���b�g�ɕR�Â����ȑO�̃A�b�v���[�h�o�b�t�@�����S�ɉ���ł��邩�m�F
    UINT64 fenceForSlot = m_fenceValues[m_currentFrameIndex];
    if (m_fence->GetCompletedValue() >= fenceForSlot) {
        // GPU �͂��̃X���b�g�̑S�R�}���h���I���Ă��� -> ���
        m_tempUploadBuffers[m_currentFrameIndex].clear();
    }
}


void Renderer::DrawMesh(
    const ModelComponent& modelComp,
    const TransformComponent& transform,
    const CameraComponent& camera,
    AssetManager& assetManager)
{

    const ModelDataContainer* modelData = assetManager.GetModel(modelComp.GetModelId());
    if (!modelData) return;

    ID3D12DescriptorHeap* heaps[] = { GetSrvHeap() };
    m_commandList->SetDescriptorHeaps(_countof(heaps), heaps);

    for (const auto& meshPtr : modelData->GetMeshes())
    {
        const MeshComponent& mesh = *meshPtr;
        const MaterialComponent* material = assetManager.GetMaterial(mesh.GetMaterialId());
        if (!material) continue;

        m_commandList->SetPipelineState(PipelineManager::GetInstance().GetPipelineState(material->GetPipelineType()));
        m_commandList->SetGraphicsRootSignature(PipelineManager::GetInstance().GetRootSignature(material->GetRootSignatureType()));

        // Root CBV
        m_commandList->SetGraphicsRootConstantBufferView(0, camera.GetViewCB()->GetGPUVirtualAddress());
        m_commandList->SetGraphicsRootConstantBufferView(1, camera.GetProjCB()->GetGPUVirtualAddress());
        m_commandList->SetGraphicsRootConstantBufferView(2, transform.GetWorldCB()->GetGPUVirtualAddress());
        m_commandList->SetGraphicsRootConstantBufferView(3, material->GetColorCB()->GetGPUVirtualAddress());


        // VB/IB
        m_commandList->IASetVertexBuffers(0, 1, &mesh.GetVertexBufferView());
        m_commandList->IASetIndexBuffer(&mesh.GetIndexBufferView());
        m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        // GPU�n���h����n��
        if (auto texRes = assetManager.GetTexture(material->GetAlbedoTextureId()))
            m_commandList->SetGraphicsRootDescriptorTable(4, texRes->srv); // texRes->srv �� D3D12_GPU_DESCRIPTOR_HANDLE

        // Draw
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

    // Default buffer �����
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

    // Upload buffer �����
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

    // �f�[�^���}�b�v���ăR�s�[
    void* mapped = nullptr;
    CD3DX12_RANGE readRange(0, 0);
    if (FAILED(uploadBuffer->Map(0, &readRange, &mapped))) {
        throw std::runtime_error("Failed to map upload buffer");
    }
    memcpy(mapped, initData, static_cast<size_t>(byteSize));
    uploadBuffer->Unmap(0, nullptr);

    // ���\�[�X�쐬�p�̈ꎞ�R�}���h�A���P�[�^ / ���X�g�����
    ComPtr<ID3D12CommandAllocator> tempAllocator;
    ComPtr<ID3D12GraphicsCommandList> tempCmdList;

    if (FAILED(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&tempAllocator)))) {
        throw std::runtime_error("Failed to create temp command allocator");
    }
    if (FAILED(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, tempAllocator.Get(), nullptr, IID_PPV_ARGS(&tempCmdList)))) {
        throw std::runtime_error("Failed to create temp command list");
    }

    // CopyBufferRegion �œ]��
    tempCmdList->CopyBufferRegion(defaultBuffer.Get(), 0, uploadBuffer.Get(), 0, byteSize);

    // �R�s�[��ɖړI�̏�Ԃ֑J�ځi�p�r�ɍ��킹�ĕύX�j
    auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        defaultBuffer.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        initialState // �K�v�ɉ����� INDEX_BUFFER ���ɕύX
    );
    tempCmdList->ResourceBarrier(1, &barrier);

    // ���s���� GPU ������҂i���[�J���ŃV�O�i�����҂��j
    ExecuteCommandListAndWait(m_commandQueue.Get(), tempCmdList.Get(), m_fence.Get(), m_fenceValue);


    m_tempUploadBuffers[m_currentFrameIndex].push_back(uploadBuffer);

    m_fenceValues[m_currentFrameIndex] = m_fenceValue;

    return  defaultBuffer;
}


void Renderer::ExecuteCommandListAndWait(ID3D12CommandQueue* commandQueue,
    ID3D12GraphicsCommandList* commandList,
    ID3D12Fence* fence,
    UINT64& fenceValue)
{
    // �R�}���h���X�g�����
    commandList->Close();

    // �R�}���h�L���[�Ɏ��s
    ID3D12CommandList* lists[] = { commandList };
    commandQueue->ExecuteCommandLists(_countof(lists), lists);

    // GPU ������ҋ@
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


