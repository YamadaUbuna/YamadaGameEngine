#include "include\pch.h"
#include "include/RenderSystem.h"
#include <stdexcept>
#include "include/Renderer.h"

//ここに関数作って、MeshComponentの情報を綺麗にする。


void RenderSystem::Initialize(IScene& scene)  
{  
    // 初期化処理が必要な場合はここに記述  
    const std::vector<std::unique_ptr<IEntity>>& entities = scene.GetEntities();  
    for (const std::unique_ptr<IEntity>& entity : entities) {  
        const ModelComponent* modelComponent = entity->GetComponent<ModelComponent>();
        const MaterialComponent* materialComponent = entity->GetComponent<MaterialComponent>();
        const TransformComponent* transformComponent = entity->GetComponent<TransformComponent>();

        if (!modelComponent || !materialComponent || !transformComponent) {  
            // 必要なコンポーネントがない場合はスキップ  
            continue;  
        }  
    }  
}

void RenderSystem::Update(IScene& scene, float deltaTime)
{
    const std::vector<std::unique_ptr<IEntity>>& entities = scene.GetEntities();

    for (const std::unique_ptr<IEntity>& entity : entities) {
        ModelComponent* model = entity->GetComponent<ModelComponent>();
        TransformComponent* transform = entity->GetComponent<TransformComponent>();
		CameraComponent* camera = scene.GetMainCameraEntity()->GetComponent<CameraComponent>();

        if (!model  || !transform || !camera) {
            // 描画に必要な情報が揃っていないのでスキップ
            continue;
        }
        UpdateConstantBuffers(transform, camera);
        Renderer::GetInstance().DrawMesh(*model, *transform, *camera,scene.GetAssetManager());

        //RenderComponent* renderComponent = entity->GetComponent<RenderComponent>();
        //if (renderComponent) {
        //    // 仮の描画（実際はDirectX等で描画）
        //    
        //    char buffer[128];
        //    sprintf_s(buffer, "Draw Entity with color RGBA(%f, %f, %f, %f)\n",
        //        renderComponent->r, renderComponent->g, renderComponent->b, renderComponent->a);
        //    OutputDebugStringA(buffer);
        //}
    }
}


inline UINT AlignTo256(UINT size) {
    return (size + 255u) & ~255u;
}

HRESULT RenderSystem::UpdateConstantBuffers(TransformComponent* transform, CameraComponent* camera)
{
    if (!transform || !camera) return E_POINTER;

    auto CreateAndUpdateCB = [&](Microsoft::WRL::ComPtr<ID3D12Resource>& cb, const void* data, size_t size) -> HRESULT
        {
            if (!cb)
            {
                CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
                CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(AlignTo256(size));

                HRESULT hr = Renderer::GetInstance().GetDevice()->CreateCommittedResource(
                    &heapProps,
                    D3D12_HEAP_FLAG_NONE,
                    &bufferDesc,
                    D3D12_RESOURCE_STATE_GENERIC_READ,
                    nullptr,
                    IID_PPV_ARGS(&cb)
                );
                if (FAILED(hr)) return hr;
            }

            void* mappedPtr = nullptr;
            D3D12_RANGE readRange{ 0,0 };
            HRESULT hr = cb->Map(0, &readRange, &mappedPtr);
            if (FAILED(hr)) return hr;

            memcpy(mappedPtr, data, size);

            // Mapは保持するのでUnmap不要
            return S_OK;
        };

    DirectX::XMMATRIX world = XMMatrixTranspose(transform->GetWorldMatrix());
    DirectX::XMMATRIX view = XMMatrixTranspose(camera->GetViewMatrix());
    DirectX::XMMATRIX proj = XMMatrixTranspose(camera->GetProjMatrix());

    HRESULT hr = S_OK;

    // TransformComponent に持たせた World CB を更新
    hr = CreateAndUpdateCB(transform->SetWorldCB(), &world, sizeof(world));
    if (FAILED(hr)) return hr;

    // カメラは Renderer 側に持たせている CB を更新
    hr = CreateAndUpdateCB(camera->SetViewCB(), &view, sizeof(view));
    if (FAILED(hr)) return hr;

    hr = CreateAndUpdateCB(camera->SetProjCB(), &proj, sizeof(proj));
    if (FAILED(hr)) return hr;

    return S_OK;
}




