#include "include\pch.h"
#include "include/PipelineManager.h"
#include "include/Renderer.h"
#include "include/LoadCSO.h"

PipelineManager& PipelineManager::GetInstance() {
    static PipelineManager instance;
    return instance;
}

void PipelineManager::Initialize()
{
    CreateRootSignature();
    //CreateTestPipelineState(RootSignatureType::def);
	CreateFbxPipelineState(RootSignatureType::def);
}

ID3D12PipelineState* PipelineManager::GetPipelineState(PipelineType type)
{
    auto it = m_pipelineStates.find(type);
    if (it != m_pipelineStates.end()) {
        return it->second.Get();
    }
    return nullptr;
}

ID3D12RootSignature* PipelineManager::GetRootSignature(RootSignatureType type)
{
    auto it = m_rootSignatures.find(type);
    if (it != m_rootSignatures.end()) {
        return it->second.Get();
    }
    return nullptr;
}


Microsoft::WRL::ComPtr<ID3D12RootSignature> PipelineManager::CreateRootSignature()
{
    // t0: Diffuse テクスチャ
    CD3DX12_DESCRIPTOR_RANGE srvRange;
    srvRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

    CD3DX12_ROOT_PARAMETER rootParams[4];
    rootParams[0].InitAsConstantBufferView(0); // b0: View
    rootParams[1].InitAsConstantBufferView(1); // b1: Projection
    rootParams[2].InitAsConstantBufferView(2); // b2: World
    rootParams[3].InitAsDescriptorTable(1, &srvRange, D3D12_SHADER_VISIBILITY_PIXEL); // t0: Diffuse

    // Static Sampler
    D3D12_STATIC_SAMPLER_DESC sampler = {};
    sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    sampler.AddressU = sampler.AddressV = sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    sampler.ShaderRegister = 0; // s0
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.NumParameters = _countof(rootParams);
    rootSigDesc.pParameters = rootParams;
    rootSigDesc.NumStaticSamplers = 1;
    rootSigDesc.pStaticSamplers = &sampler;
    rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;
    HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
        &signatureBlob, &errorBlob);
    if (FAILED(hr)) {
        if (errorBlob)
            OutputDebugStringA(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
        return nullptr;
    }

    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
    hr = Renderer::GetInstance().GetDevice()->CreateRootSignature(
        0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
        IID_PPV_ARGS(&rootSignature));
    if (FAILED(hr)) {
        OutputDebugString(L"[Error] CreateRootSignature failed.\n");
        return nullptr;
    }

    m_rootSignatures[RootSignatureType::def] = rootSignature;
    return rootSignature;
}



Microsoft::WRL::ComPtr<ID3D12PipelineState> PipelineManager::CreateTestPipelineState(RootSignatureType RType)
{
    // 必要なRootSignatureを取得
    auto rootSig = GetRootSignature(RType);
    if (!rootSig) {
        OutputDebugString(L"[Error] RootSignature is nullptr.\n");
        return nullptr;
    }

    // --- シェーダ読み込み ---
    auto vsBlob = LoadCSO(L"Resource/TestVertexShader.cso");
    if (!vsBlob) {
        OutputDebugString(L"[Error] Failed to load vertex shader.\n");
        return nullptr;
    }

    auto psBlob = LoadCSO(L"Resource/TestPixelShader.cso");
    if (!psBlob) {
        OutputDebugString(L"[Error] Failed to load pixel shader.\n");
        return nullptr;
    }

    // --- 入力レイアウト ---
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    // --- PSO設定 ---
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputLayout, _countof(inputLayout) };
    psoDesc.pRootSignature = rootSig;
    psoDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
    psoDesc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

    Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
    HRESULT hr = Renderer::GetInstance().GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso));
    if (FAILED(hr)) {
        OutputDebugString(L"[Error] CreateGraphicsPipelineState failed.\n");
        return nullptr;
    }

    m_pipelineStates[PipelineType::test] = pso;

    return pso;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> PipelineManager::CreateFbxPipelineState(RootSignatureType RType)
{
    // 必要なRootSignatureを取得
    auto rootSig = GetRootSignature(RType);
    if (!rootSig) {
        OutputDebugString(L"[Error] RootSignature is nullptr.\n");
        return nullptr;
    }

    // --- シェーダ読み込み ---
    auto vsBlob = LoadCSO(L"Resource/TestVertexShader.cso");
    if (!vsBlob) {
        OutputDebugString(L"[Error] Failed to load vertex shader.\n");
        return nullptr;
    }

    auto psBlob = LoadCSO(L"Resource/TestPixelShader.cso");
    if (!psBlob) {
        OutputDebugString(L"[Error] Failed to load pixel shader.\n");
        return nullptr;
    }

    // --- 入力レイアウト ---
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24,
          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },

        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32,
          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };

    // --- PSO設定 ---
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    psoDesc.InputLayout = { inputLayout, _countof(inputLayout) };
    psoDesc.pRootSignature = rootSig;
    psoDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
    psoDesc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);


    D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    depthStencilDesc.StencilEnable = FALSE;

    psoDesc.DepthStencilState = depthStencilDesc;
    psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;


    Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;
    HRESULT hr = Renderer::GetInstance().GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso));
    if (FAILED(hr)) {
        OutputDebugString(L"[Error] CreateGraphicsPipelineState failed.\n");
        return nullptr;
    }

    m_pipelineStates[PipelineType::fbx] = pso;

    return pso;
}

