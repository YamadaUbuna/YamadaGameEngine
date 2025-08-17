#include "include\pch.h"
#include "include/SceneManager.h"

#include "include/GameScene.h"

void SceneManager::Initialize() {

    //シーンマネージャーにシーンを登録
    SceneManager::GetInstance().AddScene("Game", std::make_unique<GameScene>());

    // ここで最初に使うシーンを設定
    SceneManager::GetInstance().ChangeScene("Game");

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