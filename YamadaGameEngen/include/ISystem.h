#pragma once
#include "include/IScene.h"

class IScene;

class ISystem {
public:
    virtual ~ISystem() = default;

    // ���W�b�N�����i�������Z�A�A�j���[�V�����Ȃǁj
    virtual void Initialize(IScene& scene) = 0;
    virtual void Update(IScene& scene, float deltaTime) = 0;

};
