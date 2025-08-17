#pragma once
#include "include/IScene.h"
#include "include/exampleEntity.h"

// GameSceneはISceneを実装し、エンティティとシステムの管理を行うゲームの主要シーンクラスです。
// エンティティの生成、削除、更新、およびカメラ管理機能を提供します。
// 複数のシステムを登録してシーン全体の処理を制御します。


class GameScene : public IScene
{
public:
    GameScene() {};
    ~GameScene() = default;;

    void Initialize() override;
    void Update(float deltaTime) override;
    void Cleanup() override;

    template<typename T, typename... Args, typename = std::enable_if_t<std::is_base_of_v<IEntity, T>>>
    IEntity* CreateEntity(Args&&... args) {
        auto entity = std::make_unique<T>(std::forward<Args>(args)...);  // 引数を渡して生成
        IEntity* rawPtr = entity.get();
        m_entities.push_back(std::move(entity));
        return rawPtr;
    }

    void RemoveEntity(IEntity* entity) override;
    const std::vector<std::unique_ptr<IEntity>>& GetEntities() const override { return m_entities; }

    void RemoveDestroyedEntities();

    // System 管理
    void AddSystem(ISystem* system) override 
    {
        m_systems.push_back(system);
    }
    void RemoveSystem(ISystem* system) override;
    const std::vector<ISystem*>& GetSystems() const override { return m_systems; }

	// カメラの取得
	IEntity* GetMainCameraEntity() const {// カメラを持っているEntityを返す。
        return m_mainCamera;
    }
    void SetMainCameraEntity(IEntity* camera) override;

    AssetManager& GetAssetManager() override { return assetManager; }

private:
    std::vector<std::unique_ptr<IEntity>> m_entities;
    std::vector<ISystem*> m_systems;
    IEntity* m_mainCamera = nullptr;
    AssetManager assetManager;
};
