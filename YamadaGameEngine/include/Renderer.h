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

// DirectX 12を用いたレンダラーのシングルトン管理クラス。
// ウィンドウとの連携やデバイス初期化、描画コマンドの発行を行う。
// メッシュやマテリアル、カメラ情報を使ってシーンの描画を実装。
// GPU同期やリソース管理、コマンドリスト操作も担当する。

using namespace Microsoft::WRL;

class Renderer {
public:
    // シングルトンインスタンス取得
    static Renderer& GetInstance();

    // DirectX12の初期化（ウィンドウハンドルとサイズ指定）
    void Initialize(HWND hwnd, int width, int height);

    // 描画開始処理：コマンドリスト準備、RTVクリア、バリア遷移（PRESENT→RENDER_TARGET）
    void RenderBegin();

    // 描画終了処理：コマンドリストクローズ、実行、Present、GPUフェンス待ち
    void RenderEnd();

    // リソース解放処理（フェンスやRTVなど）
    void Terminate();

    // メッシュ描画（メッシュ・マテリアル・トランスフォーム・カメラ情報）
    void DrawMesh(const ModelComponent& modelComp,
        const TransformComponent& transform,
        const CameraComponent& camera,
        AssetManager& assetManager);

    // DirectXデバイスの取得
    ID3D12Device* GetDevice() const { return m_device.Get(); }

    // コマンドリストの取得
    ID3D12GraphicsCommandList* GetCommandList() const { return m_commandList.Get(); }

    // コマンドキューの取得
    ID3D12CommandQueue* GetCommandQueue() const { return m_commandQueue.Get(); }

    // 初期データを渡してGPU用のデフォルトバッファ作成
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(const void* initData, UINT64 byteSize, D3D12_RESOURCE_STATES initialState);

    // 対応するFeatureLevel（12_1推奨）
    D3D_FEATURE_LEVEL  m_featureLevel = D3D_FEATURE_LEVEL_12_1;

    // ウィンドウサイズ取得
    int GetWindowWidth() const { return m_width; }
    int GetWindowHeight() const { return m_height; }

	ID3D12DescriptorHeap* GetSrvHeap() const { return m_srvHeap.Get(); }
    UINT GetSrvDescriptorSize() const { return m_srvDescriptorSize; }
    UINT AllocateSrvIndex() {
        return m_nextSrvIndex++;
    }

    // フェンス信号送信と待機
    void SignalAndWait(UINT frameIndex);

private:
    Renderer() = default;
    ~Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    // DirectX12デバイス関連
    ComPtr<ID3D12Device> m_device;
    ComPtr<IDXGISwapChain3> m_swapChain;
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;
    

    // レンダーターゲット（ダブルバッファリング）
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

    // GPU同期用フェンス関連
    ComPtr<ID3D12Fence> m_fence;
    UINT64 m_fenceValue = 0;
    HANDLE m_fenceEvent = nullptr;
    std::vector<UINT64> m_fenceValues; // 各フレームのフェンス値管理（サイズはFrameCount）

    // パイプラインステートオブジェクト（PSO）とルートシグネチャ（パイプライン設定）
    ComPtr<ID3D12PipelineState> m_pipelineState;
    ComPtr<ID3D12RootSignature> m_rootSignature;

    // 一時アップロード用バッファ（フレームごとに複数確保）
    std::vector<std::vector<ComPtr<ID3D12Resource>>> m_tempUploadBuffers;

    // ウィンドウ情報（ハンドルとサイズ）
    HWND m_hwnd = nullptr;
    int m_width = 0;
    int m_height = 0;

    // コマンドリストを実行してGPU完了まで待機
    void ExecuteCommandListAndWait(
        ID3D12CommandQueue* commandQueue,
        ID3D12GraphicsCommandList* commandList,
        ID3D12Fence* fence,
        UINT64& fenceValue);

    // 指定フレームのGPU処理完了まで待機
    void WaitForGpu(UINT frameIndex);
};

