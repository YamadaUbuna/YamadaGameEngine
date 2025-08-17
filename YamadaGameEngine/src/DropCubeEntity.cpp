#include "include/pch.h"
#include "include/DropCubeEntity.h"

DropCubeEntity::DropCubeEntity()
{
	AddComponent(std::make_unique<DropCubeComponent>());
	AddComponent(std::make_unique<TransformComponent>());
	AddComponent(std::make_unique<ModelComponent>("cube"));

	TransformComponent* transform = this->GetComponent<TransformComponent>();
	transform->SetPosition({ 0.0f,1.0f,0.0f });

}
