#include "include\pch.h"
#include "include/SceneManager.h"

#include "include/GameScene.h"

void SceneManager::Initialize() {

    //�V�[���}�l�[�W���[�ɃV�[����o�^
    SceneManager::GetInstance().AddScene("Game", std::make_unique<GameScene>());

    // �����ōŏ��Ɏg���V�[����ݒ�
    SceneManager::GetInstance().ChangeScene("Game");

    m_changeSceneFlag = false;
    auto it = m_scenes.find(m_nextSceneName);
    if (it != m_scenes.end()) {
        // ���݂̃V�[�����N���[���A�b�v
        CleanupCurrent();

        // �V�����V�[����ݒ�
        m_currentScene = it->second.get();
    }
    else {
        throw std::runtime_error("Scene not found: " + m_nextSceneName);
    }

    InitializeCurrent();
    
}