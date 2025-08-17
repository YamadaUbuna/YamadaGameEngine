#include "include\pch.h"
#include "include/GeneratorEntity.h"
#include "include/CubeGeneratorComponent.h"

GeneratorEntity::GeneratorEntity() {
	AddComponent(std::make_unique<CubeGeneratorComponent>());
}