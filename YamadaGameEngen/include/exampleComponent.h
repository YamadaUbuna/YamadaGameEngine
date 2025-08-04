#pragma once
#include "include/IComponent.h"
#include <string>
#include <sstream>

class exampleComponent :
    public IComponent
{
private:
    std::string m_myName = "yamada";

public:
    std::string GetMyName() { return m_myName; }
};

