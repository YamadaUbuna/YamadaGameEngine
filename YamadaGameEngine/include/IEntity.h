#pragma once

#include "include/IComponent.h"

#include <unordered_map>
#include <memory>
#include <typeindex>

// IEntity�́A�G���e�B�e�B�����ׂ����ʃC���^�[�t�F�[�X���`���钊�ۃN���X�ł��B
// �R���|�[�l���g�̒ǉ��E�擾���^���S�ɍs�����߂̃e���v���[�g�֐���񋟂��܂��B
// �����ł�std::type_index���g���āA�^�Ɉˑ����Ȃ��R���|�[�l���g�Ǘ����������܂��B
// �G���e�B�e�B�̔j����ԁiDestroy�t���O�j���Ǘ�����Ă��܂��B

template<typename T>
concept Component = std::derived_from<T, IComponent>;

class IEntity {
public:
    virtual ~IEntity() = default;

    // �^��񂩂�擾�i�����p�j�^�𒼐ڎw�肹���ɁA�u�^���itypeid�j�v���g���ăR���|�[�l���g���Ǘ�����Bstd::type_index �͌^�̎��ʎq�݂����Ȃ��́B
    virtual IComponent* GetComponent(std::type_index type) = 0;
    virtual void AddComponent(std::type_index type, std::unique_ptr<IComponent> comp) = 0;

    // �^T�Ŏ擾�i�O�������j
    //��́u�჌�x���֐��v�����b�v�����A�g���₷���u�^�t����GetComponent�֐��v

    template<Component T>
    T* GetComponent() {
        IComponent* comp = GetComponent(std::type_index(typeid(T)));
        if (!comp) return nullptr;

#ifdef _DEBUG
        assert(dynamic_cast<T*>(comp) != nullptr && "Invalid cast in GetComponent<T>()");
#endif

        return static_cast<T*>(comp);
    }

    // �^T�Œǉ��i�O�������j
    // �������A�g���₷���u�^�t����AddComponent�v�֐��B
    //�Ăяo�����͂���������F
    //entity->AddComponent(std::make_unique<TransformComponent>());

    template<Component T>
    void AddComponent(std::unique_ptr<T> comp) {
        AddComponent(std::type_index(typeid(T)), std::move(comp));  // �����̏���
    }

    void Destroy() { m_isDestroyed = true; }
    bool IsDestroyed() const { return m_isDestroyed; }

private:
    bool m_isDestroyed = false;

};
