#pragma once
#include "include/IScene.h"
#include "include/exampleEntity.h"

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
        auto entity = std::make_unique<T>(std::forward<Args>(args)...);  // ˆø”‚ğ“n‚µ‚Ä¶¬
        IEntity* rawPtr = entity.get();
        m_entities.push_back(std::move(entity));
        return rawPtr;
    }

    void RemoveEntity(IEntity* entity) override;
    const std::vector<std::unique_ptr<IEntity>>& GetEntities() const override { return m_entities; }

    // System ŠÇ—
    void AddSystem(ISystem* system) override 
    {
        m_systems.push_back(system);
    }
    void RemoveSystem(ISystem* system) override;
    const std::vector<ISystem*>& GetSystems() const override { return m_systems; }

	// ƒJƒƒ‰‚Ìæ“¾
	IEntity* GetMainCameraEntity() const {// ƒJƒƒ‰‚ğ‚Á‚Ä‚¢‚éEntity‚ğ•Ô‚·B
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
