#pragma once
#include <unordered_map>
#include <memory>
#include "include/IComponent.h"
#include <typeindex>

template<typename T>
concept Component = std::derived_from<T, IComponent>;

//T が IComponent を継承しているかどうかをチェックする制約（Concept）
//C++20の機能。
//つまり「TransformComponent とか RenderComponent とかは、全部 IComponent を継承してね」という前提を作ってる。

class IEntity {
public:
    virtual ~IEntity() = default;

    // 型情報から取得（内部用）型を直接指定せずに、「型情報（typeid）」を使ってコンポーネントを管理する。std::type_index は型の識別子みたいなもの。
    virtual IComponent* GetComponent(std::type_index type) = 0;
    virtual void AddComponent(std::type_index type, std::unique_ptr<IComponent> comp) = 0;


    // 型Tで取得（外部向け）
    //上の「低レベル関数」をラップした、使いやすい「型付きのGetComponent関数」
    //呼び出し側から見ると、こうなる：
    //TransformComponent* t = entity->GetComponent<TransformComponent>();

    template<Component T>
    T* GetComponent() {
        auto comp = GetComponent(std::type_index(typeid(T)));
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
        AddComponent(std::type_index(typeid(T)), std::move(comp));
    }
};
