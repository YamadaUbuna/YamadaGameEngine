#pragma once
#include "include/ISystem.h"

// �L���[�u�����V�X�e���B
// �V�[������CubeGeneratorComponent�����G���e�B�e�B���X�V���A
// �������ŐV�����L���[�u�G���e�B�e�B�𐶐��E����������B

class CubeGeneratorSystem :
    public ISystem
{
public:
    void Initialize(IScene& scene) override;
    void Update(IScene& scene, float deltaTime) override;

private:
    float m_spawnTimer = 0.0f;  // �o�ߎ��Ԃ�ێ�
};

