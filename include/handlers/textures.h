#pragma once

extern "C" {
#include <glad.h>
}

#include <string>
#include <map>

enum IMAGE_FORMAT
{
    FORMAT_PNG,
    FORMAT_JPG,
    FORMAT_NONE
};

class TextureLoader
{
    private:
        std::map<IMAGE_FORMAT, const char*> m_formatExtensions =
        {
            {FORMAT_PNG, ".png"},
            {FORMAT_JPG, ".jpg"}
        };

        IMAGE_FORMAT extensionToFormat(const char* ext) const;
        void createTexture(IMAGE_FORMAT imageFormat, GLenum target, GLint level, GLsizei width, GLsizei height, GLint border, GLenum type, const void *pixels);
	public:
		static inline const std::string textureFolder = "textures/";

		GLuint createEmptyTexture(int width, int height);
		unsigned char* loadTextureData(const char* fileName, int* width, int* height, std::string directory = textureFolder, bool flip = true, unsigned int* ID = nullptr, bool bind = false);
};
