#include "gui_textures.h"

namespace fs = std::filesystem;

GuiTextures::GuiTextures(TextureLoader& textureLoader) : m_textureLoader(textureLoader)
{
    m_guiTextureFolder = m_textureLoader.getRelativePathFromTextureFolder("/gui/");
}

GLuint GuiTextures::getOrLoad(const std::string& guiRelativePath)
{
    // If the texture has been loaded already, return it
    if (m_textureIDs.contains(guiRelativePath))
    {
        return m_textureIDs.at(guiRelativePath);
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
        return ID;
    }

    printf("GuiTextures::getOrLoad(%s): Texture not found at path. Returning missing texture %d\n", guiRelativePath.c_str(), m_textureLoader.getMissingTexture());
    // Return the missing texture if there was no other result.
    return m_textureLoader.getMissingTexture();
}