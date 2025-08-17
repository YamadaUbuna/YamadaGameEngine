#pragma once

#include "include/EntityBase.h"
#include "include/IScene.h"

#include "include/DropCubeComponent.h"
#include "include/TransformComponent.h"
#include "include/MaterialComponent.h"
#include "include/ModelComponent.h"

class IScene;

class DropCubeEntity : public EntityBase {

public:
    DropCubeEntity();
    ~DropCubeEntity() override = default; // デストラクタは必要ならオーバーライド
};
