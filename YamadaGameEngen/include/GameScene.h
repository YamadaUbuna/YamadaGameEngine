#pragma once
#include "include/IScene.h"
#include "include/exampleEntity.h"

class GameScene : public IScene
{
public:
    GameScene() {};
    ~GameScene() override {};

    void Initialize() override;
    void Update(float deltaTime) override;
    void Cleanup() override;

    // Entity �Ǘ�
    void AddEntity(std::unique_ptr<IEntity> entity) override
    {
        m_entities.emplace_back(std::move(entity));
    }
    IEntity* CreateEntity() {
        std::unique_ptr<exampleEntity> entity = std::make_unique<exampleEntity>(); // Entity�͋�ۃN���X
        IEntity* rawPtr = entity.get();           // �|�C���^��Ԃ��iScene�����L�j
        m_entities.push_back(std::move(entity));    // Scene���ɕۑ��i�폜���Ǘ��j
        return rawPtr;
    }

    void RemoveEntity(IEntity* entity) override;
    const std::vector<std::unique_ptr<IEntity>>& GetEntities() const override { return m_entities; }

    // System �Ǘ�
    void AddSystem(ISystem* system) override 
    {
        m_systems.push_back(system);
    }
    void RemoveSystem(ISystem* system) override;
    const std::vector<ISystem*>& GetSystems() const override { return m_systems; }

private:
    std::vector<std::unique_ptr<IEntity>> m_entities;
    std::vector<ISystem*> m_systems;
    // Camera m_mainCamera;
};
