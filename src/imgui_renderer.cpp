#include "pch.h"

#include <imgui_renderer.h>

#include <GWCA/Managers/ChatMgr.h>
#include <GWCA/Managers/MemoryMgr.h>
#include <GWCA/Managers/RenderMgr.h>

namespace
{
    static long OldWndProc;
    static DrawCallback* pDrawCallback = nullptr;
    static bool bImGuiInitialized = false;
    static bool bShutdown = false;
}


static LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    static bool right_mouse_down = false;

    if (Message == WM_RBUTTONDOWN) right_mouse_down = true;
    if (Message == WM_RBUTTONDBLCLK) right_mouse_down = true;
    if (Message == WM_RBUTTONUP) right_mouse_down = false;

    ImGuiIO& io = ImGui::GetIO();

    //
    // The first switch case is used to update the state of imgui with respect of the inputs.
    //

    switch (Message) {
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
        if (!right_mouse_down) io.MouseDown[0] = true;
        break;
    case WM_LBUTTONUP:
        io.MouseDown[0] = false; 
        break;
    case WM_MBUTTONDOWN:
    case WM_MBUTTONDBLCLK:
        if (!right_mouse_down) {
            io.KeysDown[VK_MBUTTON] = true;
            io.MouseDown[2] = true;
        }
        break;
    case WM_MBUTTONUP:
        io.KeysDown[VK_MBUTTON] = false;
        io.MouseDown[2] = false;
        break;
    case WM_MOUSEWHEEL: 
        if (!right_mouse_down) io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f; 
        break;
    case WM_MOUSEMOVE:
        if (!right_mouse_down) {
            io.MousePos.x = (float)GET_X_LPARAM(lParam);
            io.MousePos.y = (float)GET_Y_LPARAM(lParam);
        }
        break;
    case WM_XBUTTONDOWN:
        if (!right_mouse_down) {
            if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) io.KeysDown[VK_XBUTTON1] = true;
            if (GET_XBUTTON_WPARAM(wParam) == XBUTTON2) io.KeysDown[VK_XBUTTON2] = true;
        }
        break;
    case WM_XBUTTONUP:
        if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1) io.KeysDown[VK_XBUTTON1] = false;
        if (GET_XBUTTON_WPARAM(wParam) == XBUTTON2) io.KeysDown[VK_XBUTTON2] = false;
        break;
    case WM_SYSKEYDOWN:
    case WM_KEYDOWN:
        if (wParam < 256)
            io.KeysDown[wParam] = true;
        break;
    case WM_SYSKEYUP:
    case WM_KEYUP:
        if (wParam < 256)
            io.KeysDown[wParam] = false;
        break;
    case WM_CHAR: // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
        if (wParam > 0 && wParam < 0x10000)
            io.AddInputCharacter((unsigned short)wParam);
        break;
    default:
        break;
    }

    //
    // This second switch is used to determine whether we need to forward the input to Guild Wars.
    //

    switch (Message) {
    // Send button up mouse events to everything, to avoid being stuck on mouse-down
    case WM_LBUTTONUP:
        break;
        
    // Other mouse events:
    // - If right mouse down, leave it to gw
    // - ImGui first (above), if WantCaptureMouse that's it
    // - Toolbox module second (e.g.: minimap), if captured, that's it
    // - otherwise pass to gw
    case WM_LBUTTONDOWN:
    case WM_LBUTTONDBLCLK:
    case WM_MOUSEMOVE:
    case WM_MOUSEWHEEL:
        if (!right_mouse_down && io.WantCaptureMouse)
            return true;
        break;

    // keyboard messages
    case WM_KEYUP:
    case WM_SYSKEYUP:
        if (io.WantTextInput) break; // if imgui wants them, send to imgui (above) and to gw
        // else fallthrough
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_CHAR:
    case WM_SYSCHAR:
    case WM_IME_CHAR:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONDBLCLK:
    case WM_XBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONDBLCLK:
    case WM_MBUTTONUP:
        if (io.WantTextInput) return true; // if imgui wants them, send just to imgui (above)

        // note: capturing those events would prevent typing if you have a hotkey assigned to normal letters. 
        // We may want to not send events to toolbox if the player is typing in-game
        // Otherwise, we may want to capture events. 
        // For that, we may want to only capture *successfull* hotkey activations.
        break;

    case WM_SIZE:
        // ImGui doesn't need this, it reads the viewport size directly
        break;

    default:
        break;
    }

    return CallWindowProc((WNDPROC)OldWndProc, hWnd, Message, wParam, lParam);
}

static LRESULT CALLBACK SafeWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
    __try {
        return WndProc(hWnd, Message, wParam, lParam);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        return CallWindowProc(reinterpret_cast<WNDPROC>(OldWndProc), hWnd, Message, wParam, lParam);
    }
}


void InitializeImGui(DrawCallback* cb)
{
    if (bImGuiInitialized)
        return;

    pDrawCallback = cb;
    GW::Render::SetRenderCallback(DrawInternal);
    GW::Render::SetResetCallback([](IDirect3DDevice9* device) {
        UNREFERENCED_PARAMETER(device);
        ImGui_ImplDX9_InvalidateDeviceObjects();
    });

    bImGuiInitialized = true;
}

static void DrawInternal(IDirect3DDevice9* device)
{
	// This is call from within the game thread and all operation should be done here.
    // You can't freeze this thread, so no blocking operation or at your own risk.
    static bool initialized = false;
    static bool keep_rendering = true;

    if (!initialized) {
        HWND hWnd = GW::MemoryMgr::GetGWWindowHandle();
        OldWndProc = SetWindowLongPtr(hWnd, GWL_WNDPROC, reinterpret_cast<long>(SafeWndProc));

        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hWnd);
        ImGui_ImplDX9_Init(device);

        ImGuiIO& io = ImGui::GetIO();
        io.MouseDrawCursor = false;
        io.ConfigWindowsMoveFromTitleBarOnly = true;
        initialized = true;
    }

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::GetIO().KeysDown[VK_CONTROL] = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
    ImGui::GetIO().KeysDown[VK_SHIFT] = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
    ImGui::GetIO().KeysDown[VK_MENU] = (GetKeyState(VK_MENU) & 0x8000) != 0;

    if (pDrawCallback)
        keep_rendering = pDrawCallback(device);

    ImGui::EndFrame();

    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
    ImGui_ImplDX9_RenderDrawData(draw_data);

    if (!keep_rendering) {
        HWND hWnd = GW::MemoryMgr::GetGWWindowHandle();

        ImGui_ImplDX9_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
        SetWindowLongPtr(hWnd, GWL_WNDPROC, OldWndProc);
        GW::DisableHooks();
        bShutdown = true;
    }
}

bool IsRunning()
{
    return !bShutdown;
}