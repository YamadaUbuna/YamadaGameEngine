#pragma once

#include "include/exampleEntity.h"
#include "include/IScene.h"

// GeneratorEntityはCubeGeneratorComponentを持つエンティティで、キューブ生成機能を担当します。

class GeneratorEntity : public EntityBase
{
public:
	GeneratorEntity();
	~GeneratorEntity() override = default; // デストラクタは必要ならオーバーライド
};