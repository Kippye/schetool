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
        const GuiStyle m_defaultStyle = (GuiStyle)0;
        GuiStyle m_currentStyle;
        FontScale m_currentFontScale = FontScale::Normal;
    public:
        static const std::map<GuiStyle, GuiStyleDefinition> styleDefinitions;
        void applyStyle(GuiStyle style);
        GuiStyle getCurrentStyle() const;
        GuiStyle getDefaultStyle() const;
        FontScale getFontScale() const;
        void setFontScale(FontScale fontScale);
};