#pragma once

#include "include/exampleEntity.h"
#include "include/IScene.h"

// GeneratorEntity��CubeGeneratorComponent�����G���e�B�e�B�ŁA�L���[�u�����@�\��S�����܂��B

class GeneratorEntity : public EntityBase
{
public:
	GeneratorEntity();
	~GeneratorEntity() override = default; // �f�X�g���N�^�͕K�v�Ȃ�I�[�o�[���C�h
};