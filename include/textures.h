#pragma once

extern "C" {
#include <glad.h>
}

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <map>

struct E_Texture
{
	public:
	GLuint ID;
	// NOTE: currently not the actual path, just the name
	std::string path;
	// NOTE: might not always be set to a correct value
	glm::uvec4 atlasLocation = glm::uvec4(0);
	int width;
	int height;
};

struct TextureAtlas
{
	public:
	GLuint ID;
	unsigned char* data;
	int width;
	int height;

	std::vector<std::string> textureFiles = {};
	std::map<std::string, glm::uvec4> textureAtlasCoords;

	~TextureAtlas()
	{
		delete[] data;
	}
};

class TextureLoader
{
	private:

	public:
		static inline const std::string textureFolder = "textures/";

		GLuint createEmptyTexture(int* width, int* height);
		unsigned int loadTexture_ID(const char* fileName, std::string directory = textureFolder, bool flip = true);
		unsigned char* loadTextureData(const char* fileName, int* width, int* height, std::string directory = textureFolder, bool flip = true, unsigned int* ID = nullptr, bool bind = false);
		unsigned char* loadTextureData(std::string fullPath, int* width, int* height, bool flip = true, unsigned int* ID = nullptr, bool bind = false);

		E_Texture* loadTexture(const char* fileName, std::string directory = textureFolder, bool flip = false);
		E_Texture* loadTexture(std::string fullPath, bool flip = false);
		std::vector<E_Texture*>& loadTextures(std::vector<std::string> fileNames, std::string directory = textureFolder, bool flip = false);
		std::vector<E_Texture*>& loadTextures(std::vector<std::string> fullPaths, bool flip = false);

		TextureAtlas* loadTextureAtlas(std::vector<std::string> fullPaths, bool flip = false);

		unsigned int getAtlasTextureIndex(TextureAtlas* atlas, glm::uvec2 coords);
		unsigned int getAtlasTextureIndex(TextureAtlas* atlas, const char* textureName);
		glm::uvec4 getAtlasCoords(TextureAtlas* atlas, int index);
		std::string getAtlasTexturePath(TextureAtlas* atlas, glm::uvec2 coords);
		glm::uvec4 getAtlasTextureCoords(TextureAtlas* atlas, std::string texturePath);
};
