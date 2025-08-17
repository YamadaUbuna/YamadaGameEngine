#include "include/pch.h"
#include "include/TransformComponent.h"

#include <cmath>

namespace
{

    void XMQuaternionToEulerAngles(FXMVECTOR q, float* pitch, float* yaw, float* roll)
    {
        XMFLOAT4 qf;
        XMStoreFloat4(&qf, q);

        // Calculate the angular radians of each axis
        float ysqr = qf.y * qf.y;

        // roll (X)
        float t0 = +2.0f * (qf.w * qf.x + qf.y * qf.z);
        float t1 = +1.0f - 2.0f * (qf.x * qf.x + ysqr);
        *roll = std::atan2(t0, t1);

        // pitch (Y)
        float t2 = +2.0f * (qf.w * qf.y - qf.z * qf.x);
        t2 = t2 > 1.0f ? 1.0f : t2;
        t2 = t2 < -1.0f ? -1.0f : t2;
        *pitch = std::asin(t2);

        // yaw (Z)
        float t3 = +2.0f * (qf.w * qf.z + qf.x * qf.y);
        float t4 = +1.0f - 2.0f * (ysqr + qf.z * qf.z);
        *yaw = std::atan2(t3, t4);
    }

}


void TransformComponent::SetRotation(const XMFLOAT3& rot)
{
    rotation = rot;

    // �I�C���[�p�i�x�j�����W�A��
    float pitch = XMConvertToRadians(rot.x);
    float yaw = XMConvertToRadians(rot.y);
    float roll = XMConvertToRadians(rot.z);

    XMVECTOR q = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
    XMStoreFloat4(&quatRotation, q);
}

void TransformComponent::SetQuaternionRotation(const XMVECTOR& q)
{
    XMStoreFloat4(&quatRotation, q);

    // forward/up/right ���X�V
    XMStoreFloat3(&forward, XMVector3Rotate(XMVectorSet(0, 0, 1, 0), q));
    XMStoreFloat3(&up, XMVector3Rotate(XMVectorSet(0, 1, 0, 0), q));
    XMStoreFloat3(&right, XMVector3Rotate(XMVectorSet(1, 0, 0, 0), q));

    // �I�C���[�p���X�V
    float pitch, yaw, roll;
    XMQuaternionToEulerAngles(q, &pitch, &yaw, &roll);
    rotation.x = XMConvertToDegrees(pitch);
    rotation.y = XMConvertToDegrees(yaw);
    rotation.z = XMConvertToDegrees(roll);

}


void TransformComponent::Translate(const XMFLOAT3& translation)
{
	position.x += translation.x;
	position.y += translation.y;
	position.z += translation.z;
}

void TransformComponent::Rotate(const XMFLOAT3& deltaRotation)
{
    // ���͂��N�H�[�^�j�I���ɕϊ�
    float pitch = XMConvertToRadians(deltaRotation.x);
    float yaw = XMConvertToRadians(deltaRotation.y);
    float roll = XMConvertToRadians(deltaRotation.z);
    XMVECTOR deltaQuat = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);

    // ���݂̉�]�N�H�[�^�j�I����ǂݍ���
    XMVECTOR currentQuat = XMLoadFloat4(&quatRotation);

    // ��]�̍���
    XMVECTOR newQuat = XMQuaternionMultiply(deltaQuat, currentQuat);

    XMStoreFloat4(&quatRotation, newQuat);

    // �N�H�[�^�j�I�����I�C���[�p�ɕϊ����čX�V
    float outPitch, outYaw, outRoll;
    XMQuaternionToEulerAngles(newQuat, &outPitch, &outYaw, &outRoll);
    rotation.x = XMConvertToDegrees(outPitch);
    rotation.y = XMConvertToDegrees(outYaw);
    rotation.z = XMConvertToDegrees(outRoll);
}

void TransformComponent::LookAt(const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up)
{
    using namespace DirectX;

    XMVECTOR posVec = XMLoadFloat3(&position);
    XMVECTOR targetVec = XMLoadFloat3(&target);
    XMVECTOR upVec = XMLoadFloat3(&up);

    // forward �x�N�g�����v�Z
    XMVECTOR forward = XMVector3Normalize(targetVec - posVec);

    // LookAt ����N�H�[�^�j�I�������
    XMVECTOR right = XMVector3Normalize(XMVector3Cross(upVec, forward));
    XMVECTOR camUp = XMVector3Cross(forward, right);

    XMMATRIX rotMat;
    rotMat.r[0] = right;
    rotMat.r[1] = camUp;
    rotMat.r[2] = forward;
    rotMat.r[3] = XMVectorSet(0, 0, 0, 1);

    XMVECTOR quat = XMQuaternionRotationMatrix(rotMat);

    // TransformComponent �ɃN�H�[�^�j�I���Ƃ��ĕۑ�
    XMStoreFloat4(&quatRotation, quat);
}


void TransformComponent::UpdateAxesAndWorldMatrix()
{
    // �N�H�[�^�j�I�������]�s������
    XMMATRIX rotMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&quatRotation));

    // �X�P�[���s��
    XMMATRIX scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
    // ���s�ړ��s��
    XMMATRIX transMatrix = XMMatrixTranslation(position.x, position.y, position.z);

    // ���[���h�s��X�V
    worldMatrix = scaleMatrix * rotMatrix * transMatrix;

    // forward, up, right ���v�Z�i���[���h��Ԋ�Łj
    XMVECTOR fwd = XMVector3TransformNormal(XMVectorSet(0, 0, 1, 0), rotMatrix);
    XMVECTOR upVec = XMVector3TransformNormal(XMVectorSet(0, 1, 0, 0), rotMatrix);
    XMVECTOR rightVec = XMVector3TransformNormal(XMVectorSet(1, 0, 0, 0), rotMatrix);

    XMStoreFloat3(&forward, fwd);
    XMStoreFloat3(&up, upVec);
    XMStoreFloat3(&right, rightVec);
}


