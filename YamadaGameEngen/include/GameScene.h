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

    // Entity 管理
    void AddEntity(std::unique_ptr<IEntity> entity) override
    {
        m_entities.emplace_back(std::move(entity));
    }
    IEntity* CreateEntity() {
        std::unique_ptr<exampleEntity> entity = std::make_unique<exampleEntity>(); // Entityは具象クラス
        IEntity* rawPtr = entity.get();           // ポインタを返す（Sceneが所有）
        m_entities.push_back(std::move(entity));    // Scene内に保存（削除も管理）
        return rawPtr;
    }

    void RemoveEntity(IEntity* entity) override;
    const std::vector<std::unique_ptr<IEntity>>& GetEntities() const override { return m_entities; }

    // System 管理
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
