#pragma once
#include "include/IScene.h"

// ISystem�̓V�X�e�������̋��ʃC���^�[�t�F�[�X�ł��B
// �V�[�����ł̏������█�t���[���̍X�V�������s�����߂̒��ۃN���X�ł��B
// �������Z��A�j���[�V�����ȂǁA�Q�[�����W�b�N�̎����ɗ��p����܂��B

class IScene;

class ISystem {
public:
    virtual ~ISystem() = default;

    // ���W�b�N�����i�������Z�A�A�j���[�V�����Ȃǁj
    virtual void Initialize(IScene& scene) = 0;
    virtual void Update(IScene& scene, float deltaTime) = 0;
};
