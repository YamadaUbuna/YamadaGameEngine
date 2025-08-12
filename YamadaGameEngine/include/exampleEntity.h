#pragma once
#include "include/EntityBase.h"
#include "include/IScene.h"

#include "include/exampleComponent.h"
#include "include/TransformComponent.h"
#include "include/MaterialComponent.h"
#include "include/ModelComponent.h"

class IScene;

class exampleEntity : public EntityBase {
    // EntityBaseの実装をそのまま使うのでGetComponent/AddComponentは不要
    // ここにexampleEntity固有の処理があれば追加

public:
    exampleEntity(IScene* scene);
	~exampleEntity() override = default; // デストラクタは必要ならオーバーライド
};

