#pragma once

#include "include/IComponent.h"

// �L���[�u�����Ǘ��R���|�[�l���g�B
// ��莞�Ԃ��ƂɊm���ŃL���[�u����������s���B
// �����Ԋu�iSpawnInterval�j�Əo���m���iAppearance�j��ݒ�\�B
// �A���񐶐���h�����W�b�N�����B

class CubeGeneratorComponent : public IComponent {
public:

    CubeGeneratorComponent() = default;
    ~CubeGeneratorComponent() = default;

    void UpdateTimer(float deltaTime) { m_timer += deltaTime; }

    bool ShouldSpawn() {
        if (m_timer >= m_spawnInterval) {
            m_timer -= m_spawnInterval;

            // appearance �Ɋ�Â��m������
            bool spawn = (static_cast<float>(rand()) / RAND_MAX) <= m_appearance;

            // �A���Ő����Ȃ���h�~
            if (!spawn && !m_lastSpawned) spawn = true;

            m_lastSpawned = spawn; // ����̌��ʂ�ۑ�
            return spawn;
        }
        return false;
    }

    void SetSpawn(float spawn) { m_spawnInterval = spawn; }
    const float GetSpawn() const{ return m_spawnInterval; }
    
    void SetAppearance(float appearance) { m_appearance = appearance; }
    const float GetAppearance() const { return m_appearance; }

private:
    float m_spawnInterval = 1.0f;
    float m_timer;
    float m_appearance = 0.6f;
    bool m_lastSpawned;     // �O�񐶐��t���O
};
