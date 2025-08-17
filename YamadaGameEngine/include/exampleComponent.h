#pragma once
#include "include/IComponent.h"

// exampleComponentクラスの定義。
// IComponentを継承した空のコンポーネント。

class exampleComponent :
    public IComponent
{

public:
    ~exampleComponent() = default;
};

