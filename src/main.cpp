#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "pch.h"

#include <GWCA/Constants/Constants.h>
#include <GWCA/Managers/AgentMgr.h>
#include <GWCA/Managers/PlayerMgr.h>
#include <GWCA/Managers/ChatMgr.h>
#include <GWCA/Utilities/Hooker.h>

#include <GWCA/GameEntities/Agent.h>

#include <imgui_renderer.h>


namespace
{
    static bool running = true;
    static bool keepRendering = true;
    
    std::string WStringToString(const std::wstring& wstr)
    {
        if (wstr.empty()) return std::string();
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
        std::string strTo(static_cast<size_t>(size_needed), 0);
        WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
        return strTo;
    }

    std::wstring StringToWString(const std::string& str)
    {
        if (str.empty()) return std::wstring();
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
        std::wstring wstrTo(static_cast<size_t>(size_needed), 0);
        MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
        return wstrTo;
    }

}

// Return false to exit
bool Draw(IDirect3DDevice9* device)
{
    UNREFERENCED_PARAMETER(device);

    static bool keep_alive = true;
    static bool init = false;
    static bool show_demo = true;
    static bool show_window = true;

    if (!init)
    {
        GW::Chat::WriteChat(GW::Chat::CHANNEL_MODERATOR, APP_NAMEW L": Initialized");
        init = true;
    }    

    if (show_demo)
        ImGui::ShowDemoWindow(&show_demo);

    //ImGui::SetNextWindowSize({200, 150});
    if (ImGui::Begin(APP_NAME, &show_window, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_AlwaysAutoResize))
    {
        GW::Agent* playerAgent = GW::Agents::GetPlayer();

        if (playerAgent)
        {
            auto living = playerAgent->GetAsAgentLiving();
            auto playerName = WStringToString(GW::PlayerMgr::GetPlayerName(living->player_number));
            ImGui::Text("Your Name: %s", playerName.c_str());
            ImGui::Text("Your LoginId: %d", living->player_number);
            ImGui::Text("Your AgentId: %d", living->agent_id);
            auto primary = GW::Constants::GetProfessionAcronym((GW::Constants::Profession)living->primary);
            auto secondary = GW::Constants::GetProfessionAcronym((GW::Constants::Profession)living->secondary);
            ImGui::Text("Your Profession: %s/%s", primary, secondary);
        }

        if (!show_demo && ImGui::Button("Show Demo"))
            show_demo = true;
    }
    ImGui::End();


    if (!show_window)
    {
        GW::Chat::WriteChat(GW::Chat::CHANNEL_MODERATOR, APP_NAMEW L": Bye!");
        keep_alive = false;
    }        

    return keep_alive;
}

static DWORD WINAPI ThreadProc(LPVOID lpModule)
{
    HMODULE hModule = static_cast<HMODULE>(lpModule);

    if (GW::Initialize())
    {
        InitializeImGui(Draw);

        while (IsRunning()) {
            Sleep(100);
        }

        while (GW::HookBase::GetInHookCount())
            Sleep(16);

        Sleep(16);
        GW::Terminate();
    }

    FreeLibraryAndExitThread(hModule, EXIT_SUCCESS);
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(lpReserved);
    DisableThreadLibraryCalls(hModule);

    if (dwReason == DLL_PROCESS_ATTACH) {
        HANDLE handle = CreateThread(0, 0, ThreadProc, hModule, 0, 0);
        if (handle)
            CloseHandle(handle);
    }

    return TRUE;
}
