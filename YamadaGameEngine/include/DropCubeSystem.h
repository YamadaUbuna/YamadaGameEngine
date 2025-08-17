#pragma once
#include "include/ISystem.h"

// �����L���[�u�̐���V�X�e���B
// �X�y�[�X�L�[�ŗ����J�n�A���ʒu�Œ�~���ď������s���A�V�����L���[�u������Q�[���I�[�o�[������s���B

class DropCubeSystem :
    public ISystem
{
public:
    void Initialize(IScene& scene) override;
    void Update(IScene& scene, float deltaTime) override;

private:
    bool AreAllCubesInRange(IScene& scene);

    bool hit = false;
};

