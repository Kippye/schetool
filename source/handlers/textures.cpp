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
#include <format>

namespace fs = std::filesystem;

#define DEBUG_TEXTURE_LOADING false

void TextureLoader::init()
{
    m_missingTextureID = createTextureFromData(m_missingTextureData.imageFormat, GL_TEXTURE_2D, 0, m_missingTextureData.width, m_missingTextureData.height, 0, GL_UNSIGNED_BYTE, m_missingTextureData.data, m_missingTextureData.size);
}

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

void TextureLoader::createTexture(IMAGE_FORMAT imageFormat, GLenum target, GLint level, GLsizei width, GLsizei height, GLint border, GLenum type, const void *pixels) const
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

GLuint TextureLoader::createTextureFromData(IMAGE_FORMAT imageFormat, GLenum target, GLint level, GLsizei width, GLsizei height, GLint border, GLenum type, const unsigned char* data, size_t length) const
{
    int w, h;
    int nrChannels = 4;
    unsigned char* pixels = stbi_load_from_memory(data, length, &w, &h, &nrChannels, nrChannels);

    GLuint ID = 0;
    // 1 - amount, array of IDs
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
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

    createTexture(imageFormat,
        target,
        level,
        width,
        height,
        border,
        type,
        pixels
    );

    glBindTexture(GL_TEXTURE_2D, 0);
    return ID;
}

std::optional<std::filesystem::path> TextureLoader::existsTextureAtPath(fs::path path) const
{
    // A full, extensioned path was given, just check if it exists.
    if (path.has_extension())
    {
        if (fs::exists(path))
        {
            return path;
        }
    }
    else
    {
        // Check the path with each image format extension.
        for (const auto& [format, extension] : m_formatExtensions)
        {
            fs::path pathAttempt = fs::path(path);
            pathAttempt.replace_extension(extension);

            if (fs::exists(pathAttempt))
            {
                return pathAttempt;
            }
        }
    }

    return std::nullopt;
}

std::filesystem::path TextureLoader::getRelativePathFromTextureFolder(const std::string& relativePath) const
{
    return fs::path(std::format("{}{}", textureFolder, relativePath));
}

GLuint TextureLoader::getMissingTexture() const
{
    return m_missingTextureID;
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

unsigned char* TextureLoader::loadTextureData(const std::filesystem::path& path, int* width, int* height, GLuint* ID, bool bind, bool flip)
{
	int nrChannels;

	stbi_set_flip_vertically_on_load(flip);
	unsigned char* data = stbi_load(path.string().c_str(), width, height, &nrChannels, 4);

    if (!data)
    { 
        printf("TextureLoader::loadTextureData(...): Failed to load texture data at path: %s\n", path.string().c_str());
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Upload pixels into texture
		#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
			glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		#endif

        IMAGE_FORMAT format = extensionToFormat(path.extension().string().c_str());

        if (format == FORMAT_NONE)
        {
			printf("TextureLoader::loadTextureData(%s, %d, %d, %d, %d): Unsupported image file type with extension: %s\n", path.string().c_str(), *width, *height, flip, *ID, bind, path.extension().string().c_str());
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