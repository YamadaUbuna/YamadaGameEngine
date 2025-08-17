#pragma once

#include "include/IComponent.h"

#include <unordered_map>
#include <memory>
#include <typeindex>

// IEntityは、エンティティが持つべき共通インターフェースを定義する抽象クラスです。
// コンポーネントの追加・取得を型安全に行うためのテンプレート関数を提供します。
// 内部ではstd::type_indexを使って、型に依存しないコンポーネント管理を実現します。
// エンティティの破棄状態（Destroyフラグ）も管理されています。

template<typename T>
concept Component = std::derived_from<T, IComponent>;

class IEntity {
public:
    virtual ~IEntity() = default;

    // 型情報から取得（内部用）型を直接指定せずに、「型情報（typeid）」を使ってコンポーネントを管理する。std::type_index は型の識別子みたいなもの。
    virtual IComponent* GetComponent(std::type_index type) = 0;
    virtual void AddComponent(std::type_index type, std::unique_ptr<IComponent> comp) = 0;

    // 型Tで取得（外部向け）
    //上の「低レベル関数」をラップした、使いやすい「型付きのGetComponent関数」

    template<Component T>
    T* GetComponent() {
        IComponent* comp = GetComponent(std::type_index(typeid(T)));
        if (!comp) return nullptr;

#ifdef _DEBUG
        assert(dynamic_cast<T*>(comp) != nullptr && "Invalid cast in GetComponent<T>()");
#endif

        return static_cast<T*>(comp);
    }

    // 型Tで追加（外部向け）
    // 同じく、使いやすい「型付きのAddComponent」関数。
    //呼び出し側はこう書ける：
    //entity->AddComponent(std::make_unique<TransformComponent>());

    template<Component T>
    void AddComponent(std::unique_ptr<T> comp) {
        AddComponent(std::type_index(typeid(T)), std::move(comp));  // 既存の処理
    }

    void Destroy() { m_isDestroyed = true; }
    bool IsDestroyed() const { return m_isDestroyed; }

private:
    bool m_isDestroyed = false;

};
