#pragma once

#include "gui_style_definition.h"
#include "style_data_converter.h"
#include <filesystem>

class StyleIO {
    private:
        StyleDataConverter m_converter;
        std::filesystem::path m_stylesDir = std::filesystem::path();

    public:
        StyleIO(std::filesystem::path dataDir);

        // Read every valid style file in the styles directory.
        // Return a map of styleName -> styleDefinition
        std::map<std::string, GuiStyleDefinition> readAllStyles();
        // Read a single style file from the styles directory.
        // Return the style if read successfully.
        // Return std::nullopt if reading the style failed.
        std::optional<GuiStyleDefinition> readStyle(const std::filesystem::path& path);
};