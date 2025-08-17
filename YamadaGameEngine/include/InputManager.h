#pragma once
#include <Windows.h>
#include <array>

// �L�[�{�[�h�ƃ}�E�X���͂��Ǘ�����V���O���g���N���X�ł��B
// �L�[�̉�����Ԃ�}�E�X�̍��W�E�ړ��ʁA�z�C�[����]���擾�\�ł��B
// �e����̓C�x���g�̍X�V�������Ԏ擾�p���\�b�h��񋟂��܂��B

class InputManager
{
public:
    static void Initialize();
    static void Update();

    // �L�[�{�[�h
    static void OnKeyDown(WPARAM key);
    static void OnKeyUp(WPARAM key);
    static bool GetKey(UINT key);
    static bool GetKeyDown(UINT key);
    static bool GetKeyUp(UINT key);

    // �}�E�X
    static void OnMouseMove(LPARAM lParam);
    static void OnMouseButtonDown(WPARAM btn);
    static void OnMouseButtonUp(WPARAM btn);
    static void OnMouseWheel(short delta);

    static POINT GetMousePos();       // ���ݍ��W
    static POINT GetMouseDelta();     // �ړ���
    static int   GetWheelDelta();     // �z�C�[����]��
    static bool  GetMouseButton(int btn);
    static bool  GetMouseButtonDown(int btn);
    static bool  GetMouseButtonUp(int btn);

private:
    // �L�[�{�[�h
    static bool m_currentKeys[256];
    static bool m_prevKeys[256];

    // �}�E�X
    static POINT m_mousePos;
    static POINT m_prevMousePos;
    static int   m_wheelDelta;

    static bool m_currentMouse[5]; // ���E�� + X1 + X2
    static bool m_prevMouse[5];
};
