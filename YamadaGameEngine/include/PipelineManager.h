#pragma once
#include <unordered_map>
#include <wrl/client.h>
#include <d3d12.h>

enum class RootSignatureType
{
    def,
};

enum class PipelineType {
	test, // �e�X�g�p�̃p�C�v���C��
	fbx, // FBX�p�̃p�C�v���C��
};

class PipelineManager
{
public:
	PipelineManager() = default;
	~PipelineManager() = default;

    static PipelineManager& GetInstance();

    void Initialize(); // ������PSO��RootSignature�����

    ID3D12PipelineState* GetPipelineState(PipelineType type);
    ID3D12RootSignature* GetRootSignature(RootSignatureType type);

private:

    Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateRootSignature();
    Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateTestPipelineState(RootSignatureType RType);
    Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateFbxPipelineState(RootSignatureType RType);

    std::unordered_map<PipelineType, Microsoft::WRL::ComPtr<ID3D12PipelineState>> m_pipelineStates;
    std::unordered_map<RootSignatureType, Microsoft::WRL::ComPtr<ID3D12RootSignature>> m_rootSignatures;

};

