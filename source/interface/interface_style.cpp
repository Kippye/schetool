#include "imgui.h"
#include "interface_style.h"

enum class GuiStyle
{
    Light,
    Dark
};

// NOTE: Do NOT make styles have each other as their base style, it will cause an endless loop -_-
const std::map<GuiStyle, GuiStyleDefinition> InterfaceStyleHandler::styleDefinitions =
{
    {GuiStyle::Light, {"Light", std::nullopt, [](){
        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text]                   = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
        colors[ImGuiCol_WindowBg]               = ImVec4(0.95f, 0.95f, 0.95f, 0.94f);
        colors[ImGuiCol_PopupBg]                = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
        colors[ImGuiCol_FrameBg]                = ImVec4(0.81f, 0.81f, 0.81f, 0.54f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.69f, 0.83f, 0.87f, 0.54f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.56f, 0.67f, 0.71f, 0.67f);
        colors[ImGuiCol_TitleBg]                = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.44f, 0.75f, 0.87f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.16f, 0.16f, 0.16f, 0.51f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.54f, 0.68f, 0.90f, 1.00f);
        colors[ImGuiCol_CheckMark]              = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
        colors[ImGuiCol_Button]                 = ImVec4(0.85f, 0.92f, 0.93f, 1.00f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.79f, 0.86f, 0.87f, 1.00f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.66f, 0.71f, 0.72f, 1.00f);
        colors[ImGuiCol_Header]                 = ImVec4(0.48f, 0.73f, 0.78f, 0.31f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.50f, 0.77f, 0.82f, 0.80f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.55f, 0.84f, 0.90f, 1.00f);
        colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.48f, 0.73f, 0.78f, 0.31f);
        colors[ImGuiCol_TableBorderLight]       = ImVec4(0.77f, 0.77f, 0.77f, 1.00f);
        colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.84f, 0.83f, 0.83f, 0.53f);
        colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.43f, 0.43f, 0.43f, 1.00f);
        colors[ImGuiCol_CheckMark]              = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
    }}},
    {GuiStyle::Dark, {"Dark", std::nullopt, [](){
        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_WindowBg]               = ImVec4(0.14f, 0.14f, 0.16f, 0.94f);
        colors[ImGuiCol_PopupBg]                = ImVec4(0.19f, 0.19f, 0.21f, 0.94f);
        colors[ImGuiCol_FrameBg]                = ImVec4(0.26f, 0.26f, 0.26f, 0.54f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.31f, 0.31f, 0.31f, 0.54f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.45f, 0.45f, 0.45f, 0.67f);
        colors[ImGuiCol_TitleBg]                = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.26f, 0.28f, 0.33f, 1.00f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
        colors[ImGuiCol_CheckMark]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
        colors[ImGuiCol_Button]                 = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
        colors[ImGuiCol_Header]                 = ImVec4(0.58f, 0.58f, 0.58f, 0.31f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.62f, 0.62f, 0.62f, 0.80f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.71f, 0.71f, 0.71f, 1.00f);
        colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.08f, 0.08f, 0.08f, 0.53f);
        colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
        colors[ImGuiCol_TableBorderLight]       = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    }}},
};

void InterfaceStyleHandler::applyStyle(GuiStyle style)
{
    // The style has no definition. It probably shouldn't be applied.
    if (styleDefinitions.contains(style) == false)
    {
        return;
    }
    const GuiStyleDefinition& styleDefinition = styleDefinitions.at(style); 
    // If the style has a base style, apply that first.
    if (styleDefinition.baseStyle.has_value())
    {
        if (styleDefinition.baseStyle.value() != style) // If the base style was allowed to be the same as the deriving style, it would lead to an endless recursion loop!
        {
            applyStyle(styleDefinition.baseStyle.value());
        }
    }
    styleDefinition.applyFunction();
    m_currentStyle = style;
}

GuiStyle InterfaceStyleHandler::getCurrentStyle() const
{
    return m_currentStyle;
}

GuiStyle InterfaceStyleHandler::getDefaultStyle() const
{
    return m_defaultStyle;
}

FontScale InterfaceStyleHandler::getFontScale() const
{
    return m_currentFontScale;
}

void InterfaceStyleHandler::setFontScale(FontScale fontScale)
{
    ImGui::GetIO().FontGlobalScale = gui_fonts::fontScaleMultipliers.at(fontScale);
    m_currentFontScale = fontScale;
}