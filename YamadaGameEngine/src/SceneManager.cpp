#include "include\pch.h"
#include "include/SceneManager.h"

#include "include/GameScene.h"

void SceneManager::Initialize() {

    //�V�[���}�l�[�W���[�ɃV�[����o�^
    SceneManager::GetInstance().AddScene("Game", std::make_unique<GameScene>());

    // �����ōŏ��Ɏg���V�[����ݒ�
    SceneManager::GetInstance().ChangeScene("Game");
}