#pragma once

#include <map>
#include <optional>
#include <functional>
#include "gui_constants.h"

// Container for a style definition.
// Each style has a name (used only for display in the style selector or elsewhere) and a base style.
// When applying a style, the base style will be applied first and then the deriving style's modifications added on top.
// The apply function applies the style when called.
struct GuiStyleDefinition {
        std::optional<GuiStyle> baseStyle;
        std::function<void()> applyFunction;
};

struct GuiStyleTransition {
        float progress = 0.0f;
        ImVec4 previousColors[ImGuiCol_COUNT];
        ImVec4 newColors[ImGuiCol_COUNT];
};

class InterfaceStyleHandler {
    private:
        static std::map<FontSize, ImFont*> loadedFonts;
        static FontSize currentFontSize;
        const float TRANSITION_TIME_SECS = 2.2f;
        GuiStyle m_currentStyle;
        bool m_transitioningBetweenStyles = false;
        GuiStyleTransition m_transitionInfo;

        void beginStyleTransition();

    public:
        static const std::map<GuiStyle, const char*> styleNames;
        static const std::map<GuiStyle, GuiStyleDefinition> styleDefinitions;
        void loadFontSizes(const char* fontPath);
        void applyStyle(GuiStyle style);
        static GuiStyle getDefaultStyle();
        GuiStyle getCurrentStyle() const;
        // Changes the internal state of the current FontSize.
        // To actually apply the font size, push a font before drawing gui elements.
        void setFontSize(FontSize fontSize);
        static FontSize getDefaultFontSize();
        static FontSize getFontSize();
        // Get the font data for the given size or nullptr if there is none.
        static ImFont* getFontData(FontSize fontSize);
        // Transition from the previous style's colors to the current style's colors using the given deltaTime.
        // Does nothing if a transition isn't active.
        void transitionStyle(float deltaTime);
};