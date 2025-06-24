#include "imgui.h"
#include "interface_style.h"

enum class GuiStyle {
    Light,
    Dark
};

const std::map<GuiStyle, const char*> InterfaceStyleHandler::styleNames = {
    {GuiStyle::Light, "Light"},
    {GuiStyle::Dark, "Dark"},
};

std::map<FontSize, ImFont*> InterfaceStyleHandler::loadedFonts = {};

// NOTE: Do NOT make styles have each other as their base style, it will cause an endless loop -_-
// NOTE: ImGuiCol_CheckMark is also used to color EVERY texture button that should be colored according to the style
// Currently, this mostly means gui_templates::ImageButtonStyleColored()
// NOTE: It is probably a good idea to define EVERY color that is defined in other styles
// Otherwise there might be leftover colors when switching between styles
const std::map<GuiStyle, GuiStyleDefinition> InterfaceStyleHandler::styleDefinitions = {
    {GuiStyle::Light,
     {std::nullopt,
      []() {
          ImVec4* colors = ImGui::GetStyle().Colors;
          colors[ImGuiCol_Text] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
          // Windows
          colors[ImGuiCol_WindowBg] = ImVec4(0.95f, 0.95f, 0.95f, 0.94f);
          colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
          colors[ImGuiCol_TitleBg] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
          colors[ImGuiCol_TitleBgActive] = ImVec4(0.44f, 0.75f, 0.87f, 1.00f);
          colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.16f, 0.16f, 0.16f, 0.51f);
          colors[ImGuiCol_ScrollbarBg] = ImVec4(0.84f, 0.83f, 0.83f, 0.53f);
          colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.64f, 0.64f, 0.64f, 1.00f);
          colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
          colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.43f, 0.43f, 0.43f, 1.00f);
          colors[ImGuiCol_MenuBarBg] = ImVec4(0.54f, 0.68f, 0.90f, 1.00f);
          // Frames
          colors[ImGuiCol_FrameBg] = ImVec4(0.81f, 0.81f, 0.81f, 0.54f);
          colors[ImGuiCol_FrameBgHovered] = ImVec4(0.69f, 0.83f, 0.87f, 0.54f);
          colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.67f, 0.71f, 0.67f);
          // Buttons
          colors[ImGuiCol_Button] = ImVec4(0.85f, 0.92f, 0.93f, 1.00f);
          colors[ImGuiCol_ButtonHovered] = ImVec4(0.79f, 0.86f, 0.87f, 1.00f);
          colors[ImGuiCol_ButtonActive] = ImVec4(0.66f, 0.71f, 0.72f, 1.00f);
          // Headers (Selectables + others)
          colors[ImGuiCol_Header] = ImVec4(0.48f, 0.73f, 0.78f, 0.31f);
          colors[ImGuiCol_HeaderHovered] = ImVec4(0.50f, 0.77f, 0.82f, 0.80f);
          colors[ImGuiCol_HeaderActive] = ImVec4(0.55f, 0.84f, 0.90f, 1.00f);
          // Tables
          colors[ImGuiCol_TableHeaderBg] = ImVec4(0.48f, 0.73f, 0.78f, 0.31f);
          colors[ImGuiCol_TableBorderLight] = ImVec4(0.77f, 0.77f, 0.77f, 1.00f);
          // Tabs
          colors[ImGuiCol_Tab] = ImVec4(0.851f, 0.922f, 0.929f, 1.00f);
          colors[ImGuiCol_TabHovered] = ImVec4(0.782f, 0.927f, 0.943f, 1.00f);
          colors[ImGuiCol_TabSelected] = ImVec4(0.719f, 0.907f, 0.948f, 1.00f);
          colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.898f, 0.931f, 0.938f, 1.00f);
          // Other
          colors[ImGuiCol_CheckMark] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
          ImGui::GetStyle().ChildRounding = 4.0f;
          ImGui::GetStyle().FrameRounding = 4.0f;
          ImGui::GetStyle().PopupRounding = 4.0f;
          ImGui::GetStyle().WindowRounding = 4.0f;
      }}},
    {GuiStyle::Dark,
     {std::nullopt,
      []() {
          ImVec4* colors = ImGui::GetStyle().Colors;
          colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
          // Windows
          colors[ImGuiCol_WindowBg] = ImVec4(0.14f, 0.14f, 0.16f, 0.94f);
          colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.21f, 0.94f);
          colors[ImGuiCol_TitleBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
          colors[ImGuiCol_TitleBgActive] = ImVec4(0.26f, 0.28f, 0.33f, 1.00f);
          colors[ImGuiCol_ScrollbarBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.53f);
          colors[ImGuiCol_MenuBarBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
          // Frames
          colors[ImGuiCol_FrameBg] = ImVec4(0.26f, 0.26f, 0.26f, 0.54f);
          colors[ImGuiCol_FrameBgHovered] = ImVec4(0.31f, 0.31f, 0.31f, 0.54f);
          colors[ImGuiCol_FrameBgActive] = ImVec4(0.45f, 0.45f, 0.45f, 0.67f);
          // Buttons
          colors[ImGuiCol_Button] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
          colors[ImGuiCol_ButtonHovered] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
          colors[ImGuiCol_ButtonActive] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
          // Headers (Selectables + others)
          colors[ImGuiCol_Header] = ImVec4(0.58f, 0.58f, 0.58f, 0.31f);
          colors[ImGuiCol_HeaderHovered] = ImVec4(0.62f, 0.62f, 0.62f, 0.80f);
          colors[ImGuiCol_HeaderActive] = ImVec4(0.71f, 0.71f, 0.71f, 1.00f);
          // Tables
          colors[ImGuiCol_TableHeaderBg] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
          colors[ImGuiCol_TableBorderStrong] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
          colors[ImGuiCol_TableBorderLight] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
          // Tabs
          colors[ImGuiCol_Tab] = ImVec4(0.302, 0.302f, 0.302, 1.0f);
          colors[ImGuiCol_TabHovered] = ImVec4(0.349f, 0.349f, 0.349f, 1.00f);
          colors[ImGuiCol_TabSelected] = ImVec4(0.502f, 0.502f, 0.502f, 1.00f);
          colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.640f, 0.640f, 0.640f, 1.00f);
          // Other
          colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
          // Rounding
          ImGui::GetStyle().ChildRounding = 4.0f;
          ImGui::GetStyle().FrameRounding = 4.0f;
          ImGui::GetStyle().PopupRounding = 4.0f;
          ImGui::GetStyle().WindowRounding = 4.0f;
      }}},
};

