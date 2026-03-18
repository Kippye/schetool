#include "interface_style.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <iostream>
#include <format>

std::map<FontSize, ImFont*> InterfaceStyleHandler::loadedFonts = {};
std::map<std::string, GuiStyleDefinition> InterfaceStyleHandler::styleDefinitions = {};

// NOTE: ImGuiCol_CheckMark is also used to color EVERY texture button that should be colored according to the style
// Currently, this mostly means gui_templates::ImageButtonStyleColored()
// NOTE: It is probably a good idea to define EVERY color that is defined in other styles (or use a base style)
// Otherwise there might be leftover colors when switching between styles

FontSize InterfaceStyleHandler::currentFontSize = FontSize::Normal;

void InterfaceStyleHandler::setStyleDefinitions(const std::map<std::string, GuiStyleDefinition>& styles) {
    styleDefinitions = styles;
}  // static

// Get a map of style definition -> style name
std::map<GuiStyleDefinition, const char*> InterfaceStyleHandler::getStyleDefinitionToName() {
    std::map<GuiStyleDefinition, const char*> definitionToName = {};

    for (auto& [name, definition] : styleDefinitions) {
        definitionToName.insert({definition, definition.displayName.c_str()});
    }
    return definitionToName;
}  // static

void InterfaceStyleHandler::getStylesToApplyRecursive(const GuiStyleDefinition& styleDefinition,
                                                      std::stack<GuiStyleDefinition>& styleStack,
                                                      std::set<std::string>& styleNameHistory) const {
    styleStack.push(styleDefinition);
    styleNameHistory.insert(styleDefinition.name);

    // The style has a base style and it hasn't been added already
    if (styleDefinition.baseStyleName.has_value() && !styleNameHistory.contains(styleDefinition.baseStyleName.value())) {
        auto baseStyle = getStyle(styleDefinition.baseStyleName.value());
        if (baseStyle.has_value()) {
            getStylesToApplyRecursive(baseStyle.value(), styleStack, styleNameHistory);
        } else {
            std::cout << std::format("Warning: Missing base style '{}' for style '{}'!",
                                     styleDefinition.baseStyleName.value(),
                                     styleDefinition.name)
                      << std::endl;
        }
    }
}

void InterfaceStyleHandler::beginStyleTransition() {
    m_transitionInfo.progress = 0.0f;
    m_transitioningBetweenStyles = true;
}

void InterfaceStyleHandler::applyStyle(const GuiStyleDefinition& styleDefinition) {
    // Copy previous colors to transition info
    std::copy(
        std::begin(ImGui::GetStyle().Colors), std::end(ImGui::GetStyle().Colors), std::begin(m_transitionInfo.previousColors));

    std::stack<GuiStyleDefinition> stylesToApply = {};
    std::set<std::string> _styleNameHistory = {};
    getStylesToApplyRecursive(styleDefinition, stylesToApply, _styleNameHistory);

    while (!stylesToApply.empty()) {
        const GuiStyleDefinition& style = stylesToApply.top();

        for (auto& [colorEnum, color] : style.styleColors) {
            ImGui::GetStyle().Colors[colorEnum] = color;
        }

        for (auto& [varEnum, varValue] : style.styleVars) {
            const ImGuiStyleVarInfo* varInfo = ImGui::GetStyleVarInfo(varEnum);
            if (varInfo->DataType == ImGuiDataType_Float && varInfo->Count == 1) {
                *((float*)varInfo->GetVarPtr(&ImGui::GetStyle())) = varValue;
            }
        }

        for (auto& [varEnum, varValue] : style.styleVarsVector) {
            const ImGuiStyleVarInfo* varInfo = ImGui::GetStyleVarInfo(varEnum);
            if (varInfo->DataType == ImGuiDataType_Float && varInfo->Count == 2) {
                *((ImVec2*)varInfo->GetVarPtr(&ImGui::GetStyle())) = varValue;
            }
        }

        stylesToApply.pop();
    }

    // Copy the new colors after style application to transition info
    std::copy(std::begin(ImGui::GetStyle().Colors), std::end(ImGui::GetStyle().Colors), std::begin(m_transitionInfo.newColors));
    // Apply the previous colors again so the transition can start
    std::copy(std::begin(m_transitionInfo.previousColors),
              std::end(m_transitionInfo.previousColors),
              std::begin(ImGui::GetStyle().Colors));
    beginStyleTransition();

    m_currentStyle = styleDefinition;
}

GuiStyleDefinition InterfaceStyleHandler::getDefaultStyle() {
    GuiStyleDefinition defaultStyle = GuiStyleDefinition();

    if (styleDefinitions.contains(DEFAULT_STYLE_NAME_LIGHT)) {
        defaultStyle = styleDefinitions.at(DEFAULT_STYLE_NAME_LIGHT);
    } else if (styleDefinitions.contains(DEFAULT_STYLE_NAME_DARK)) {
        defaultStyle = styleDefinitions.at(DEFAULT_STYLE_NAME_DARK);
    }

    return defaultStyle;
}

GuiStyleDefinition InterfaceStyleHandler::getCurrentStyle() const {
    return m_currentStyle;
}

std::optional<GuiStyleDefinition> InterfaceStyleHandler::getStyle(const std::string& styleName) {
    return styleDefinitions.contains(styleName) ? std::optional(styleDefinitions.at(styleName)) : std::nullopt;
}

void InterfaceStyleHandler::loadFontSizes(const char* fontPath) {
    for (FontSize fontSize = FontSize::Small; fontSize <= FontSize::Huge; fontSize = (FontSize)((int)fontSize + 1)) {
        loadedFonts.insert_or_assign(
            fontSize, ImGui::GetIO().Fonts->AddFontFromFileTTF(fontPath, gui_fonts::fontSizePixelSizes.at(fontSize)));
    }
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

void InterfaceStyleHandler::transitionStyle(float deltaTime) {
    if (m_transitioningBetweenStyles == false) {
        return;
    }

    auto imguiColors = ImGui::GetStyle().Colors;

    const float addition = deltaTime * 2.2f;
    if (m_transitionInfo.progress + (deltaTime * TRANSITION_TIME_SECS) >= 1.0f) {
        m_transitioningBetweenStyles = false;
    }
    m_transitionInfo.progress = std::min(m_transitionInfo.progress + (deltaTime * TRANSITION_TIME_SECS), 1.0f);
    for (size_t i = 0; i < std::size(m_transitionInfo.previousColors); i++) {
        imguiColors[i] = ImLerp(m_transitionInfo.previousColors[i], m_transitionInfo.newColors[i], m_transitionInfo.progress);
    }
}