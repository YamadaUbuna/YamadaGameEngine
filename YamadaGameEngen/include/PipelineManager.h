#pragma once
#include <unordered_map>
#include <wrl/client.h>
#include <d3d12.h>

enum class RootSignatureType
{
    def,
};

enum class PipelineType {
	test, // テスト用のパイプライン
};

class PipelineManager
{
public:

    static PipelineManager& GetInstance();

    void Initialize(); // ここでPSOとRootSignatureを作る

    ID3D12PipelineState* GetPipelineState(PipelineType type);
    ID3D12RootSignature* GetRootSignature(RootSignatureType type);

private:

    Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateRootSignature(RootSignatureType type);
    Microsoft::WRL::ComPtr<ID3D12PipelineState> CreatePipelineState(PipelineType Ptype, RootSignatureType RType);

    std::unordered_map<PipelineType, Microsoft::WRL::ComPtr<ID3D12PipelineState>> m_pipelineStates;
    std::unordered_map<RootSignatureType, Microsoft::WRL::ComPtr<ID3D12RootSignature>> m_rootSignatures;

};

