#pragma once
#include <optional>
#include "include/IComponent.h"

class CameraComponent : public IComponent
{
public:
	CameraComponent() = default;
    ~CameraComponent();


    const DirectX::XMFLOAT3& GetPosition() const { return position; }
    void SetPosition(const DirectX::XMFLOAT3& pos) { position = pos;}

    const DirectX::XMFLOAT3& GetTarget() const { return target; }
    void SetTarget(const DirectX::XMFLOAT3& tgt) { target = tgt; }

    const DirectX::XMFLOAT3& GetUp() const { return up; }
    void SetUp(const DirectX::XMFLOAT3& u) { up = u; }

    //float GetFovY() const { return fovY; }
    //void SetFovY(float fov) { fovY = fov; }

    //float GetAspectRatio() const {
    //    return aspectRatioOverride.value_or(0.0f); // 0.0f を未設定のサインとする
    //}
    //bool HasAspectRatioOverride() const {
    //    return aspectRatioOverride.has_value();
    //}
    //void SetAspectRatio(float ratio) { aspectRatioOverride = ratio; }

    //float GetNearZ() const { return nearZ; }
    //void SetNearZ(float z) { nearZ = z; }

    //float GetFarZ() const { return farZ; }
    //void SetFarZ(float z) { farZ = z; }

    //DirectX::XMMATRIX GetViewMatrix() const { return viewMatrix; }
    //DirectX::XMMATRIX GetProjMatrix() const { return projMatrix; }


private:

    DirectX::XMFLOAT3 position;       // カメラの位置
    DirectX::XMFLOAT3 target;         // 見ている方向（ターゲット）
    DirectX::XMFLOAT3 up;             // 上方向ベクトル

    //float fovY;                       // 縦方向視野角（ラジアン）
    //std::optional<float> aspectRatioOverride;               // アスペクト比（幅 / 高さ）
    //float nearZ;                     // ニアクリップ面
    //float farZ;                      // ファークリップ面

    //// 結果として使う行列（毎フレーム更新）
    //DirectX::XMMATRIX viewMatrix;
    //DirectX::XMMATRIX projMatrix;
};