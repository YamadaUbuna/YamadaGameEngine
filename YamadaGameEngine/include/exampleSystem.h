#pragma once
#include "include/ISystem.h"

// exampleSystem��exampleComponent�����G���e�B�e�B�̏������ƍX�V��S�����܂��B
// �������ł͈ʒu�ƃ}�e���A���̐F��ݒ肵�A�X�V�ł̓G���e�B�e�B���E�����Ɉړ������܂��B
// �ʒu�����l�𒴂���ƃG���e�B�e�B��j�����܂��B

class exampleSystem :
    public ISystem
{
public:
    void Initialize(IScene& scene) override;
    void Initialize(std::unique_ptr<IEntity>& entity, IScene& scene);
    void Update(IScene& scene, float deltaTime) override;
};

