#include "include\pch.h"
#include "include/exampleEntity.h"
#include "include/IScene.h"

exampleEntity::exampleEntity()
{
    AddComponent(std::make_unique<exampleComponent>());
    AddComponent(std::make_unique<TransformComponent>());
	AddComponent(std::make_unique<ModelComponent>("cube"));
}
