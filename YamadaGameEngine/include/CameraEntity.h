#pragma once
#include "include/EntityBase.h"
#include "CameraComponent.h"
#include "TransformComponent.h"

class CameraEntity : public EntityBase {
    // EntityBase�̎��������̂܂܎g���̂�GetComponent/AddComponent�͕s�v
    // ������exampleEntity�ŗL�̏���������Βǉ�
public:

    CameraEntity() {
		AddComponent(std::make_unique<CameraComponent>());
		AddComponent(std::make_unique<TransformComponent>());

		//���̉��̂���A��p��System������Ă��ׂ��Ȃ̂ŁA���Ƃł������Ă���������
		TransformComponent* CameraTransform = this->GetComponent<TransformComponent>();
		CameraTransform->SetPosition({ 0.0f, 0.0f, -5.0f });
		CameraTransform->SetRotation({ 0.0f, 0.0f, 0.0f });
    }
	~CameraEntity() override = default; // �f�X�g���N�^�͕K�v�Ȃ�I�[�o�[���C�h
};