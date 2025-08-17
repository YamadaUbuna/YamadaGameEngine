#pragma once
#include "include/ISystem.h"
#include "include/CameraComponent.h"

#include <DirectXMath.h>

// CameraSystemはISystemを継承し、シーン内のCameraComponentを管理・更新するシステムです。
// カメラのビュー行列や投影行列の計算・更新処理を担当します。
// InitializeとUpdateでシーンのカメラ状態を毎フレーム制御します。

class CameraSystem :
    public ISystem
{
public:
    void Initialize(IScene& scene) override;
    void Update(IScene& scene, float deltaTime) override;

private:
    void UpdateMatrices(CameraComponent* cam);
};

