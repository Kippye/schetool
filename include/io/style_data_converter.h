#pragma once

#include "gui_style_definition.h"
#include <string>
#include <filesystem>

class StyleDataConverter {
    private:
        std::string m_extension = ".json";
        std::optional<ImGuiCol> getAsImGuiCol(const std::string& name) const;
        std::optional<ImGuiStyleVar> getAsStyleVar(const std::string& name) const;

    public:
        // Get the file extension used by the StyleDataConverter.
        const std::string& getExtension() const;

        bool isValidStyleFile(const std::filesystem::path& path) const;
        // Read a style definition file from the given path, returning a GuiStyleDefinition if successful.
        std::optional<GuiStyleDefinition> readStyle(const std::filesystem::path& path);
};