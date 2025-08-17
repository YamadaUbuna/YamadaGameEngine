#pragma once

#include "include/IEntity.h"
#include "include/ISystem.h"
#include "include/exampleEntity.h"
#include "include/AssetManager.h"

#include <vector>
#include <memory>

// ISceneは、シーンの基本インターフェースを定義する抽象クラスです。
// エンティティとシステムの管理、初期化・更新・クリーンアップ処理を担当します。
// アセット管理やメインカメラエンティティの取得・設定も可能です。
// シーンごとの実装はこのインターフェースを継承して行います。


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

	virtual IEntity* GetMainCameraEntity() const = 0; // カメラの取得
	virtual void SetMainCameraEntity(IEntity* camera) = 0; // カメラの設定
   
};
