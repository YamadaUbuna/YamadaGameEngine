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

    // AssetManager は参照で取得
    AssetManager& assetManager = scene.GetAssetManager();

    for (const std::unique_ptr<IEntity>& entity : entities) {
        ModelComponent* model = entity->GetComponent<ModelComponent>();
        TransformComponent* transform = entity->GetComponent<TransformComponent>();
        CameraComponent* camera = scene.GetMainCameraEntity()->GetComponent<CameraComponent>();

        if (!model || !transform || !camera) continue;

        ModelDataContainer* modelData = assetManager.GetModelForUpdate(model->GetModelId());
        if (!modelData) continue;

        for (auto& meshPtr : modelData->GetMeshes())
        {
            MeshComponent& mesh = *meshPtr;
            MaterialComponent* material = assetManager.GetMaterialForUpdate(mesh.GetMaterialId());
            if (material) {
                UpdateMaterialCB(material);
            }
        }

        UpdateTransformCBs(transform, camera);
        Renderer::GetInstance().DrawMesh(*model, *transform, *camera, assetManager);
    }
}

inline UINT AlignTo256(UINT size) {
    return (size + 255u) & ~255u;
}

HRESULT RenderSystem::CreateAndUpdateCB(Microsoft::WRL::ComPtr<ID3D12Resource>& cb, const void* data, size_t size)
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
    return S_OK;
}

HRESULT RenderSystem::UpdateTransformCBs(TransformComponent* transform, CameraComponent* camera)
{
    if (!transform || !camera) return E_POINTER;

    HRESULT hr = S_OK;

    DirectX::XMMATRIX world = XMMatrixTranspose(transform->GetWorldMatrix());
    hr = CreateAndUpdateCB(transform->SetWorldCB(), &world, sizeof(world));
    if (FAILED(hr)) return hr;

    DirectX::XMMATRIX view = XMMatrixTranspose(camera->GetViewMatrix());
    hr = CreateAndUpdateCB(camera->SetViewCB(), &view, sizeof(view));
    if (FAILED(hr)) return hr;

    DirectX::XMMATRIX proj = XMMatrixTranspose(camera->GetProjMatrix());
    hr = CreateAndUpdateCB(camera->SetProjCB(), &proj, sizeof(proj));
    if (FAILED(hr)) return hr;

    return S_OK;
}

HRESULT RenderSystem::UpdateMaterialCB(MaterialComponent* material)
{
    if (!material) return E_POINTER;

    DirectX::XMFLOAT4 color = material->GetBaseColor();
    return CreateAndUpdateCB(material->SetColorCB(), &color, sizeof(color));
}





