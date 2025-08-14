#pragma once
#include <string>
#include <vector>
#include "include/MeshComponent.h"
#include "include/MaterialComponent.h"

class ModelDataContainer
{
public:
    const std::vector<std::unique_ptr<MeshComponent>>& GetMeshes() const { return m_meshes; }

    void AddMesh(std::unique_ptr<MeshComponent> mesh) {
        m_meshes.push_back(std::move(mesh));
    }

private:
    std::vector<std::unique_ptr<MeshComponent>> m_meshes;
    //std::unordered_map<std::wstring, std::unique_ptr<MaterialContainer>> m_pMaterialContainer;
};

