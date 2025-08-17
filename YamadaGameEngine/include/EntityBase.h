#pragma once
#include "include/IEntity.h"
#include <unordered_map>

// EntityBaseは、コンポーネントベースのエンティティを実装する基本クラスです。
// 任意の型のIComponentをマップに保持し、動的な追加・取得を可能にします。
// 型安全なテンプレート版GetComponent/AddComponentも提供しています。
// IEntityインターフェースを実装し、シーン内のオブジェクト管理に利用されます。


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

    // unique_ptr の場合
    template<Component T>
    void AddComponent(std::unique_ptr<T> comp) {
        AddComponent(std::type_index(typeid(T)), std::move(comp));  // 既存の処理
    }

};

