#pragma once
#include "include/ISystem.h"

// TransformComponent�̉�]��񂩂�O���E��E�E�����x�N�g�����v�Z���A
// �e�G���e�B�e�B��Transform���X�V����V�X�e���ł��B
// �X�P�[���E��]�E���s�ړ���g�ݍ��킹�����[���h�s����������܂��B
// ���t���[���A�S�G���e�B�e�B��Transform���X�V���Ĕ��f���܂��B

class TransformSystem :
    public ISystem
{
public:
    void Initialize(IScene& scene) override;
    void Update(IScene& scene, float deltaTime) override;
};

