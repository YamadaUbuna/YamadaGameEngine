#pragma once
#include "include/EntityBase.h"
#include "include/IScene.h"

#include "include/exampleComponent.h"
#include "include/TransformComponent.h"
#include "include/MaterialComponent.h"
#include "include/ModelComponent.h"

class IScene;

class exampleEntity : public EntityBase {
    // EntityBase�̎��������̂܂܎g���̂�GetComponent/AddComponent�͕s�v
    // ������exampleEntity�ŗL�̏���������Βǉ�

public:
    exampleEntity(IScene* scene);
	~exampleEntity() override = default; // �f�X�g���N�^�͕K�v�Ȃ�I�[�o�[���C�h
};

