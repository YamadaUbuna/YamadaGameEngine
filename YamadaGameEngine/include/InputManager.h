#pragma once
class InputManager
{
public:
    static void Initialize();
    static void Update();

    static void OnKeyDown(WPARAM key);
    static void OnKeyUp(WPARAM key);

    static bool GetKey(UINT key);
    static bool GetKeyDown(UINT key);
    static bool GetKeyUp(UINT key);

private:
    static bool m_currentKeys[256];
    static bool m_prevKeys[256];
};

