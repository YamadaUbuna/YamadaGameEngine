#pragma once
#include "include/EntityBase.h"
#include "include/IScene.h"

#include "include/exampleComponent.h"
#include "include/TransformComponent.h"
#include "include/MaterialComponent.h"
#include "include/ModelComponent.h"

// exampleEntity�N���X�̃R���X�g���N�^�����B
// exampleComponent�ATransformComponent�AModelComponent�i"cube"���f���j������{�I�ȃG���e�B�e�B�B

class IScene;

class exampleEntity : public EntityBase {

public:
    exampleEntity();
	~exampleEntity() override = default; // �f�X�g���N�^�͕K�v�Ȃ�I�[�o�[���C�h
};

