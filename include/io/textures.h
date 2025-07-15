#pragma once

extern "C" {
#include <glad.h>
}

#include <string>
#include <map>
#include <filesystem>
#include <optional>

enum IMAGE_FORMAT {
    FORMAT_PNG,
    FORMAT_JPG,
    FORMAT_NONE
};

class Texture {
    private:
        GLuint m_ID;
        int m_width;
        int m_height;

    public:
        Texture(GLuint ID, int width, int height) : m_ID(ID), m_width(width), m_height(height) {
        }

        GLuint getID() const {
            return m_ID;
        }
        int getWidth() const {
            return m_width;
        }
        int getHeight() const {
            return m_height;
        }
};

class TextureLoader {
    private:
        struct {
                size_t size;
                size_t width;
                size_t height;
                IMAGE_FORMAT imageFormat;
                const unsigned char data[145];
        } m_missingTextureData = {
            145, 16, 16, FORMAT_PNG, {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44,
                                      0x52, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x10, 0x04, 0x03, 0x00, 0x00, 0x00, 0xed,
                                      0xdd, 0xe2, 0x52, 0x00, 0x00, 0x00, 0x01, 0x73, 0x52, 0x47, 0x42, 0x00, 0xae, 0xce, 0x1c,
                                      0xe9, 0x00, 0x00, 0x00, 0x04, 0x67, 0x41, 0x4d, 0x41, 0x00, 0x00, 0xb1, 0x8f, 0x0b, 0xfc,
                                      0x61, 0x05, 0x00, 0x00, 0x00, 0x06, 0x50, 0x4c, 0x54, 0x45, 0x00, 0x00, 0x00, 0xf4, 0xa8,
                                      0xff, 0x5c, 0x76, 0xbb, 0xaf, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00,
                                      0x0e, 0xc2, 0x00, 0x00, 0x0e, 0xc2, 0x01, 0x15, 0x28, 0x4a, 0x80, 0x00, 0x00, 0x00, 0x14,
                                      0x49, 0x44, 0x41, 0x54, 0x18, 0xd3, 0x63, 0x00, 0x01, 0x41, 0x20, 0xa0, 0x94, 0x41, 0x0d,
                                      0x33, 0x18, 0x18, 0x18, 0x00, 0x32, 0x9f, 0x04, 0x41, 0xf0, 0x11, 0x00, 0x81, 0x00, 0x00,
                                      0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82}};

        Texture m_missingTexture = Texture(0, 0, 0);

        std::map<IMAGE_FORMAT, const char*> m_formatExtensions = {{FORMAT_PNG, ".png"}, {FORMAT_JPG, ".jpg"}};

        IMAGE_FORMAT extensionToFormat(const char* ext) const;
        void createTexture(IMAGE_FORMAT imageFormat,
                           GLenum target,
                           GLint level,
                           GLsizei width,
                           GLsizei height,
                           GLint border,
                           GLenum type,
                           const void* pixels) const;
        GLuint createTextureFromData(IMAGE_FORMAT imageFormat,
                                     GLenum target,
                                     GLint level,
                                     GLsizei width,
                                     GLsizei height,
                                     GLint border,
                                     GLenum type,
                                     const unsigned char* data,
                                     size_t length) const;

    public:
        inline static const std::string textureFolder = "./textures/";

        void init();

        // Returns the path that a texture was found at or std::nullopt if not.
        // The file name can contain a path or only the filename.
        // If it contains an extension, that extension will be used.
        // If it doesn't contain an extension, every supported extension will be tried until a file is found.
        std::optional<std::filesystem::path> existsTextureAtPath(std::filesystem::path path) const;

        std::filesystem::path getRelativePathFromTextureFolder(const std::string& relativePath) const;

        Texture getMissingTexture() const;
        // Load a texture.
        // Returns a Texture containing its information when successfully loaded.
        // Returns std::nullopt if there was a problem loading the texture.
        std::optional<Texture> loadTexture(const std::filesystem::path& path, bool flip = false);
        // Load a texture and get its raw data.
        // It is up to you to delete the pointer.
        unsigned char* loadTextureData(const std::filesystem::path& path,
                                       int* width,
                                       int* height,
                                       GLuint* ID = nullptr,
                                       bool bind = true,
                                       bool flip = false);
};