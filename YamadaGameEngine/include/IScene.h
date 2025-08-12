#pragma once
#include <vector>
#include <memory>
#include "include/IEntity.h"
#include "include/ISystem.h"
#include "include/exampleEntity.h"
#include "AssetManager.h"

class ISystem;

class IScene {
public:
    virtual ~IScene() = default;

    virtual void Initialize() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Cleanup() = 0;

    //virtual IEntity* CreateEntity() = 0;
    virtual void RemoveEntity(IEntity* entity) = 0;
    virtual const std::vector<std::unique_ptr<IEntity>>& GetEntities() const = 0;

    virtual void AddSystem(ISystem* system) = 0;
    virtual void RemoveSystem(ISystem* system) = 0;
    virtual const std::vector<ISystem*>& GetSystems() const = 0;

    virtual AssetManager& GetAssetManager() = 0;

	virtual IEntity* GetMainCameraEntity() const = 0; // ƒJƒƒ‰‚Ìæ“¾
	virtual void SetMainCameraEntity(IEntity* camera) = 0; // ƒJƒƒ‰‚Ìİ’è
   
};
