#pragma once

#include <map>
#include <optional>
#include <functional>
#include "gui_constants.h"

// Container for a style definition.
// Each style has a name (used only for display in the style selector or elsewhere) and a base style.
// When applying a style, the base style will be applied first and then the deriving style's modifications added on top.
// The apply function applies the style when called.
struct GuiStyleDefinition
{
    const char* name;
    std::optional<GuiStyle> baseStyle;
    std::function<void()> applyFunction;
};

class InterfaceStyleHandler
{
    private:
        static std::map<FontSize, ImFont*> loadedFonts;
        const GuiStyle m_defaultStyle = (GuiStyle)0;
        GuiStyle m_currentStyle;
        static FontSize currentFontSize;
    public:
        static const std::map<GuiStyle, GuiStyleDefinition> styleDefinitions;
        void loadFontSizes(const char* fontPath);
        void applyStyle(GuiStyle style);
        GuiStyle getCurrentStyle() const;
        GuiStyle getDefaultStyle() const;
        // Changes the internal state of the current FontSize.
        // To actually apply the font size, push a font before drawing gui elements.
        void setFontSize(FontSize fontSize);
        static FontSize getFontSize();
        // Get the font data for the given size or nullptr if there is none.
        static ImFont* getFontData(FontSize fontSize);
};