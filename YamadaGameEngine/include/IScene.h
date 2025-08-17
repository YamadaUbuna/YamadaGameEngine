#pragma once

#include "include/IEntity.h"
#include "include/ISystem.h"
#include "include/exampleEntity.h"
#include "include/AssetManager.h"

#include <vector>
#include <memory>

// IScene�́A�V�[���̊�{�C���^�[�t�F�[�X���`���钊�ۃN���X�ł��B
// �G���e�B�e�B�ƃV�X�e���̊Ǘ��A�������E�X�V�E�N���[���A�b�v������S�����܂��B
// �A�Z�b�g�Ǘ��⃁�C���J�����G���e�B�e�B�̎擾�E�ݒ���\�ł��B
// �V�[�����Ƃ̎����͂��̃C���^�[�t�F�[�X���p�����čs���܂��B


class ISystem;

class IScene {
public:
    virtual ~IScene() = default;

    virtual void Initialize() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Cleanup() = 0;

    //virtual IEntity* CreateEntity() = 0;
    virtual void RemoveEntity(IEntity* entity) = 0;
    virtual const std::vector<std::unique_ptr<IEntity>>& GetEntities() const = 0;

    virtual void AddSystem(ISystem* system) = 0;
    virtual void RemoveSystem(ISystem* system) = 0;
    virtual const std::vector<ISystem*>& GetSystems() const = 0;

    virtual AssetManager& GetAssetManager() = 0;

	virtual IEntity* GetMainCameraEntity() const = 0; // �J�����̎擾
	virtual void SetMainCameraEntity(IEntity* camera) = 0; // �J�����̐ݒ�
   
};
