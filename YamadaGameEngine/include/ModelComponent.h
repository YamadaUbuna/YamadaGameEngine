#pragma once
#include "include/MeshComponent.h"
#include "include/IComponent.h"
#include "include/AssetManager.h"
#include "include/ModelData.h"

#include <memory>

// ModelComponentは、モデルデータへの参照（ID）を保持する軽量なコンポーネントです。
// 実体（メッシュなど）はAssetManagerが管理し、本クラスはID経由でアクセスします。
// モデルデータ自体は保持せず、シーン内での参照専用として機能します。


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
