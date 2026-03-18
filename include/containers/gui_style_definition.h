#pragma once

#include "imgui.h"
#include <string>
#include <map>
#include <optional>

// Container for a style definition.
// Each style has a name (used only for display in the style selector or elsewhere) and a base style.
// Each style contains a map of colors and style var values for the style.
// When applying a style, the base style will be applied first and then the deriving style's modifications added on top.
struct GuiStyleDefinition {
        std::string name = "";
        std::string displayName = "";
        std::optional<std::string> baseStyleName = std::nullopt;
        std::map<ImGuiCol, ImVec4> styleColors = {};
        std::map<ImGuiStyleVar, float> styleVars = {};
        std::map<ImGuiStyleVar, ImVec2> styleVarsVector = {};

        bool operator==(const GuiStyleDefinition& other) const {
            return name == other.name;
        }

        bool operator!=(const GuiStyleDefinition& other) const {
            return name != other.name;
        }

        friend bool operator<(const GuiStyleDefinition& left, const GuiStyleDefinition& right) {
            return left.name < right.name;
        }

        friend bool operator>(const GuiStyleDefinition& left, const GuiStyleDefinition& right) {
            return left.name > right.name;
        }
};