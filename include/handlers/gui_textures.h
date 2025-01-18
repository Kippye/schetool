#pragma once

#include "textures.h"
#include "imgui.h"

struct GuiTextureInfo
{
    GLuint ID;
    ImTextureID ImID;
    unsigned int width, height;
};

// Stores and loads textures for interface elements
class GuiTextures
{
    private:
        std::filesystem::path m_guiTextureFolder;
        TextureLoader& m_textureLoader;
        // Texture names mapped to their texture IDs
        std::map<std::string, GuiTextureInfo> m_cachedTextures = {};
    public:
        GuiTextures() = delete;
        GuiTextures(TextureLoader& textureLoader);
        std::filesystem::path getGuiTextureFolderPath() const;
        // DEPRECATED. Transition existing code to use the exists() function instead.
        // The file name can contain a relative path from the gui texture folder or only the filename.
        // If it contains an extension, that extension will be used.
        // If it doesn't contain an extension, every supported extension will be tried until a file is found.
        GLuint getOrLoad(const std::string& guiRelativePath);
        // Do the same as getOrLoad, but returning whether the texture exists.
        // The provided GLuint reference will be set to the texture if it is found.
        // Does not print an error if the texture is not found.
        bool exists(const std::string& guiRelativePath, GuiTextureInfo& textureOut);
};