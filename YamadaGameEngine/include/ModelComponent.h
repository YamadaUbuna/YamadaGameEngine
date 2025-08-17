#pragma once
#include "include/MeshComponent.h"
#include "include/IComponent.h"
#include "include/AssetManager.h"
#include "include/ModelData.h"

#include <memory>

// ModelComponent�́A���f���f�[�^�ւ̎Q�ƁiID�j��ێ�����y�ʂȃR���|�[�l���g�ł��B
// ���́i���b�V���Ȃǁj��AssetManager���Ǘ����A�{�N���X��ID�o�R�ŃA�N�Z�X���܂��B
// ���f���f�[�^���͕̂ێ������A�V�[�����ł̎Q�Ɛ�p�Ƃ��ċ@�\���܂��B


class ModelComponent : public IComponent {
public:
    ModelComponent(const std::string& id) : m_modelId(id) {}

    const std::string& GetModelId() const { return m_modelId; }

    // ���̃A�N�Z�X
    const ModelDataContainer* GetModelData(AssetManager& assets) const {
        return assets.GetModel(m_modelId);
    }

private:
    std::string m_modelId; // ���͎̂����Ȃ�
};
