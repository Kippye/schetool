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
        
        // Do the same as getOrLoad, but returning whether the texture exists.
        // The provided GLuint reference will be updated to the texture's information if it is found.
        // Does not print an error if the texture is not found.
        bool exists(const std::string& guiRelativePath, GuiTextureInfo& textureOut);
        // Less safe method of getting / loading a texture.
        // Returns a GuiTextureInfo with the texture if it is found.
        // Returns a GuiTextureInfo with the missing texture if the texture isn't found.
        // Prints an error if the texture is not found.
        GuiTextureInfo getOrLoad(const std::string& guiRelativePath);
};