
#include "platform.hpp"

// Windows platform layer
#if PLATFORM_WINDOWS

#include <Windows.h>
#include <windowsx.h>
#include "core/logger.hpp"

struct Win32State
{
    HINSTANCE hInstance = nullptr;
    HWND handle = nullptr;
    bool shouldClose = false;
};

static Win32State win32State;

void Platform::fatalError(const string &message)
{
    MessageBoxA(0, message.c_str(), "FATAL ERROR", MB_ICONEXCLAMATION | MB_OK);
    abort(); // TODO : Change this to allow ressource freeing before exit.
}

LRESULT CALLBACK win32ProcessMessages(HWND hwnd, UINT umsg, WPARAM wParam, LPARAM lParam)
{
    switch (umsg)
    {
    case WM_ERASEBKGND:
        // Ereasing will by handled by the application to prevent flicker.
        return 1;
    case WM_CLOSE:
        win32State.shouldClose = true;
        return 0;
    case WM_DESTROY:
        win32State.shouldClose = true;
        PostQuitMessage(0);
        return 0;

    case WM_SIZE:
    {
        RECT newRect;
        GetClientRect(hwnd, &newRect);
        int width = newRect.right - newRect.left;
        int height = newRect.bottom - newRect.top;

        // TODO : fire event
    }
    break;

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        bool pressed = umsg == WM_KEYDOWN || umsg == WM_SYSKEYDOWN;
        // TODO : handle input
    }
    break;

    case WM_MOUSEMOVE:
    {
        int mouseX = GET_X_LPARAM(lParam);
        int mouseY = GET_Y_LPARAM(lParam);
    }
    break;

    case WM_MOUSEHWHEEL:
    {
        int delta = GET_WHEEL_DELTA_WPARAM(wParam);
        if (delta != 0)
            delta = delta < 0 ? -1 : 1; // Normalize input for OS-independent use
        // TODO : handle input
    }
    break;

    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    {
        bool pressed = umsg == WM_LBUTTONDOWN || umsg == WM_RBUTTONDOWN || umsg == WM_MBUTTONDOWN;
        // TODO : handle input
    }
    break;

    default:
        break;
    }

    return DefWindowProcA(hwnd, umsg, wParam, lParam);
}

Platform::Platform(const string &name, int width, int height)
{

    LOGDEBUG("Initializing platform...");

    // Can't initialize the system twice !
    if (win32State.handle != nullptr || win32State.hInstance != nullptr)
    {
        LOGWARN("Platform already initialized. Skipping initialization.");
        return;
    }

    // Get the application handle associated with the current process
    win32State.hInstance = GetModuleHandleA(0);

    // Window properties
    WNDCLASS wc = {};
    wc.lpfnWndProc = win32ProcessMessages;
    wc.hInstance = win32State.hInstance;
    wc.lpszClassName = "BasicBool_class_name";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(win32State.hInstance, IDI_APPLICATION);
    wc.style = CS_DBLCLKS; // Listen double-clicks
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hbrBackground = NULL; // transparent

    // Register window
    if (!RegisterClassA(&wc))
    {
        LOGFATAL("Window registration failed !");
        fatalError("Window registration failed !");
        return;
    }

    // --- Creating the window ---

    // window decoration
    UINT32 windowStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
    UINT32 windowExStyle = WS_EX_APPWINDOW;

    windowStyle |= WS_MAXIMIZEBOX;
    windowStyle |= WS_MINIMIZEBOX;
    windowStyle |= WS_THICKFRAME;

    // Get border size
    RECT borderRect = {0, 0, 0, 0};
    AdjustWindowRectEx(&borderRect, windowStyle, 0, windowExStyle);
    // Inner window size should match with user specified size
    int windowWidth = width + (borderRect.right - borderRect.left);
    int windowHeight = height + (borderRect.bottom - borderRect.top);

    win32State.handle = CreateWindowExA(
        windowExStyle, "BasicBool_class_name", "BasicBool", windowStyle,
        NULL, NULL, windowWidth, windowHeight, 0, 0, win32State.hInstance, 0);

    if (win32State.handle == 0)
    {
        LOGFATAL("Window creation failed !");
        fatalError("Window creation failed !");
        return;
    }

    ShowWindow(win32State.handle, SW_SHOW);
}

Platform::~Platform()
{
    DestroyWindow(win32State.handle);
    win32State.handle = 0;
    LOGDEBUG("Platform closed");
}

bool Platform::processEvents()
{
    MSG message;
    while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
    return !win32State.shouldClose;
}

void Platform::swapBuffers()
{
    // TODO : Implement
}

#endif