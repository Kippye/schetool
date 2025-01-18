#include "gui_textures.h"
#include <iostream>
#include <format>

namespace fs = std::filesystem;

GuiTextures::GuiTextures(TextureLoader& textureLoader) : m_textureLoader(textureLoader)
{
    m_guiTextureFolder = m_textureLoader.getRelativePathFromTextureFolder("/gui/");
}

GLuint GuiTextures::getOrLoad(const std::string& guiRelativePath)
{
    GLuint texture;

    if (exists(guiRelativePath, texture))
    {
        return texture;
    }
    else
    {
        std::cout << std::format("GuiTextures::getOrLoad(): Texture not found at gui relative path '{}'. Returning missing texture {}", guiRelativePath.c_str(), m_textureLoader.getMissingTexture()) << std::endl;
        // Return the missing texture if there was no other result.
        return texture;
    }
}

bool GuiTextures::exists(const std::string& guiRelativePath, GLuint& outTexture)
{
    // If the texture has been loaded already, give it and return true
    if (m_textureIDs.contains(guiRelativePath))
    {
        outTexture = m_textureIDs.at(guiRelativePath);
        return true;
    }
    
    fs::path fullPath = m_guiTextureFolder;
    fullPath.append(guiRelativePath);
    auto texturePath = m_textureLoader.existsTextureAtPath(fullPath);

    // A texture was found at the path with some extension.
    if (texturePath.has_value())
    {
        int w, h;
        GLuint ID;
        m_textureLoader.loadTextureData(texturePath.value(), &w, &h, &ID);
        m_textureIDs.insert({guiRelativePath, ID});
        outTexture = ID;
        return true;
    }

    // Output the missing texture if there was no other result.
    outTexture = m_textureLoader.getMissingTexture();
    return false;
}