#pragma once
#include "include/IScene.h"
#include "Renderer.h"

#include <unordered_map>
#include <string>
#include <memory>
#include <stdexcept>

// シーンの管理と切り替えを行うシングルトンクラス。
// 複数のシーンを登録し、現在のシーンの初期化・更新・クリーンアップを制御。
// シーンの変更要求を受けて安全にシーンを切り替える処理も担当。

class SceneManager
{
public:

    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

    // インスタンス取得
    static SceneManager& GetInstance() {
        static SceneManager instance;
        return instance;
    }

	// シーンマネージャーの初期化
    void Initialize();

    // シーンを登録
    void AddScene(const std::string& name, std::unique_ptr<IScene> scene) {
        m_scenes[name] = std::move(scene);
    }

    // シーンを切り替え
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
                // 現在のシーンをクリーンアップ
                CleanupCurrent();

                // 新しいシーンを設定
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

