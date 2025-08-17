#pragma once
#include "include/IScene.h"
#include "Renderer.h"

#include <unordered_map>
#include <string>
#include <memory>
#include <stdexcept>

// �V�[���̊Ǘ��Ɛ؂�ւ����s���V���O���g���N���X�B
// �����̃V�[����o�^���A���݂̃V�[���̏������E�X�V�E�N���[���A�b�v�𐧌�B
// �V�[���̕ύX�v�����󂯂Ĉ��S�ɃV�[����؂�ւ��鏈�����S���B

class SceneManager
{
public:

    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

    // �C���X�^���X�擾
    static SceneManager& GetInstance() {
        static SceneManager instance;
        return instance;
    }

	// �V�[���}�l�[�W���[�̏�����
    void Initialize();

    // �V�[����o�^
    void AddScene(const std::string& name, std::unique_ptr<IScene> scene) {
        m_scenes[name] = std::move(scene);
    }

    // �V�[����؂�ւ�
    void ChangeScene(const std::string& name) {

        m_nextSceneName = name;
        m_changeSceneFlag = true;
    }

    void ChangeSceneProcess()
    {
        if (m_changeSceneFlag) {
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
    }

    void UpdateCurrent(float deltaTime) {
        if (m_currentScene) m_currentScene->Update(deltaTime);
    }

    void InitializeCurrent() {
        if (m_currentScene) m_currentScene->Initialize();
	}

    void CleanupCurrent() {
        if (m_currentScene) m_currentScene->Cleanup();
	}

private:
    std::unordered_map<std::string, std::unique_ptr<IScene>> m_scenes;
    IScene* m_currentScene = nullptr;
    std::string m_nextSceneName;
    bool m_changeSceneFlag = false;

    SceneManager() = default;
};

