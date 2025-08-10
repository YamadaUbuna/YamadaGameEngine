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
    CreateRootSignature(RootSignatureType::def);
    m_pipelineStates[PipelineType::test] =
        CreatePipelineState(PipelineType::test, RootSignatureType::def);
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


Microsoft::WRL::ComPtr<ID3D12RootSignature> PipelineManager::CreateRootSignature(RootSignatureType type)
{
    D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
    rootSigDesc.NumParameters = 0;
    rootSigDesc.pParameters = nullptr;
    rootSigDesc.NumStaticSamplers = 0;
    rootSigDesc.pStaticSamplers = nullptr;
    rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

    HRESULT hr = D3D12SerializeRootSignature(
        &rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
        &signatureBlob, &errorBlob
    );
    if (FAILED(hr)) {
        // エラー処理（エラーBlobの出力など）
    }

    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
    Renderer::GetInstance().GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
        IID_PPV_ARGS(&rootSignature));

    m_rootSignatures[type] = rootSignature;

    return rootSignature;
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> PipelineManager::CreatePipelineState(PipelineType Ptype, RootSignatureType RType)
{
    // 必要なRootSignatureを取得
    auto rootSig = GetRootSignature(RType);

    // --- シェーダ読み込み ---
    auto vsBlob = LoadCSO(L"Resource/TestVertexShader.cso");
    auto psBlob = LoadCSO(L"Resource/TestPixelShader.cso");

    // --- 入力レイアウト（例） ---
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
          D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
          // 必要なら TEXCOORD なども追加
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
    Renderer::GetInstance().GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pso));

	m_pipelineStates[Ptype] = pso;

    return pso;
}
