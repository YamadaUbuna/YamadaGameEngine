#pragma once
#include <Windows.h>
#include <array>

// キーボードとマウス入力を管理するシングルトンクラスです。
// キーの押下状態やマウスの座標・移動量、ホイール回転を取得可能です。
// 各種入力イベントの更新処理や状態取得用メソッドを提供します。

class InputManager
{
public:
    static void Initialize();
    static void Update();

    // キーボード
    static void OnKeyDown(WPARAM key);
    static void OnKeyUp(WPARAM key);
    static bool GetKey(UINT key);
    static bool GetKeyDown(UINT key);
    static bool GetKeyUp(UINT key);

    // マウス
    static void OnMouseMove(LPARAM lParam);
    static void OnMouseButtonDown(WPARAM btn);
    static void OnMouseButtonUp(WPARAM btn);
    static void OnMouseWheel(short delta);

    static POINT GetMousePos();       // 現在座標
    static POINT GetMouseDelta();     // 移動量
    static int   GetWheelDelta();     // ホイール回転量
    static bool  GetMouseButton(int btn);
    static bool  GetMouseButtonDown(int btn);
    static bool  GetMouseButtonUp(int btn);

private:
    // キーボード
    static bool m_currentKeys[256];
    static bool m_prevKeys[256];

    // マウス
    static POINT m_mousePos;
    static POINT m_prevMousePos;
    static int   m_wheelDelta;

    static bool m_currentMouse[5]; // 左右中 + X1 + X2
    static bool m_prevMouse[5];
};
