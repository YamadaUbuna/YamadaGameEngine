#pragma once
#include "include/ISystem.h"
#include "include/MeshComponent.h"
#include "include/MaterialComponent.h"
#include "include/TransformComponent.h"
#include "include/ModelComponent.h"
#include "include/CameraComponent.h"

#include <DirectXMath.h>

// RenderSystemはISystemを実装し、シーン内のレンダリング処理を担当します。
// エンティティのモデル、マテリアル、トランスフォームコンポーネントを取得し、
// それらの定数バッファを作成・更新したうえでRendererに描画を依頼します。
// Direct3D12用の定数バッファ作成と更新のヘルパー関数も含みます。

class RenderSystem :
    public ISystem
{
public:
    void Initialize(IScene& scene) override;
    void Update(IScene& scene, float deltaTime) override;

private:
    HRESULT CreateAndUpdateCB(Microsoft::WRL::ComPtr<ID3D12Resource>& cb, const void* data, size_t size);
    HRESULT UpdateTransformCBs(TransformComponent* transform, CameraComponent* camera);
    HRESULT UpdateMaterialCB(MaterialComponent* material);
};

