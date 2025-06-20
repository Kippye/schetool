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

class TextureLoader {
    private:
        static const unsigned char missing[];

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

        GLuint m_missingTextureID = 0;

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

        GLuint getMissingTexture() const;
        GLuint createEmptyTexture(int width, int height);
        unsigned char* loadTextureData(const std::filesystem::path& path,
                                       int* width,
                                       int* height,
                                       GLuint* ID = nullptr,
                                       bool bind = true,
                                       bool flip = false);
};

inline const unsigned char TextureLoader::missing[] = {
    // 0x01, 0x01, 0x00, 0xff, 0x00, 0x01, 0x00, 0xff, 0x01, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff,
    // 0x00, 0x00, 0x00, 0xff, 0x01, 0x00, 0x00, 0xff, 0x01, 0x00, 0x00, 0xff, 0x01, 0x00, 0x00, 0xff,
    // 0xf4, 0xa8, 0xfe, 0xff, 0xf5, 0xa8, 0xff, 0xff, 0xf5, 0xa8, 0xff, 0xff, 0xf4, 0xa9, 0xff, 0xff,
    // 0xf4, 0xa8, 0xfe, 0xff, 0xf4, 0xa9, 0xff, 0xff, 0xf4, 0xa8, 0xfe, 0xff, 0xf4, 0xa8, 0xff, 0xff,
    // 0x00, 0x01, 0x01, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x01, 0x01, 0xff, 0x00, 0x00, 0x01, 0xff,
    // 0x00, 0x00, 0x01, 0xff, 0x01, 0x00, 0x00, 0xff, 0x00, 0x00, 0x01, 0xff, 0x00, 0x00, 0x00, 0xff,
    // 0xf4, 0xa9, 0xfe, 0xff, 0xf4, 0xa8, 0xfe, 0xff, 0xf5, 0xa8, 0xff, 0xff, 0xf5, 0xa9, 0xff, 0xff,
    // 0xf4, 0xa9, 0xff, 0xff, 0xf4, 0xa9, 0xff, 0xff, 0xf4, 0xa8, 0xfe, 0xff, 0xf4, 0xa8, 0xff, 0xff,
    // 0x00, 0x01, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x01, 0x01, 0xff, 0x01, 0x00, 0x01, 0xff,
    // 0x00, 0x00, 0x00, 0xff, 0x00, 0x01, 0x00, 0xff, 0x00, 0x00, 0x01, 0xff, 0x00, 0x00, 0x00, 0xff,
    // 0xf4, 0xa8, 0xff, 0xff, 0xf4, 0xa9, 0xfe, 0xff, 0xf4, 0xa8, 0xff, 0xff, 0xf4, 0xa8, 0xff, 0xff,
    // 0xf5, 0xa8, 0xff, 0xff, 0xf4, 0xa8, 0xff, 0xff, 0xf5, 0xa8, 0xff, 0xff, 0xf4, 0xa9, 0xff, 0xff,
    // 0x00, 0x01, 0x00, 0xff, 0x01, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x01, 0x00, 0xff,
    // 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x01, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff,
    // 0xf4, 0xa8, 0xff, 0xff, 0xf5, 0xa8, 0xfe, 0xff, 0xf4, 0xa8, 0xff, 0xff, 0xf4, 0xa8, 0xff, 0xff,
    // 0xf4, 0xa8, 0xfe, 0xff, 0xf4, 0xa8, 0xfe, 0xff, 0xf5, 0xa8, 0xff, 0xff, 0xf4, 0xa8, 0xff, 0xff,
    // 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x01, 0xff, 0x00, 0x01, 0x00, 0xff,
    // 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x01, 0xff, 0x01, 0x01, 0x00, 0xff, 0x01, 0x01, 0x01, 0xff,
    // 0xf4, 0xa8, 0xfe, 0xff, 0xf5, 0xa9, 0xff, 0xff, 0xf4, 0xa9, 0xff, 0xff, 0xf5, 0xa8, 0xff, 0xff,
    // 0xf4, 0xa8, 0xff, 0xff, 0xf5, 0xa9, 0xff, 0xff, 0xf4, 0xa8, 0xfe, 0xff, 0xf5, 0xa8, 0xff, 0xff,
    // 0x01, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x01, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff,
    // 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x01, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x01, 0x00, 0xff,
    // 0xf4, 0xa8, 0xff, 0xff, 0xf4, 0xa9, 0xff, 0xff, 0xf4, 0xa9, 0xff, 0xff, 0xf5, 0xa8, 0xfe, 0xff,
    // 0xf4, 0xa8, 0xff, 0xff, 0xf5, 0xa8, 0xff, 0xff, 0xf5, 0xa8, 0xff, 0xff, 0xf4, 0xa8, 0xff, 0xff,
    // 0x00, 0x00, 0x00, 0xff, 0x01, 0x00, 0x00, 0xff, 0x00, 0x01, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff,
    // 0x01, 0x00, 0x01, 0xff, 0x01, 0x00, 0x01, 0xff, 0x00, 0x01, 0x00, 0xff, 0x01, 0x00, 0x00, 0xff,
    // 0xf5, 0xa8, 0xff, 0xff, 0xf5, 0xa8, 0xff, 0xff, 0xf4, 0xa8, 0xfe, 0xff, 0xf4, 0xa8, 0xff, 0xff,
    // 0xf4, 0xa8, 0xff, 0xff, 0xf5, 0xa9, 0xff, 0xff, 0xf4, 0xa9, 0xff, 0xff, 0xf4, 0xa9, 0xff, 0xff,
    // 0x00, 0x00, 0x01, 0xff, 0x00, 0x00, 0x00, 0xff, 0x01, 0x00, 0x01, 0xff, 0x00, 0x00, 0x00, 0xff,
    // 0x00, 0x01, 0x00, 0xff, 0x00, 0x00, 0x01, 0xff, 0x01, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff,
    // 0xf5, 0xa8, 0xff, 0xff, 0xf4, 0xa8, 0xfe, 0xff, 0xf4, 0xa8, 0xff, 0xff, 0xf5, 0xa8, 0xfe, 0xff,
    // 0xf4, 0xa8, 0xff, 0xff, 0xf5, 0xa8, 0xff, 0xff, 0xf4, 0xa8, 0xff, 0xff, 0xf5, 0xa8, 0xfe, 0xff,
    // 0xf5, 0xa8, 0xff, 0xff, 0xf4, 0xa8, 0xfe, 0xff, 0xf4, 0xa8, 0xff, 0xff, 0xf4, 0xa8, 0xff, 0xff,
    // 0xf4, 0xa9, 0xff, 0xff, 0xf4, 0xa8, 0xff, 0xff, 0xf4, 0xa8, 0xfe, 0xff, 0xf4, 0xa8, 0xff, 0xff,
    // 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x01, 0x00, 0x00, 0xff, 0x00, 0x01, 0x00, 0xff,
    // 0x00, 0x01, 0x01, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x01, 0xff, 0x00, 0x00, 0x01, 0xff,
    // 0xf4, 0xa8, 0xff, 0xff, 0xf5, 0xa9, 0xff, 0xff, 0xf4, 0xa8, 0xff, 0xff, 0xf4, 0xa8, 0xfe, 0xff,
    // 0xf4, 0xa8, 0xfe, 0xff, 0xf4, 0xa8, 0xff, 0xff, 0xf5, 0xa8, 0xfe, 0xff, 0xf4, 0xa8, 0xff, 0xff,
    // 0x00, 0x00, 0x00, 0xff, 0x01, 0x01, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x01, 0x01, 0x00, 0xff,
    // 0x00, 0x01, 0x00, 0xff, 0x01, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x01, 0xff,
    // 0xf5, 0xa8, 0xff, 0xff, 0xf4, 0xa8, 0xff, 0xff, 0xf4, 0xa9, 0xff, 0xff, 0xf5, 0xa8, 0xfe, 0xff,
    // 0xf5, 0xa9, 0xff, 0xff, 0xf5, 0xa8, 0xff, 0xff, 0xf4, 0xa8, 0xfe, 0xff, 0xf4, 0xa8, 0xff, 0xff,
    // 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x01, 0x00, 0xff, 0x01, 0x00, 0x00, 0xff,
    // 0x01, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x01, 0x00, 0xff,
    // 0xf5, 0xa8, 0xff, 0xff, 0xf4, 0xa9, 0xff, 0xff, 0xf5, 0xa9, 0xff, 0xff, 0xf5, 0xa8, 0xfe, 0xff,
    // 0xf5, 0xa8, 0xff, 0xff, 0xf5, 0xa8, 0xff, 0xff, 0xf4, 0xa8, 0xff, 0xff, 0xf4, 0xa8, 0xfe, 0xff,
    // 0x01, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x01, 0x01, 0xff, 0x00, 0x00, 0x00, 0xff,
    // 0x01, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x01, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff,
    // 0xf4, 0xa9, 0xff, 0xff, 0xf4, 0xa8, 0xfe, 0xff, 0xf5, 0xa9, 0xfe, 0xff, 0xf4, 0xa8, 0xff, 0xff,
    // 0xf4, 0xa8, 0xff, 0xff, 0xf4, 0xa8, 0xff, 0xff, 0xf4, 0xa8, 0xff, 0xff, 0xf4, 0xa9, 0xff, 0xff,
    // 0x01, 0x01, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x01, 0x00, 0x00, 0xff, 0x01, 0x01, 0x00, 0xff,
    // 0x00, 0x01, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x01, 0xff,
    // 0xf4, 0xa8, 0xff, 0xff, 0xf4, 0xa8, 0xff, 0xff, 0xf4, 0xa8, 0xff, 0xff, 0xf4, 0xa8, 0xff, 0xff,
    // 0xf5, 0xa8, 0xff, 0xff, 0xf4, 0xa8, 0xff, 0xff, 0xf4, 0xa8, 0xfe, 0xff, 0xf4, 0xa8, 0xff, 0xff,
    // 0x00, 0x00, 0x00, 0xff, 0x01, 0x01, 0x00, 0xff, 0x00, 0x01, 0x01, 0xff, 0x00, 0x00, 0x00, 0xff,
    // 0x00, 0x01, 0x00, 0xff, 0x00, 0x00, 0x01, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x01, 0x00, 0xff,
    // 0xf5, 0xa9, 0xff, 0xff, 0xf4, 0xa8, 0xfe, 0xff, 0xf5, 0xa8, 0xff, 0xff, 0xf5, 0xa8, 0xff, 0xff,
    // 0xf4, 0xa8, 0xfe, 0xff, 0xf4, 0xa8, 0xfe, 0xff, 0xf4, 0xa8, 0xff, 0xff, 0xf5, 0xa9, 0xff, 0xff,
    // 0x01, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x01, 0x00, 0x01, 0xff, 0x00, 0x01, 0x00, 0xff,
    // 0x01, 0x00, 0x01, 0xff, 0x00, 0x00, 0x01, 0xff, 0x00, 0x01, 0x00, 0xff, 0x00, 0x00, 0x01, 0xff,
    // 0xf5, 0xa8, 0xfe, 0xff, 0xf4, 0xa8, 0xff, 0xff, 0xf5, 0xa9, 0xfe, 0xff, 0xf4, 0xa9, 0xff, 0xff,
    // 0xf5, 0xa8, 0xff, 0xff, 0xf5, 0xa8, 0xff, 0xff, 0xf4, 0xa8, 0xff, 0xff, 0xf4, 0xa9, 0xfe, 0xff,
    // 0x00, 0x01, 0x00, 0xff, 0x01, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x01, 0x00, 0x00, 0xff,
    // 0x00, 0x00, 0x01, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0xff

    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x01, 0x00,
    0x00, 0x00, 0x01, 0x00, 0x04, 0x03, 0x00, 0x00, 0x00, 0xae, 0x5c, 0xb5, 0x55, 0x00, 0x00, 0x00, 0x01, 0x73, 0x52, 0x47,
    0x42, 0x00, 0xae, 0xce, 0x1c, 0xe9, 0x00, 0x00, 0x00, 0x04, 0x67, 0x41, 0x4d, 0x41, 0x00, 0x00, 0xb1, 0x8f, 0x0b, 0xfc,
    0x61, 0x05, 0x00, 0x00, 0x00, 0x06, 0x50, 0x4c, 0x54, 0x45, 0x00, 0x00, 0x00, 0xf4, 0xa8, 0xff, 0x5c, 0x76, 0xbb, 0xaf,
    0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00, 0x0e, 0xc3, 0x00, 0x00, 0x0e, 0xc3, 0x01, 0xc7, 0x6f, 0xa8,
    0x64, 0x00, 0x00, 0x00, 0xa3, 0x49, 0x44, 0x41, 0x54, 0x78, 0xda, 0xed, 0xcf, 0x31, 0x11, 0x00, 0x00, 0x0c, 0x02, 0x31,
    0x2c, 0xd4, 0xbf, 0xd9, 0x8a, 0x60, 0x60, 0xc9, 0x0b, 0xe0, 0x42, 0xda, 0xae, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x0f, 0xa8, 0x07,
    0xe6, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0xaa, 0x92, 0x07, 0x91, 0xd0, 0x40, 0x3d, 0x6c, 0x8a, 0x71, 0x7f, 0x00, 0x00, 0x00, 0x00,
    0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82};