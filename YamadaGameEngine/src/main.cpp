#include "include/pch.h"
#include "include/SceneManager.h"

#include "include/InputManager.h"

#include "include/Renderer.h"

// �O���[�o���ϐ�
HWND g_hWnd = nullptr;
const int g_WindowWidth = 1280;
const int g_WindowHeight = 720;

// �E�B���h�E�v���V�[�W��
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_KEYDOWN:
        if ((lParam & (1 << 30)) == 0) {
            InputManager::OnKeyDown(wParam);
        }
        return 0;

    case WM_KEYUP:
        InputManager::OnKeyUp(wParam);
        return 0;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}



// WinMain�G���g���[�|�C���g
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    // �E�B���h�E�N���X��`
    WNDCLASSEX wc = {
        sizeof(WNDCLASSEX),
        CS_CLASSDC,
        WndProc,
        0L, 0L,
        hInstance,
        nullptr, nullptr, nullptr, nullptr,
        L"DX12WindowClass",
        nullptr
    };

    RegisterClassEx(&wc);

    // �E�B���h�E�쐬
    RECT wr = { 0, 0, g_WindowWidth, g_WindowHeight };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
    g_hWnd = CreateWindowW(
        wc.lpszClassName,
        L"DirectX12 Window",
        WS_OVERLAPPEDWINDOW,
        100, 100,
        wr.right - wr.left, wr.bottom - wr.top,
        nullptr, nullptr,
        wc.hInstance,
        nullptr
    );

    ShowWindow(g_hWnd, nCmdShow);
    UpdateWindow(g_hWnd);

    //�F�X�ȏ�����
    Renderer::GetInstance().Initialize(g_hWnd, g_WindowWidth, g_WindowHeight);

    PipelineManager::GetInstance().Initialize();

	SceneManager::GetInstance().Initialize();

    InputManager::Initialize();

    // �^�C�}�[�̏�����
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);

    LARGE_INTEGER prevTime;
    QueryPerformanceCounter(&prevTime);

    // ���b�Z�[�W���[�v
    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {

            // deltaTime�̌v�Z
            LARGE_INTEGER currentTime;
            QueryPerformanceCounter(&currentTime);

            float deltaTime = static_cast<float>(currentTime.QuadPart - prevTime.QuadPart) / frequency.QuadPart;
            prevTime = currentTime;
            Renderer::GetInstance().RenderBegin();
			SceneManager::GetInstance().UpdateCurrent(deltaTime);//�����ŕ`�������̂ŁA�㉺��RenderBegin��RenderEnd���ĂԕK�v������
            Renderer::GetInstance().RenderEnd();

            InputManager::Update();//SceneUpdate�̂��Ƃɂ��
            
        }
    }
    Renderer::GetInstance().Terminate();

    UnregisterClass(wc.lpszClassName, wc.hInstance);
    return 0;
}