FontSize InterfaceStyleHandler::currentFontSize = FontSize::Normal;

void InterfaceStyleHandler::loadFontSizes(const char* fontPath) {
    for (FontSize fontSize = FontSize::Small; fontSize <= FontSize::Huge; fontSize = (FontSize)((int)fontSize + 1)) {
        loadedFonts.insert_or_assign(
            fontSize, ImGui::GetIO().Fonts->AddFontFromFileTTF(fontPath, gui_fonts::fontSizePixelSizes.at(fontSize)));
    }
}

void InterfaceStyleHandler::applyStyle(GuiStyle style) {
    // The style has no definition. It probably shouldn't be applied.
    if (styleDefinitions.contains(style) == false) {
        return;
    }
    const GuiStyleDefinition& styleDefinition = styleDefinitions.at(style);
    // If the style has a base style, apply that first.
    if (styleDefinition.baseStyle.has_value()) {
        if (styleDefinition.baseStyle.value() !=
            style)  // If the base style was allowed to be the same as the deriving style, it would lead to an endless recursion loop!
        {
            applyStyle(styleDefinition.baseStyle.value());
        }
    }
    styleDefinition.applyFunction();
    m_currentStyle = style;
}

GuiStyle InterfaceStyleHandler::getDefaultStyle() {
    return (GuiStyle)0;
}

GuiStyle InterfaceStyleHandler::getCurrentStyle() const {
    return m_currentStyle;
}

void InterfaceStyleHandler::setFontSize(FontSize fontSize) {
    currentFontSize = fontSize;
}

FontSize InterfaceStyleHandler::getDefaultFontSize() {
    return FontSize::Normal;
}

FontSize InterfaceStyleHandler::getFontSize() {
    return currentFontSize;
}

ImFont* InterfaceStyleHandler::getFontData(FontSize fontSize) {
    if (loadedFonts.contains(fontSize) == false) {
        return nullptr;
    }

    return loadedFonts.at(fontSize);
}