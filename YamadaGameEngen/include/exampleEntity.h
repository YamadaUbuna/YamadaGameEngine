#pragma once
#include "include/IEntity.h"
#include "IEntity.h"
class exampleEntity :
    public IEntity
{
private:
    std::unordered_map<std::type_index, std::unique_ptr<IComponent>> m_components;

public:
    IComponent* GetComponent(std::type_index type) override {
        auto it = m_components.find(type);
        return it != m_components.end() ? it->second.get() : nullptr;
    }

    void AddComponent(std::type_index type, std::unique_ptr<IComponent> comp) override {
        m_components[type] = std::move(comp);
    }
};

class ExampleEntity :
    public IEntity
{
};

