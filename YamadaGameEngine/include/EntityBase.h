#pragma once
#include "include/IEntity.h"
#include <unordered_map>

// EntityBase�́A�R���|�[�l���g�x�[�X�̃G���e�B�e�B�����������{�N���X�ł��B
// �C�ӂ̌^��IComponent���}�b�v�ɕێ����A���I�Ȓǉ��E�擾���\�ɂ��܂��B
// �^���S�ȃe���v���[�g��GetComponent/AddComponent���񋟂��Ă��܂��B
// IEntity�C���^�[�t�F�[�X���������A�V�[�����̃I�u�W�F�N�g�Ǘ��ɗ��p����܂��B


class EntityBase : public IEntity {
protected:
    std::unordered_map<std::type_index, std::unique_ptr<IComponent>> m_components;

public:
    IComponent* GetComponent(std::type_index type) override {
        auto it = m_components.find(type);
        return it != m_components.end() ? it->second.get() : nullptr;
    }

    void AddComponent(std::type_index type, std::unique_ptr<IComponent> comp) override {
        m_components[type] = std::move(comp);
    }

    template<Component T>
    T* GetComponent() {
        IComponent* comp = GetComponent(std::type_index(typeid(T)));
        if (!comp) return nullptr;
#ifdef _DEBUG
        assert(dynamic_cast<T*>(comp) != nullptr && "Invalid cast in GetComponent<T>()");
#endif
        return static_cast<T*>(comp);
    }

    // unique_ptr �̏ꍇ
    template<Component T>
    void AddComponent(std::unique_ptr<T> comp) {
        AddComponent(std::type_index(typeid(T)), std::move(comp));  // �����̏���
    }

};

