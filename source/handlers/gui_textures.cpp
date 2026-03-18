#include "gui_textures.h"
#include <iostream>
#include <format>

namespace fs = std::filesystem;

GuiTextureInfo::GuiTextureInfo() {
}

GuiTextureInfo::GuiTextureInfo(const Texture& texture) {
    ID = texture.getID();
    ImID = (ImTextureID)(intptr_t)texture.getID();
    width = static_cast<unsigned int>(texture.getWidth());
    height = static_cast<unsigned int>(texture.getHeight());
}

GuiTextures::GuiTextures(TextureLoader& textureLoader) : m_textureLoader(textureLoader) {
    m_guiTextureFolder = m_textureLoader.getRelativePathFromTextureFolder("/gui/");
}

std::filesystem::path GuiTextures::getGuiTextureFolderPath() const {
    return m_guiTextureFolder;
}

bool GuiTextures::exists(const std::string& guiRelativePath, GuiTextureInfo& outTexture) {
    // If the texture has been loaded already, give it and return true
    if (m_cachedTextures.contains(guiRelativePath)) {
        outTexture = m_cachedTextures.at(guiRelativePath);
        return true;
    }

    fs::path fullPath = m_guiTextureFolder;
    fullPath.append(guiRelativePath);
    auto texturePath = m_textureLoader.existsTextureAtPath(fullPath);

    // A texture was found at the path with some extension.
    if (texturePath.has_value()) {
        auto texture = m_textureLoader.loadTexture(texturePath.value());
        if (texture.has_value()) {
            GuiTextureInfo texInfo = GuiTextureInfo(texture.value());
            m_cachedTextures.insert({guiRelativePath, texInfo});
            outTexture = GuiTextureInfo(texInfo);
            return true;
        }
    }

    // Output the missing texture if there was no other result.
    outTexture = GuiTextureInfo(m_textureLoader.getMissingTexture());
    return false;
}

GuiTextureInfo GuiTextures::getOrLoad(const std::string& guiRelativePath) {
    GuiTextureInfo textureInfo;
    if (!exists(guiRelativePath, textureInfo)) {
        std::cout
            << std::format(
                   "GuiTextures::getOrLoad(): Texture not found at gui relative path '{}'. Returning missing texture '{}'.",
                   guiRelativePath.c_str(),
                   textureInfo.ID)
            << std::endl;
    }
    return textureInfo;
}