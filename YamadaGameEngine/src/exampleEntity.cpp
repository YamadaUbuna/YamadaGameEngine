#include "include\pch.h"
#include "include/exampleEntity.h"
#include "include/IScene.h"

exampleEntity::exampleEntity(IScene* scene)
{
    AddComponent(std::make_unique<exampleComponent>());
    AddComponent(std::make_unique<TransformComponent>());
	AddComponent(std::make_unique<ModelComponent>("hero"));

    TransformComponent* transformComponent = this->GetComponent<TransformComponent>();
    transformComponent->SetPosition({ 0.0f, 0.0f, 0.0f });
    transformComponent->SetRotation({ 0.0f, 0.0f, 0.0f });
    transformComponent->SetScale({ 1.0f, 1.0, 1.0 });


}
