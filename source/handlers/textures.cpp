#include <textures.h>
extern "C" {
#include <glad.h>
}
#include <GLFW/glfw3.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <filesystem>

namespace fs = std::filesystem;

#define DEBUG_TEXTURE_LOADING false

IMAGE_FORMAT TextureLoader::extensionToFormat(const char* ext) const
{
    for (const auto& formatExtensionPair : m_formatExtensions)
    {
        if (strcmp(ext, formatExtensionPair.second) == 0)
        {
            return formatExtensionPair.first;
        }
    }
    return FORMAT_NONE;
}

void TextureLoader::createTexture(IMAGE_FORMAT imageFormat, GLenum target, GLint level, GLsizei width, GLsizei height, GLint border, GLenum type, const void *pixels)
{
    switch (imageFormat)
    {
        case(FORMAT_PNG):
        {
            glTexImage2D(target, level, GL_RGBA, width, height, border, GL_RGBA, type, pixels);
            break;
        }
        case(FORMAT_JPG):
        {
			glTexImage2D(target, level, GL_RGB, width, height, border, GL_RGB, type, pixels);
            break;
        }
        default: break;
    }
}

GLuint TextureLoader::createEmptyTexture(int width, int height)
{
	unsigned char *data = new unsigned char[width * height];

	GLuint ID;

    for (int i = 0; i < width * height; i++)
    {
        data = 0;
    }
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glBindTexture(GL_TEXTURE_2D, 0);
    delete[] data;
    return ID;
}

unsigned char* TextureLoader::loadTextureData(const char* fileName, int* width, int* height, std::string directory, bool flip, unsigned int* ID, bool bind)
{
	std::string filePath = fileName;
	std::string fullPath = (directory + filePath);

	int nrChannels;

	stbi_set_flip_vertically_on_load(flip);
	unsigned char* data = stbi_load(fullPath.c_str(), width, height, &nrChannels, 4);

    if (!data)
    { 
        printf("TextureLoader::loadTextureData(...): Failed to load texture data at path: %s\n", fullPath.c_str());
        return data;
    }

	if (bind)
	{
		// 1 - amount, &texture - array of IDs
		glGenTextures(1, ID);
		glBindTexture(GL_TEXTURE_2D, *ID);
		// filtering options for this texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// GL_LINEAR would be the "normal" mode, GL_NEAREST pixel mode
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Upload pixels into texture
		#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
			glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		#endif

        IMAGE_FORMAT format = extensionToFormat(fs::path(filePath).extension().string().c_str());

        if (format == FORMAT_NONE)
        {
			printf("TextureLoader::loadTextureData(%s, %d, %d, %s, %d, %d): Unsupported image file type with extension: %s\n", fileName, *width, *height, directory.c_str(), flip, *ID, bind, fs::path(filePath).extension().string().c_str());
        }
        else
        {
            createTexture(format,
                GL_TEXTURE_2D,
                0,
                *width,
                *height,
                0,
                GL_UNSIGNED_BYTE,
                data
            );
		    glBindTexture(GL_TEXTURE_2D, 0);
        }
	}
	// download free memory
	// stbi_image_free(data);

	return data;
}