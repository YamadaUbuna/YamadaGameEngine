// Renderer.h
#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <string>
#include <vector>

#include "include/MeshComponent.h"
#include "include/MaterialComponent.h"
#include "include/CameraComponent.h"
#include "include/TransformComponent.h"

class Renderer {
public:
    static Renderer& GetInstance();

    void Initialize(HWND hwnd, int width, int height);//DX12の初期化全般
    void RenderBegin();//	コマンドリストの準備・RTVクリア・バリア遷移（PRESENT→RENDER_TARGET）
    void RenderEnd(); // コマンドリストClose → 実行 → Present & フェンス待ち
    void Terminate(); //リソース解放（フェンス、RTなど）

    void DrawMesh(const MeshComponent& mesh, const MaterialComponent& material,const TransformComponent&);
    //void SetPipeline(Material* material);
    void SetCamera(const CameraComponent& camera);

	ID3D12Device* GetDevice() const { return m_device.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() const { return m_commandList.Get(); }

    struct ConstantBufferData
    {
        DirectX::XMMATRIX world;
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
    };

private:
    Renderer() = default;
    ~Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    // DirectX core
    Microsoft::WRL::ComPtr<ID3D12Device> m_device;
    Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;

    // Render target
    static const UINT FrameCount = 2;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    UINT m_rtvDescriptorSize = 0;
    UINT m_frameIndex = 0;

    // Fence
    Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
    UINT64 m_fenceValue = 0;
    HANDLE m_fenceEvent = nullptr;

    // パイプラインステートオブジェクトとルートシグネチャ（描画パイプラインの構成）
    Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

    // State
    HWND m_hwnd = nullptr;
    int m_width = 0;
    int m_height = 0;

    // 内部処理
    void WaitForPreviousFrame();//フェンス同期
    void CreateRenderTargetViews();
    void CreateRootSignature();
    void CreatePipelineState();

    D3D12_GPU_VIRTUAL_ADDRESS InitializeConstantBufferData(TransformComponent* transform, CameraComponent* camera);

};
