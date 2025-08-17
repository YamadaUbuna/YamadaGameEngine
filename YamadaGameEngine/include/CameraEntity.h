#pragma once
#include "include/EntityBase.h"
#include "CameraComponent.h"
#include "TransformComponent.h"

// �J�����p�G���e�B�e�B�N���X�B
// CameraComponent��TransformComponent�������A�����ʒu�ƌ�����ݒ肷��B

class CameraEntity : public EntityBase {
public:

    CameraEntity() {
		AddComponent(std::make_unique<CameraComponent>());
		AddComponent(std::make_unique<TransformComponent>());

		TransformComponent* CameraTransform = this->GetComponent<TransformComponent>();
		CameraTransform->SetPosition({ 0.0f, 3.0f, -6.0f });
		CameraTransform->SetRotation({ 0.0f, 0.0f, 0.0f });
		DirectX::XMFLOAT3 target = { 0.0f, 0.0f, 0.0f }; // �{���������ꏊ
		DirectX::XMFLOAT3 up = { 0.0f, 1.0f, 0.0f }; // ���[���h�̏����

		// �������������悤�Ƀ^�[�Q�b�g�����ɂ��炷
		target.y -= 90.0f; 

		CameraTransform->LookAt(target, up);

    }
	~CameraEntity() override = default; 
};