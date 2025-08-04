#pragma once
#include <unordered_map>
#include <string>
#include <memory>
#include <stdexcept>

#include "include/IScene.h"


class SceneManager
{
private:
    std::unordered_map<std::string, std::unique_ptr<IScene>> m_scenes;
    IScene* m_currentScene = nullptr;

public:
    // �V�[����o�^
    void AddScene(const std::string& name, std::unique_ptr<IScene> scene) {
        m_scenes[name] = std::move(scene);
    }

    // �V�[����؂�ւ�
    void ChangeScene(const std::string& name) {
        auto it = m_scenes.find(name);
        if (it != m_scenes.end()) {
            m_currentScene = it->second.get();
        }
        else {
            throw std::runtime_error("Scene not found: " + name);
        }
    }

    // ���݂̃V�[�����X�V�E�`��
    void UpdateCurrent(float deltaTime) {
        if (m_currentScene) m_currentScene->Update(deltaTime);
    }

    void InitializeCurrent() {
        if (m_currentScene) m_currentScene->Initialize();
	}
};

