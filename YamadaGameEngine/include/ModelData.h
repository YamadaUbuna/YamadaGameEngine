#pragma once
#include "include/MeshComponent.h"
#include "include/MaterialComponent.h"

#include <string>
#include <vector>

// ���b�V���R���|�[�l���g�̃R���N�V�������Ǘ�����N���X�ł��B
// �����̃��b�V����ێ����A�ǉ���擾���s���܂��B
// ���f���̍\���v�f�Ƃ��ė��p����܂��B

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

