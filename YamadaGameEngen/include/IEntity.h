#pragma once
#include <unordered_map>
#include <memory>
#include "include/IComponent.h"
#include <typeindex>

template<typename T>
concept Component = std::derived_from<T, IComponent>;

//T �� IComponent ���p�����Ă��邩�ǂ������`�F�b�N���鐧��iConcept�j
//C++20�̋@�\�B
//�܂�uTransformComponent �Ƃ� RenderComponent �Ƃ��́A�S�� IComponent ���p�����Ăˁv�Ƃ����O�������Ă�B

class IEntity {
public:
    virtual ~IEntity() = default;

    // �^��񂩂�擾�i�����p�j�^�𒼐ڎw�肹���ɁA�u�^���itypeid�j�v���g���ăR���|�[�l���g���Ǘ�����Bstd::type_index �͌^�̎��ʎq�݂����Ȃ��́B
    virtual IComponent* GetComponent(std::type_index type) = 0;
    virtual void AddComponent(std::type_index type, std::unique_ptr<IComponent> comp) = 0;


    // �^T�Ŏ擾�i�O�������j
    //��́u�჌�x���֐��v�����b�v�����A�g���₷���u�^�t����GetComponent�֐��v
    //�Ăяo�������猩��ƁA�����Ȃ�F
    //TransformComponent* t = entity->GetComponent<TransformComponent>();

    template<Component T>
    T* GetComponent() {
        auto comp = GetComponent(std::type_index(typeid(T)));
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
        AddComponent(std::type_index(typeid(T)), std::move(comp));
    }
};
