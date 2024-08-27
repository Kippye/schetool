#pragma once

#include "textures.h"

// Stores and loads textures for interface elements
class GuiTextures
{
    private:
        std::filesystem::path m_guiTextureFolder;
        TextureLoader& m_textureLoader;
        // Texture names mapped to their texture IDs
        std::map<std::string, GLuint> m_textureIDs = {};
    public:
        GuiTextures() = delete;
        GuiTextures(TextureLoader& textureLoader);
        // The file name can contain a relative path from the gui texture folder or only the filename.
        // If it contains an extension, that extension will be used.
        // If it doesn't contain an extension, every supported extension will be tried until a file is found.
        GLuint getOrLoad(const std::string& guiRelativePath);
};