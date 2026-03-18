#include "style_io.h"
#include <format>
#include <iostream>

namespace fs = std::filesystem;

StyleIO::StyleIO(fs::path stylesDir) : m_stylesDir(stylesDir), m_converter() {
}

std::map<std::string, GuiStyleDefinition> StyleIO::readAllStyles() {
    std::map<std::string, GuiStyleDefinition> styles = {};

    if (fs::exists(m_stylesDir) == false) {
        std::cout << std::format("StyleIO::readAllStyles(): Styles directory '{}' does not exist.", m_stylesDir.string())
                  << std::endl;
        return {};
    }

    for (const auto& entry : fs::directory_iterator(m_stylesDir)) {
        // Skip non-regular files
        if (entry.is_regular_file() == false) {
            std::cout << "StyleIO::readAllStyles(): Skipped file " << entry.path().string()
                      << " (entry.is_regular_file() == false)" << std::endl;
            continue;
        }
        // Skip invalid style files
        if (m_converter.isValidStyleFile(entry.path()) == false) {
            std::cout << "StyleIO::readAllStyles(): Skipped file " << entry.path().string() << " (not a valid style file)"
                      << std::endl;
            continue;
        }

        auto loadedStyle = readStyle(entry.path());

        if (loadedStyle.has_value()) {
            styles.insert({loadedStyle->name, loadedStyle.value()});
        }
    }

    return styles;
}

std::optional<GuiStyleDefinition> StyleIO::readStyle(const fs::path& path) {
    if (fs::exists(path) == false) {
        std::cout << std::format("StyleIO::readStyle(): Tried to read style at path to non-existant file: '{}'", path.string())
                  << std::endl;
        return std::nullopt;
    }
    if (m_converter.isValidStyleFile(path.string().c_str()) == false) {
        std::cout << std::format("StyleIO::readStyle(): Tried to read invalid style file at path: '{}'", path.string())
                  << std::endl;
        return std::nullopt;
    }

    auto loadedStyle = m_converter.readStyle(path);
    return loadedStyle;
}