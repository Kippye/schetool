#include "style_data_converter.h"
#include "nlohmann/json.hpp"
#include "gui_constants.h"
#include <fstream>
#include <iostream>
#include <format>

using json = nlohmann::json;

std::optional<ImGuiCol> StyleDataConverter::getAsImGuiCol(const std::string& name) const {
    for (ImGuiCol col = 0; col < ImGuiCol_COUNT; col++) {
        if (ImGui::GetStyleColorName(col) == name) {
            return col;
        }
    }
    return std::nullopt;
}

std::optional<ImGuiStyleVar> StyleDataConverter::getAsStyleVar(const std::string& name) const {
    return styleVarNameToEnum.contains(name) ? std::optional<ImGuiStyleVar>(styleVarNameToEnum.at(name)) : std::nullopt;
}

const std::string& StyleDataConverter::getExtension() const {
    return m_extension;
}

bool StyleDataConverter::isValidStyleFile(const std::filesystem::path& path) const {
    return strcmp(path.extension().string().c_str(), getExtension().c_str()) == 0;
}

ImVec4 getAsColor(const json& colorArray) {
    auto toFloat = [](const json& colorElement) -> float {
        if (colorElement.type() == json::value_t::number_float) {
            return colorElement;
        } else {  // Convert everything else from RGB int value to float in range 0.0f - 1.0f;
            return std::clamp(((float)colorElement) / 255, 0.0f, 1.0f);
        }
    };

    return ImVec4(toFloat(colorArray[0]), toFloat(colorArray[1]), toFloat(colorArray[2]), toFloat(colorArray[3]));
}

ImVec2 getAsImVec2(const json& arr) {
    return ImVec2(arr[0], arr[1]);
}

std::optional<GuiStyleDefinition> StyleDataConverter::readStyle(const std::filesystem::path& path) {
    std::ifstream f(path);
    json data = json::parse(f);
    GuiStyleDefinition styleDefinition = GuiStyleDefinition();

    if (data.contains("name")) {
        styleDefinition.name = data["name"];
    } else {
        return std::nullopt;
    }
    if (data.contains("displayName")) {
        styleDefinition.displayName = data["displayName"];
    } else {
        styleDefinition.displayName = styleDefinition.name;
    }
    if (data.contains("base")) {
        styleDefinition.baseStyleName = data["base"];
    }

    // Colors
    if (data.contains("colors")) {
        for (auto& [colorName, colorValue] : data["colors"].items()) {
            // Colors are represented as an array of R, G, B, A floats
            if (colorValue.type() == json::value_t::array && colorValue.size() == 4) {
                // Valid color enum name
                if (auto colorEnum = getAsImGuiCol(colorName)) {
                    styleDefinition.styleColors.insert_or_assign(colorEnum.value(), getAsColor(colorValue));
                } else {
                    std::cout << "StyleDataConverter::readStyle(): Invalid color name: " << colorName << std::endl;
                }
            } else {
                std::cout << "StyleDataConverter::readStyle(): Invalid color value for color: " << colorName << std::endl;
            }
        }
    }
    // Style vars
    if (data.contains("vars")) {
        for (auto& [varName, varValue] : data["vars"].items()) {
            // Valid style var enum name
            if (auto varEnum = getAsStyleVar(varName)) {
                switch (varValue.type()) {
                    case json::value_t::number_float:  // float
                        styleDefinition.styleVars.insert_or_assign(varEnum.value(), varValue);
                        break;
                    case json::value_t::array:  // Should be an ImVec2
                        if (varValue.size() == 2) {
                            styleDefinition.styleVarsVector.insert_or_assign(varEnum.value(), getAsImVec2(varValue));
                        }
                        break;
                    default:
                        std::cout << std::format(
                                         "StyleDataConverter::readStyle(): Invalid style var value type {} for style var {}",
                                         varValue.type_name(),
                                         varName)
                                  << std::endl;
                }
            } else {
                std::cout << "StyleDataConverter::readStyle(): Invalid style var name: " << varName << std::endl;
            }
        }
    }

    return styleDefinition;
}