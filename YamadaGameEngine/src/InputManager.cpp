#include "include/pch.h"
#include "include/InputManager.h"

bool InputManager::m_currentKeys[256] = {};
bool InputManager::m_prevKeys[256] = {};

void InputManager::Initialize()
{
    memset(m_currentKeys, 0, sizeof(m_currentKeys));
    memset(m_prevKeys, 0, sizeof(m_prevKeys));
}

void InputManager::Update()
{
    memcpy(m_prevKeys, m_currentKeys, sizeof(m_currentKeys));
}

void InputManager::OnKeyDown(WPARAM key)
{
    if (key < 256) m_currentKeys[key] = true;
}

void InputManager::OnKeyUp(WPARAM key)
{
    if (key < 256) m_currentKeys[key] = false;
}

bool InputManager::GetKey(UINT key)
{
    return key < 256 ? m_currentKeys[key] : false;
}

bool InputManager::GetKeyDown(UINT key)
{
    return key < 256 ? (m_currentKeys[key] && !m_prevKeys[key]) : false;
}

bool InputManager::GetKeyUp(UINT key)
{
    return key < 256 ? (!m_currentKeys[key] && m_prevKeys[key]) : false;
}
