#pragma once

#include <map>
#include <set>
#include <stack>
#include <optional>
#include "gui_constants.h"
#include "gui_style_definition.h"

struct GuiStyleTransition {
        float progress = 0.0f;
        ImVec4 previousColors[ImGuiCol_COUNT];
        ImVec4 newColors[ImGuiCol_COUNT];
};

inline const std::string DEFAULT_STYLE_NAME_LIGHT = "default_light";
inline const std::string DEFAULT_STYLE_NAME_DARK = "default_dark";

class InterfaceStyleHandler {
    private:
        static std::map<FontSize, ImFont*> loadedFonts;
        static std::map<std::string, GuiStyleDefinition> styleDefinitions;
        static FontSize currentFontSize;
        const float TRANSITION_TIME_SECS = 2.2f;
        GuiStyleDefinition m_currentStyle;
        bool m_transitioningBetweenStyles = false;
        GuiStyleTransition m_transitionInfo;

        // Pass in an empty stack and an empty set.
        // The result will be stored in the stack (from lowest base style to the provided style).
        void getStylesToApplyRecursive(const GuiStyleDefinition& style,
                                       std::stack<GuiStyleDefinition>& styleStack,
                                       std::set<std::string>& styleNameHistory) const;
        void beginStyleTransition();

    public:
        // Pass style definitions
        static void setStyleDefinitions(const std::map<std::string, GuiStyleDefinition>& styles);
        // Get a map of style definition -> style name
        static std::map<GuiStyleDefinition, const char*> getStyleDefinitionToName();
        // Applies the style with the given name
        void applyStyle(const GuiStyleDefinition& style);
        // Returns the style definition of a default style or an empty style definition.
        static GuiStyleDefinition getDefaultStyle();
        GuiStyleDefinition getCurrentStyle() const;
        // Get a style by its name if a definition exists for it
        static std::optional<GuiStyleDefinition> getStyle(const std::string& styleName);

        void loadFontSizes(const char* fontPath);
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