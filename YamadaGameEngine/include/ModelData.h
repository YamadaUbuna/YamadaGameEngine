#pragma once
#include "include/MeshComponent.h"
#include "include/MaterialComponent.h"

#include <string>
#include <vector>

// メッシュコンポーネントのコレクションを管理するクラスです。
// 複数のメッシュを保持し、追加や取得を行います。
// モデルの構成要素として利用されます。

class ModelDataContainer
{
public:
    const std::vector<std::unique_ptr<MeshComponent>>& GetMeshes() const { return m_meshes; }

    void AddMesh(std::unique_ptr<MeshComponent> mesh) {
        m_meshes.push_back(std::move(mesh));
    }

private:
    std::vector<std::unique_ptr<MeshComponent>> m_meshes;
};

