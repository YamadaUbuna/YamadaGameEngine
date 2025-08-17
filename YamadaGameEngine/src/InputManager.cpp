#include "include/pch.h"
#include "include/InputManager.h"
#include <cstring>
#include <Windowsx.h>

bool InputManager::m_currentKeys[256] = {};
bool InputManager::m_prevKeys[256] = {};

POINT InputManager::m_mousePos = { 0,0 };
POINT InputManager::m_prevMousePos = { 0,0 };
int InputManager::m_wheelDelta = 0;

bool InputManager::m_currentMouse[5] = {};
bool InputManager::m_prevMouse[5] = {};

void InputManager::Initialize()
{
    memset(m_currentKeys, 0, sizeof(m_currentKeys));
    memset(m_prevKeys, 0, sizeof(m_prevKeys));
    memset(m_currentMouse, 0, sizeof(m_currentMouse));
    memset(m_prevMouse, 0, sizeof(m_prevMouse));
    m_mousePos = { 0,0 };
    m_prevMousePos = { 0,0 };
    m_wheelDelta = 0;
}

void InputManager::Update()
{
    memcpy(m_prevKeys, m_currentKeys, sizeof(m_currentKeys));
    memcpy(m_prevMouse, m_currentMouse, sizeof(m_currentMouse));
    m_prevMousePos = m_mousePos;
    m_wheelDelta = 0; // フレームごとにリセット
}

// ==== キーボード ====
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

// ==== マウス ====
void InputManager::OnMouseMove(LPARAM lParam)
{
    m_mousePos.x = GET_X_LPARAM(lParam);
    m_mousePos.y = GET_Y_LPARAM(lParam);
}

void InputManager::OnMouseButtonDown(WPARAM btn)
{
    if (btn >= 0 && btn < 5)
        m_currentMouse[btn] = true;
}
void InputManager::OnMouseButtonUp(WPARAM btn)
{
    if (btn >= 0 && btn < 5)
        m_currentMouse[btn] = false;
}

void InputManager::OnMouseWheel(short delta)
{
    m_wheelDelta = delta;
}

POINT InputManager::GetMousePos()
{
    return m_mousePos;
}
POINT InputManager::GetMouseDelta()
{
    return { m_mousePos.x - m_prevMousePos.x, m_mousePos.y - m_prevMousePos.y };
}
int InputManager::GetWheelDelta()
{
    return m_wheelDelta;
}
bool InputManager::GetMouseButton(int btn)
{
    return (btn >= 0 && btn < 5) ? m_currentMouse[btn] : false;
}
bool InputManager::GetMouseButtonDown(int btn)
{
    return (btn >= 0 && btn < 5) ? (m_currentMouse[btn] && !m_prevMouse[btn]) : false;
}
bool InputManager::GetMouseButtonUp(int btn)
{
    return (btn >= 0 && btn < 5) ? (!m_currentMouse[btn] && m_prevMouse[btn]) : false;
}
