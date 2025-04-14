#include "gui_textures.h"
#include <iostream>
#include <format>

namespace fs = std::filesystem;

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
        int w, h;
        GLuint ID;
        m_textureLoader.loadTextureData(texturePath.value(), &w, &h, &ID);
        GuiTextureInfo texInfo =
            GuiTextureInfo{ID, (ImTextureID)(intptr_t)ID, static_cast<unsigned int>(w), static_cast<unsigned int>(h)};
        m_cachedTextures.insert({guiRelativePath, texInfo});
        outTexture = GuiTextureInfo(texInfo);
        return true;
    }

    // Output the missing texture if there was no other result.
    outTexture = {m_textureLoader.getMissingTexture(), (ImTextureID)(intptr_t)m_textureLoader.getMissingTexture(), 16, 16};
    return false;
}

GuiTextureInfo GuiTextures::getOrLoad(const std::string& guiRelativePath) {
    GuiTextureInfo textureInfo;
    if (!exists(guiRelativePath, textureInfo)) {
        std::cout << std::format(
                         "GuiTextures::exists(): Texture not found at gui relative path '{}'. Returning missing texture '{}'.",
                         guiRelativePath.c_str(),
                         textureInfo.ID)
                  << std::endl;
    }
    return textureInfo;
}