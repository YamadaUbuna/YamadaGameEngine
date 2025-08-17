#pragma once
#include "include/ISystem.h"
#include "include/CameraComponent.h"

#include <DirectXMath.h>

// CameraSystem��ISystem���p�����A�V�[������CameraComponent���Ǘ��E�X�V����V�X�e���ł��B
// �J�����̃r���[�s��Ⓤ�e�s��̌v�Z�E�X�V������S�����܂��B
// Initialize��Update�ŃV�[���̃J������Ԃ𖈃t���[�����䂵�܂��B

class CameraSystem :
    public ISystem
{
public:
    void Initialize(IScene& scene) override;
    void Update(IScene& scene, float deltaTime) override;

private:
    void UpdateMatrices(CameraComponent* cam);
};

