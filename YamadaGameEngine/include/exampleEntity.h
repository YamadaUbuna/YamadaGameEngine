#pragma once
#include "include/EntityBase.h"
#include "include/IScene.h"

#include "include/exampleComponent.h"
#include "include/TransformComponent.h"
#include "include/MaterialComponent.h"
#include "include/ModelComponent.h"

// exampleEntityクラスのコンストラクタ実装。
// exampleComponent、TransformComponent、ModelComponent（"cube"モデル）を持つ基本的なエンティティ。

class IScene;

class exampleEntity : public EntityBase {

public:
    exampleEntity();
	~exampleEntity() override = default; // デストラクタは必要ならオーバーライド
};

