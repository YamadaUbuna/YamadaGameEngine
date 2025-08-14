#pragma once

#include "include/exampleEntity.h"
#include "include/IScene.h"

class cubeEntity : public EntityBase
{
public:
	cubeEntity();
	~cubeEntity() override = default; // デストラクタは必要ならオーバーライド
};