// Styling functions, not necessary for function.
#pragma once

#include "src/vendor/imgui/imgui.h"

inline void SetupGuiStyle() {
    ImGuiStyle& style = ImGui::GetStyle();

    ImVec4 lowIntensityOrange = ImVec4(1.0f, 0.5f, 0.08f, 1.0f);
    ImVec4 highIntensityOrange = ImVec4(1.0f, 0.6f, 0.08f, 0.9f);

    style.Alpha = 1.0f;
    style.FrameRounding = 0.0f;
    style.Colors[ImGuiCol_Text] = ImVec4(0.0f, 0.2f, 0.60f, 1.0f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(.8f, 0.8f, .8f, 1.0f);
    style.Colors[ImGuiCol_PopupBg] = style.Colors[ImGuiCol_WindowBg];
    style.Colors[ImGuiCol_Border] = ImVec4(0.5f, .5f, .5f, 0.750f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.5f,0.5f,0.5f,1.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.6f,0.6f,0.6f,1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = highIntensityOrange;
    style.Colors[ImGuiCol_FrameBgActive] = highIntensityOrange;
    style.Colors[ImGuiCol_TitleBg] = lowIntensityOrange;
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.6f,0.6f,0.6f,1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = lowIntensityOrange;
    style.Colors[ImGuiCol_MenuBarBg] = highIntensityOrange;
    //style.Colors[ImGuiCol_ScrollbarBg];
    style.Colors[ImGuiCol_ScrollbarGrab] = lowIntensityOrange;
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = highIntensityOrange;
    style.Colors[ImGuiCol_ScrollbarGrabActive] = highIntensityOrange;
    style.Colors[ImGuiCol_CheckMark] = lowIntensityOrange;
    style.Colors[ImGuiCol_SliderGrab] = lowIntensityOrange;
    style.Colors[ImGuiCol_SliderGrabActive] = highIntensityOrange;
    style.Colors[ImGuiCol_Button] = lowIntensityOrange;
    style.Colors[ImGuiCol_ButtonHovered] = highIntensityOrange;
    style.Colors[ImGuiCol_ButtonActive] = highIntensityOrange;
    //style.Colors[ImGuiCol_Header] = style.Colors[ImGuiCol_ScrollbarBg];
    //style.Colors[ImGuiCol_HeaderHovered] = style.Colors[ImGuiCol_ScrollbarGrab];
    //style.Colors[ImGuiCol_HeaderActive] = style.Colors[ImGuiCol_ScrollbarGrabActive];
    //style.Colors[ImGuiCol_ResizeGrip]
    //style.Colors[ImGuiCol_ResizeGripHovered]
    //style.Colors[ImGuiCol_ResizeGripActive]
    style.Colors[ImGuiCol_PlotLines] = lowIntensityOrange;
    style.Colors[ImGuiCol_PlotLinesHovered] = highIntensityOrange;
    style.Colors[ImGuiCol_PlotHistogram] = lowIntensityOrange;
    style.Colors[ImGuiCol_PlotHistogramHovered] = highIntensityOrange;
    style.Colors[ImGuiCol_TextSelectedBg] = lowIntensityOrange;
    style.Colors[ImGuiCol_TableHeaderBg] = lowIntensityOrange;
}
