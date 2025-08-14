#pragma once
#include <memory>
#include "MeshComponent.h"
#include "IComponent.h"
#include "include/AssetManager.h"
#include "ModelData.h"


class ModelComponent : public IComponent {
public:
    ModelComponent(const std::string& id) : m_modelId(id) {}

    const std::string& GetModelId() const { return m_modelId; }

    // 実体アクセス
    const ModelDataContainer* GetModelData(AssetManager& assets) const {
        return assets.GetModel(m_modelId);
    }

private:
    std::string m_modelId; // 実体は持たない
};
