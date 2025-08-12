#include "include\pch.h"
#include "include/SceneManager.h"

#include "include/GameScene.h"

void SceneManager::Initialize() {

    //シーンマネージャーにシーンを登録
    SceneManager::GetInstance().AddScene("Game", std::make_unique<GameScene>());

    // ここで最初に使うシーンを設定
    SceneManager::GetInstance().ChangeScene("Game");
}