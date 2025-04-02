#include "common.hpp"
#include "imgui.h"
#include "putrid.hpp"
#include <memory>

void SettingsGUI(bool * run, std::shared_ptr<AssetManager> asMgr, std::shared_ptr<ConfigManager> cfgMgr) {
    if (*run) {
        // Create a new draft cfg
        static Config draft_cfg = *cfgMgr->GetActiveConfig();

        // Window title
        ImGui::Begin("Config", NULL, ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_AlwaysAutoResize);

        // Disable imgui config
        ImGuiIO* io = &ImGui::GetIO();
        io->IniFilename = NULL;
        io->LogFilename = NULL;

        ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor::HSV(1.0f,0.0f,0.0f));
        ImGui::Text("Display Settings");
        ImGui::PopStyleColor();

        // Monitor selection
        ImGui::SliderInt("Monitor number", &draft_cfg.res.selectedMonitor, 0, GetMonitorCount() - 1);
        ImGui::Text("Selected Monitor: %s", GetMonitorName(draft_cfg.res.selectedMonitor));

        // Disable resolution settings if in fullscreen
        if (draft_cfg.res.fullscreen) {
            ImGui::BeginDisabled();
        }

        // Screen Ratio Selection
        const char * ratios[RATIOCOUNTER] = {"4:3", "16:9", "16:10"};
        const char * selected_ratio = ratios[draft_cfg.res.selectedRatio]; // unsure if safe...?
        ImGui::NewLine();
        ImGui::SliderInt("Aspect Ratio", &draft_cfg.res.selectedRatio, 0, RATIOCOUNTER - 1, selected_ratio, ImGuiSliderFlags_NoInput);

        // Display Resolution Multiplier
        ImGui::SliderInt("##", &draft_cfg.res.multiplier, 1, 6, "Resolution");

        int baseResolutionX;
        int baseResolutionY;
        if (draft_cfg.res.selectedRatio == FOURBYTHREE) {
            baseResolutionX = 640;
            baseResolutionY = 480;
        } else if (draft_cfg.res.selectedRatio == SIXTEENBYNINE) {
            baseResolutionX = 640;
            baseResolutionY = 360;
        } else {
            baseResolutionX = 1280;
            baseResolutionY = 800;
        }

        draft_cfg.res.x = draft_cfg.res.multiplier * baseResolutionX;
        draft_cfg.res.y = draft_cfg.res.multiplier * baseResolutionY;

        ImGui::Text("Selected: %dx%d", draft_cfg.res.x, draft_cfg.res.y);

        // Disable and reset resolution controls if fullscreen is enabled. otherwise, the window breaks.
        if (draft_cfg.res.fullscreen) {
            ImGui::EndDisabled();
            draft_cfg.res.x = GetMonitorWidth(draft_cfg.res.selectedMonitor);
            draft_cfg.res.y = GetMonitorHeight(draft_cfg.res.selectedMonitor);
        }

        // Framerate selection
        ImGui::InputInt("Target FPS", &draft_cfg.res.targetFPS);

        // Fullscreen setting
        ImGui::Checkbox("Fullscreen", &draft_cfg.res.fullscreen);
        if (ImGui::Button("Discard Changes")) {
            *run = false;
            draft_cfg = *cfgMgr->GetActiveConfig();
        }

        // Closing buttons
        ImGui::SameLine();
        if (ImGui::Button("Save and Close")) {
            *run = false;
            cfgMgr->SetActiveConfig(draft_cfg);
            cfgMgr->SaveConfig(asMgr.get(), *cfgMgr->GetActiveConfig());
        }
        ImGui::SameLine();
        if (ImGui::Button("Apply")) { // consider enabling autorevert
            cfgMgr->SetActiveConfig(draft_cfg);
            cfgMgr->SaveConfig(asMgr.get(), *cfgMgr->GetActiveConfig());
        }

        ImGui::End();
    }
}
