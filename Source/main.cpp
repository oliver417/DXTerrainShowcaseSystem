#include <Windows.h>
#include "Graphics/D3D11Renderer.h"
#include "Graphics/Camera.h"

LRESULT CALLBACK WindowProc(
    HWND hwnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(
        hwnd,
        message,
        wParam,
        lParam
    );
}

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE,
    LPSTR,
    int nCmdShow)
{
    constexpr int WIDTH = 1280;
    constexpr int HEIGHT = 720;

    const wchar_t CLASS_NAME[] =
        L"DXTerrainShowcaseWindow";

    WNDCLASSW wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    if (!RegisterClassW(&wc))
        return -1;

    HWND hwnd = CreateWindowExW(
        0,
        CLASS_NAME,
        L"DX11 Terrain Showcase",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        WIDTH,
        HEIGHT,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (!hwnd)
        return -1;

    ShowWindow(hwnd, nCmdShow);

    // -------------------------
    // DirectX 11 초기화
    // -------------------------

    D3D11Renderer renderer;

    if (!renderer.Initialize(hwnd, WIDTH, HEIGHT))
    {
        MessageBoxW(
            hwnd,
            L"DirectX 11 initialization failed.",
            L"Error",
            MB_OK | MB_ICONERROR
        );

        return -1;
    }

    Camera camera;

    camera.SetPosition(
        0.0f,
        5.0f,
        -10.0f
    );

    camera.SetRotation(
        15.0f,
        0.0f,
        0.0f
    );

    camera.SetProjection(
        60.0f,
        static_cast<float>(WIDTH) /
        static_cast<float>(HEIGHT),
        0.1f,
        1000.0f
    );

    camera.Update();

    // -------------------------
    // Game Loop
    // -------------------------

    MSG msg = {};

    bool running = true;

    while (running)
    {
        while (PeekMessage(
            &msg,
            nullptr,
            0,
            0,
            PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                running = false;
                break;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (!running)
            break;

        renderer.BeginFrame();

        // 앞으로 여기에 Terrain.Render()

        renderer.EndFrame();
    }

    renderer.Shutdown();

    return 0;
}