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
    //    return aspectRatioOverride.value_or(0.0f); // 0.0f �𖢐ݒ�̃T�C���Ƃ���
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

    DirectX::XMFLOAT3 position;       // �J�����̈ʒu
    DirectX::XMFLOAT3 target;         // ���Ă�������i�^�[�Q�b�g�j
    DirectX::XMFLOAT3 up;             // ������x�N�g��

    //float fovY;                       // �c��������p�i���W�A���j
    //std::optional<float> aspectRatioOverride;               // �A�X�y�N�g��i�� / �����j
    //float nearZ;                     // �j�A�N���b�v��
    //float farZ;                      // �t�@�[�N���b�v��

    //// ���ʂƂ��Ďg���s��i���t���[���X�V�j
    //DirectX::XMMATRIX viewMatrix;
    //DirectX::XMMATRIX projMatrix;
};