// Renderer.h
#pragma once

#include "include/MeshComponent.h"
#include "include/MaterialComponent.h"
#include "include/CameraComponent.h"
#include "include/TransformComponent.h"
#include "include/ModelComponent.h"

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <string>
#include <vector>
#include <fbxsdk.h>

// DirectX 12��p���������_���[�̃V���O���g���Ǘ��N���X�B
// �E�B���h�E�Ƃ̘A�g��f�o�C�X�������A�`��R�}���h�̔��s���s���B
// ���b�V����}�e���A���A�J���������g���ăV�[���̕`��������B
// GPU�����⃊�\�[�X�Ǘ��A�R�}���h���X�g������S������B

using namespace Microsoft::WRL;

class Renderer {
public:
    // �V���O���g���C���X�^���X�擾
    static Renderer& GetInstance();

    // DirectX12�̏������i�E�B���h�E�n���h���ƃT�C�Y�w��j
    void Initialize(HWND hwnd, int width, int height);

    // �`��J�n�����F�R�}���h���X�g�����ARTV�N���A�A�o���A�J�ځiPRESENT��RENDER_TARGET�j
    void RenderBegin();

    // �`��I�������F�R�}���h���X�g�N���[�Y�A���s�APresent�AGPU�t�F���X�҂�
    void RenderEnd();

    // ���\�[�X��������i�t�F���X��RTV�Ȃǁj
    void Terminate();

    // ���b�V���`��i���b�V���E�}�e���A���E�g�����X�t�H�[���E�J�������j
    void DrawMesh(const ModelComponent& modelComp,
        const TransformComponent& transform,
        const CameraComponent& camera,
        AssetManager& assetManager);

    // DirectX�f�o�C�X�̎擾
    ID3D12Device* GetDevice() const { return m_device.Get(); }

    // �R�}���h���X�g�̎擾
    ID3D12GraphicsCommandList* GetCommandList() const { return m_commandList.Get(); }

    // �R�}���h�L���[�̎擾
    ID3D12CommandQueue* GetCommandQueue() const { return m_commandQueue.Get(); }

    // �����f�[�^��n����GPU�p�̃f�t�H���g�o�b�t�@�쐬
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(const void* initData, UINT64 byteSize, D3D12_RESOURCE_STATES initialState);

    // �Ή�����FeatureLevel�i12_1�����j
    D3D_FEATURE_LEVEL  m_featureLevel = D3D_FEATURE_LEVEL_12_1;

    // �E�B���h�E�T�C�Y�擾
    int GetWindowWidth() const { return m_width; }
    int GetWindowHeight() const { return m_height; }

	ID3D12DescriptorHeap* GetSrvHeap() const { return m_srvHeap.Get(); }
    UINT GetSrvDescriptorSize() const { return m_srvDescriptorSize; }
    UINT AllocateSrvIndex() {
        return m_nextSrvIndex++;
    }

    // �t�F���X�M�����M�Ƒҋ@
    void SignalAndWait(UINT frameIndex);

private:
    Renderer() = default;
    ~Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    // DirectX12�f�o�C�X�֘A
    ComPtr<ID3D12Device> m_device;
    ComPtr<IDXGISwapChain3> m_swapChain;
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;
    

    // �����_�[�^�[�Q�b�g�i�_�u���o�b�t�@�����O�j
    static const UINT FrameCount = 2;
    ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
    ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    UINT m_rtvDescriptorSize = 0;
    UINT m_currentFrameIndex = 0;

    ComPtr<ID3D12Resource> m_depthStencilBuffer;
    D3D12_CPU_DESCRIPTOR_HANDLE m_dsvHandle{};
    ComPtr<ID3D12DescriptorHeap> m_dsvHeap;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_srvHeap;
    UINT m_srvDescriptorSize = 0;
    UINT m_nextSrvIndex = 0;

    // GPU�����p�t�F���X�֘A
    ComPtr<ID3D12Fence> m_fence;
    UINT64 m_fenceValue = 0;
    HANDLE m_fenceEvent = nullptr;
    std::vector<UINT64> m_fenceValues; // �e�t���[���̃t�F���X�l�Ǘ��i�T�C�Y��FrameCount�j

    // �p�C�v���C���X�e�[�g�I�u�W�F�N�g�iPSO�j�ƃ��[�g�V�O�l�`���i�p�C�v���C���ݒ�j
    ComPtr<ID3D12PipelineState> m_pipelineState;
    ComPtr<ID3D12RootSignature> m_rootSignature;

    // �ꎞ�A�b�v���[�h�p�o�b�t�@�i�t���[�����Ƃɕ����m�ہj
    std::vector<std::vector<ComPtr<ID3D12Resource>>> m_tempUploadBuffers;

    // �E�B���h�E���i�n���h���ƃT�C�Y�j
    HWND m_hwnd = nullptr;
    int m_width = 0;
    int m_height = 0;

    // �R�}���h���X�g�����s����GPU�����܂őҋ@
    void ExecuteCommandListAndWait(
        ID3D12CommandQueue* commandQueue,
        ID3D12GraphicsCommandList* commandList,
        ID3D12Fence* fence,
        UINT64& fenceValue);

    // �w��t���[����GPU���������܂őҋ@
    void WaitForGpu(UINT frameIndex);
};

