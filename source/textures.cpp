#include <textures.h>
extern "C" {
#include <glad.h>
}
#include <util.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <filesystem>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>
#include <cmath>
#include <rectpack2D/include/finders_interface.h>
#include <glm/gtx/io.hpp>
#include <chrono>

using namespace rectpack2D;

#define DEBUG_TEXTURE_LOADING false

GLuint TextureLoader::createEmptyTexture(int* width, int* height)
{
	unsigned char *data = new unsigned char[*width * *height];

	GLuint ID;

    for (int i = 0; i < *width * *height; i++)
        data = 0;
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Upload pixels into texture
		#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
			glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
		#endif
	}

	if (data)
	{
		// generate texture
		if (filePath.find(".png") != std::string::npos)
		{
			if (bind) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		else if (filePath.find(".jpg") != std::string::npos)
		{
			if (bind) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, *width, *height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cout << "Unsupported image file type!" << std::endl;
		}
	}
	else
	{
		std::cout << "Failed to load texture data at: " << fullPath << std::endl;
	}

	if (bind)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	// download free memory
	// stbi_image_free(data);

	return data;
}

unsigned char* TextureLoader::loadTextureData(std::string fullPath, int* width, int* height, bool flip, unsigned int* ID, bool bind)
{
	int nrChannels;

	stbi_set_flip_vertically_on_load(flip);
	unsigned char* data = stbi_load(fullPath.c_str(), width, height, &nrChannels, 4);

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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
	}

	if (data)
	{
		// generate texture
		if (fullPath.find(".png") != std::string::npos)
		{
			if (bind) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, *width, *height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		else if (fullPath.find(".jpg") != std::string::npos)
		{
			if (bind) glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, *width, *height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cout << "Unsupported image file type!" << std::endl;
		}
	}
	else
	{
		std::cout << "Failed to load texture data at: " << fullPath << std::endl;
	}

	if (bind)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	// download free memory
	// stbi_image_free(data);

	return data;
}

unsigned int TextureLoader::loadTexture_ID(const char* fileName, std::string directory, bool flip)
{
	std::string filePath = fileName;
	std::string fullPath = (directory + filePath);

	unsigned int texture;
	// 1 - amount, &texture - array of IDs
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// filtering options for this texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// GL_LINEAR would be the "normal" mode, GL_NEAREST pixel mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_set_flip_vertically_on_load(flip);
	int width, height, nrChannels;
	unsigned char* data = stbi_load(fullPath.c_str(), &width, &height, &nrChannels, 0);
	if (data)
	{
		// generate texture
		if (filePath.find(".png") != std::string::npos)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		else if (filePath.find(".jpg") != std::string::npos)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cout << "Unsupported image file type!" << std::endl;
		}
		// generates a mipmap for us
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture at: " << fullPath << std::endl;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	// download free memory
	stbi_image_free(data);

	return texture;
}

E_Texture* TextureLoader::loadTexture(const char* fileName, std::string directory, bool flip)
{
	if (DEBUG_TEXTURE_LOADING)std::cout << "loadTexture" << std::endl;
	std::string filePath = fileName;
	std::string fullPath = (directory + filePath);
	E_Texture* texture = new E_Texture();

	texture->path = fileName;

	stbi_set_flip_vertically_on_load(flip);
	unsigned char* data = stbi_load(fullPath.c_str(), &texture->width, &texture->height, NULL, 4);

	// 1 - amount, &texture - array of IDs
	glGenTextures(1, &texture->ID);
	glBindTexture(GL_TEXTURE_2D, texture->ID);
	// filtering options for this texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// GL_LINEAR would be the "normal" mode, GL_NEAREST pixel mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Upload pixels into texture
	#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
		glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
	#endif

	if (data)
	{
		// generate texture
		if (filePath.find(".png") != std::string::npos)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		else if (filePath.find(".jpg") != std::string::npos)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->width, texture->height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cout << "Unsupported image file type!" << std::endl;
		}
	}
	else
	{
		std::cout << "Failed to load texture at: " << fullPath << std::endl;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	// download free memory
	stbi_image_free(data);

	if (DEBUG_TEXTURE_LOADING)std::cout << "loadTexture end" << std::endl;
	return texture;
}

E_Texture* TextureLoader::loadTexture(std::string fullPath, bool flip)
{
	if (DEBUG_TEXTURE_LOADING)std::cout << "loadTexture" << std::endl;
	E_Texture* texture = new E_Texture();

	std::filesystem::path fileName = std::filesystem::path(fullPath).filename();

	texture->path = fileName.string();

	stbi_set_flip_vertically_on_load(flip);
	unsigned char* data = stbi_load(fullPath.c_str(), &texture->width, &texture->height, NULL, 4);

	// 1 - amount, &texture - array of IDs
	glGenTextures(1, &texture->ID);
	glBindTexture(GL_TEXTURE_2D, texture->ID);
	// filtering options for this texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// GL_LINEAR would be the "normal" mode, GL_NEAREST pixel mode
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif

	if (data)
	{
		// generate texture
		if (fileName.extension().string() == ".png")
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture->width, texture->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		else if (fileName.extension().string() == ".jpg")
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->width, texture->height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			std::cout << "Unsupported image file type!" << std::endl;
		}
	}
	else
	{
		std::cout << "Failed to load texture at: " << fullPath << std::endl;
	}

	glBindTexture(GL_TEXTURE_2D, 0);
	// download free memory
	stbi_image_free(data);

	if (DEBUG_TEXTURE_LOADING)std::cout << "loadTexture end" << std::endl;
	return texture;
}

std::vector<E_Texture*>& TextureLoader::loadTextures(std::vector<std::string> fileNames, std::string directory, bool flip)
{
	if (DEBUG_TEXTURE_LOADING)std::cout << "loadTextures" << std::endl;
	std::vector<E_Texture*>* textures = new std::vector<E_Texture*>();

	for (int i = 0; i < fileNames.size(); i++)
	{
		textures->push_back(loadTexture(fileNames[i].c_str(), directory, flip));
	}

	if (DEBUG_TEXTURE_LOADING)std::cout << "loadTextures end" << std::endl;
	return *textures;
}

std::vector<E_Texture*>& TextureLoader::loadTextures(std::vector<std::string> fullPaths, bool flip)
{
	if (DEBUG_TEXTURE_LOADING)std::cout << "loadTextures" << std::endl;
	std::vector<E_Texture*>* textures = new std::vector<E_Texture*>();

	for (int i = 0; i < fullPaths.size(); i++)
	{
		textures->push_back(loadTexture(fullPaths[i], flip));
	}

	if (DEBUG_TEXTURE_LOADING)std::cout << "loadTextures end" << std::endl;
	return *textures;
}

TextureAtlas* TextureLoader::loadTextureAtlas(std::vector<std::string> fullPaths, bool flip)
{
	auto start_time = std::chrono::high_resolution_clock::now();
	constexpr bool allow_flip = false;
	const auto runtime_flipping_mode = flipping_option::DISABLED;

	/* 
		Here, we choose the "empty_spaces" class that the algorithm will use from now on. 
	
		The first template argument is a bool which determines
		if the algorithm will try to flip rectangles to better fit them.

		The second argument is optional and specifies an allocator for the empty spaces.
		The default one just uses a vector to store the spaces.
		You can also pass a "static_empty_spaces<10000>" which will allocate 10000 spaces on the stack,
		possibly improving performance.
	*/

	using spaces_type = rectpack2D::empty_spaces<allow_flip, default_empty_spaces>;

	/* 
		rect_xywh or rect_xywhf (see src/rect_structs.h), 
		depending on the value of allow_flip.
	*/

	using rect_type = output_rect_t<spaces_type>;

	/*
		Note: 

		The multiple-bin functionality was removed. 
		This means that it is now up to you what is to be done with unsuccessful insertions.
		You may initialize another search when this happens.
	*/

	auto report_successful = [](rect_type&) {
		return callback_result::CONTINUE_PACKING;
	};

	auto report_unsuccessful = [](rect_type&) {
		//std::cerr << 
		return callback_result::ABORT_PACKING;
	};

	/*
		Initial size for the bin, from which the search begins.
		The result can only be smaller - if it cannot, the algorithm will gracefully fail.
	*/

	const auto max_side = 10000;

	/*
		The search stops when the bin was successfully inserted into,
		AND the next candidate bin size differs from the last successful one by *less* then discard_step.

		The best possible granuarity is achieved with discard_step = 1.
		If you pass a negative discard_step, the algoritm will search with even more granularity -
		E.g. with discard_step = -4, the algoritm will behave as if you passed discard_step = 1,
		but it will make as many as 4 attempts to optimize bins down to the single pixel.

		Since discard_step = 0 does not make sense, the algoritm will automatically treat this case 
		as if it were passed a discard_step = 1.

		For common applications, a discard_step = 1 or even discard_step = 128
		should yield really good packings while being very performant.
		If you are dealing with very small rectangles specifically,
		it might be a good idea to make this value negative.

		See the algorithm section of README for more information.
	*/

	const auto discard_step = -4;

	std::vector<rect_type> rectangles;
	rectpack2D::rect_wh packed_size;

	std::vector<unsigned char*> atlasTextures = {};

	auto texture_load_start_time = std::chrono::high_resolution_clock::now();
	
	/// PRELOADED / PROGRAM TEXTURES
	// missing texture (loaded from the program's textures folder)
	int missingWidth, missingHeight;
	unsigned char* missingTexture = loadTextureData(textureFolder + "missing_blue.png", &missingWidth, &missingHeight, false);
	atlasTextures.push_back(missingTexture);
	rectangles.emplace_back(rect_xywh(0, 0, missingWidth, missingHeight));

	// grid texture (loaded from the program's textures folder)
	int gridWidth, gridHeight;
	unsigned char* gridTexture = loadTextureData(textureFolder + "grid_blue.png", &gridWidth, &gridHeight, false);
	atlasTextures.push_back(gridTexture);
	rectangles.emplace_back(rect_xywh(0, 0, gridWidth, gridHeight));

	// drag gizmo texture (loaded from the program's textures folder)
	int dragWidth, dragHeight;
	unsigned char* dragTexture = loadTextureData(textureFolder + "drag_blue.png", &dragWidth, &dragHeight, false);
	atlasTextures.push_back(dragTexture);
	rectangles.emplace_back(rect_xywh(0, 0, dragWidth, dragHeight));

	// dragger gizmo texture (loaded from the program's textures folder)
	int draggerWidth, draggerHeight;
	unsigned char* draggerTexture = loadTextureData(textureFolder + "dragger_blue.png", &draggerWidth, &draggerHeight, false);
	atlasTextures.push_back(draggerTexture);
	rectangles.emplace_back(rect_xywh(0, 0, draggerWidth, draggerHeight));

	// // horizontal move dragger gizmo texture (loaded from the program's textures folder)
	// int hMoveDraggerWidth, hMoveDraggerHeight;
	// unsigned char* hMoveDraggerTexture = loadTextureData(textureFolder + "dragger_move_horizontal_blue.png", &hMoveDraggerWidth, &hMoveDraggerHeight, false);
	// atlasTextures.push_back(hMoveDraggerTexture);
	// rectangles.emplace_back(rect_xywh(0, 0, hMoveDraggerWidth, hMoveDraggerHeight));

	// // vertical move dragger gizmo texture (loaded from the program's textures folder)
	// int vMoveDraggerWidth, vMoveDraggerHeight;
	// unsigned char* vMoveDraggerTexture = loadTextureData(textureFolder + "dragger_move_vertical_blue.png", &vMoveDraggerWidth, &vMoveDraggerHeight, false);
	// atlasTextures.push_back(vMoveDraggerTexture);
	// rectangles.emplace_back(rect_xywh(0, 0, vMoveDraggerWidth, vMoveDraggerHeight));

	// // biaxial move dragger gizmo texture (loaded from the program's textures folder)
	// int bMoveDraggerWidth, bMoveDraggerHeight;
	// unsigned char* bMoveDraggerTexture = loadTextureData(textureFolder + "dragger_move_biaxial_blue.png", &bMoveDraggerWidth, &bMoveDraggerHeight, false);
	// atlasTextures.push_back(bMoveDraggerTexture);
	// rectangles.emplace_back(rect_xywh(0, 0, bMoveDraggerWidth, bMoveDraggerHeight));

	for (int i = 0; i < fullPaths.size(); i++)
	{
		int width, height;
		unsigned char* texture = loadTextureData(fullPaths[i], &width, &height, false);
		atlasTextures.push_back(texture);

		// add rect to pack
		rectangles.emplace_back(rect_xywh(0, 0, width, height));
	}

	auto end_time2 = std::chrono::high_resolution_clock::now();
	auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(end_time2 - texture_load_start_time).count();
	std::cout << "Atlas textures loaded & rectangles created in " << duration2 << " ms" << std::endl;
	
	auto report_result = [&rectangles](const rect_wh& result_size) {
		std::cout << "Packed atlas size: " << result_size.w << "; " << result_size.h << std::endl;
	};

	{
		/*
			Example 1: Find best packing with default orders. 

			If you pass no comparators whatsoever, 
			the standard collection of 6 orders:
		   	by area, by perimeter, by bigger side, by width, by height and by "pathological multiplier"
			- will be passed by default.
		*/

		const auto result_size = find_best_packing<spaces_type>(
			rectangles,
			make_finder_input(
				max_side,
				discard_step,
				report_successful,
				report_unsuccessful,
				runtime_flipping_mode
			)
		);

		packed_size = result_size;
		report_result(result_size);
	}

	auto end_time = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
	std::cout << "Atlas packed in " << duration << " ms" << std::endl;

	TextureAtlas* textureAtlas = new TextureAtlas();

	int atlasDataSize = (packed_size.area());
	unsigned char* textureAtlasData = new unsigned char[atlasDataSize * 4];
	// create an empty texture for the atlas
	textureAtlas->width = packed_size.w;
	textureAtlas->height = packed_size.h;
	textureAtlas->ID = createEmptyTexture(&textureAtlas->width, &textureAtlas->height);
	textureAtlas->data = textureAtlasData;

	fullPaths.insert(fullPaths.begin(), std::string("textures/missing_blue.png"));
	fullPaths.insert(fullPaths.begin() + 1, std::string("textures/grid_blue.png"));
	fullPaths.insert(fullPaths.begin() + 2, std::string("textures/drag_blue.png"));
	fullPaths.insert(fullPaths.begin() + 3, std::string("textures/dragger_blue.png"));
	// fullPaths.insert(fullPaths.begin() + 4, std::string("textures\\dragger_move_horizontal_blue.png"));
	// fullPaths.insert(fullPaths.begin() + 5, std::string("textures\\dragger_move_vertical_blue.png"));
	// fullPaths.insert(fullPaths.begin() + 6, std::string("textures\\dragger_move_biaxial_blue.png"));

	for (int i = 0; i < rectangles.size(); i++)
	{
		unsigned char* texture = atlasTextures[i];
		std::filesystem::path fileName = std::filesystem::path(fullPaths[i]).filename();
		textureAtlas->textureFiles.push_back(fileName.string());
		textureAtlas->textureAtlasCoords[fileName.string()] = glm::uvec4(rectangles[i].x, rectangles[i].y, rectangles[i].w, rectangles[i].h);

		// add subtexture to texture atlas
		glBindTexture(GL_TEXTURE_2D, textureAtlas->ID);
		if (DEBUG_TEXTURE_LOADING) { std::cout << "Packed atlas texture: " + fileName.string() << ": " << rectangles[i].x << "; " << rectangles[i].y << "; " << rectangles[i].w << "; " << rectangles[i].h << std::endl; }
		glTexSubImage2D(GL_TEXTURE_2D, 0, rectangles[i].x, rectangles[i].y, rectangles[i].w, rectangles[i].h, GL_RGBA, GL_UNSIGNED_BYTE, texture);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	glBindTexture(GL_TEXTURE_2D, textureAtlas->ID);
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureAtlas->data);

	if (DEBUG_TEXTURE_LOADING) { stbi_write_png("packed_atlas.png", textureAtlas->width, textureAtlas->height, 4, textureAtlas->data, textureAtlas->width * 4); }
	//stbi_write_png("packed_atlas.png", textureAtlas->width, textureAtlas->height, 4, textureAtlas->data, textureAtlas->width * 4);
	return textureAtlas;

	// {
	// 	/* Example 2: Find best packing using all provided custom rectangle orders. */

	// 	using rect_ptr = rect_type*;

	// 	auto my_custom_order_1 = [](const rect_ptr a, const rect_ptr b) {
	// 		return a->get_wh().pathological_mult() > b->get_wh().pathological_mult();
	// 	};

	// 	auto my_custom_order_2 = [](const rect_ptr a, const rect_ptr b) {
	// 		return a->get_wh().pathological_mult() < b->get_wh().pathological_mult();
	// 	};

	// 	const auto result_size = find_best_packing<spaces_type>(
	// 		rectangles,
	// 		make_finder_input(
	// 			max_side,
	// 			discard_step,
	// 			report_successful,
	// 			report_unsuccessful,
	// 			runtime_flipping_mode
	// 		),

	// 		my_custom_order_1,
	// 		my_custom_order_2
	// 	);

	// 	report_result(result_size);
	// }

	// {
	// 	/* Example 3: Find best packing exactly in the order of provided input. */

	// 	const auto result_size = find_best_packing_dont_sort<spaces_type>(
	// 		rectangles,
	// 		make_finder_input(
	// 			max_side,
	// 			discard_step,
	// 			report_successful,
	// 			report_unsuccessful,
	// 			runtime_flipping_mode
	// 		)
	// 	);

	// 	report_result(result_size);
	// }

	// {
	// 	/* Example 4: Manually perform insertions. This way you can try your own best-bin finding logic. */

	// 	auto packing_root = spaces_type({ max_side, max_side });
	// 	packing_root.flipping_mode = runtime_flipping_mode;

	// 	for (auto& r : rectangles) {
	// 		if (const auto inserted_rectangle = packing_root.insert(std::as_const(r).get_wh())) {
	// 			r = *inserted_rectangle;
	// 		}
	// 		else {
	// 			std::cout << "Failed to insert a rectangle." << std::endl;
	// 			break;
	// 		}
	// 	}

	// 	const auto result_size = packing_root.get_rects_aabb();

	// 	report_result(result_size);
	// }
}

glm::uvec4 TextureLoader::getAtlasCoords(TextureAtlas* atlas, int index)
{
	if (atlas == nullptr) { std::cerr << "getAtlasCoords: atlas == nullptr" << std::endl; return getAtlasTextureCoords(atlas, std::string("missing_blue.png")); }
	
	//std::cout << "I: " << index << " = " << atlas->textureAtlasCoords[atlas->textureFiles[index]].x << "; " << atlas->textureAtlasCoords[atlas->textureFiles[index]].y << "; " << atlas->textureAtlasCoords[atlas->textureFiles[index]].z << "; " << atlas->textureAtlasCoords[atlas->textureFiles[index]].w << std::endl;

	return atlas->textureAtlasCoords[atlas->textureFiles[index]]; 
}

unsigned int TextureLoader::getAtlasTextureIndex(TextureAtlas* atlas, glm::uvec2 coords)
{
	if (atlas == nullptr) { std::cerr << "getAtlasTextureIndex: atlas == nullptr" << std::endl; return 0; }
	
	// TODO: this could be improved
	std::string textureName = getAtlasTexturePath(atlas, coords);
	return std::find(atlas->textureFiles.begin(), atlas->textureFiles.end(), std::string(textureName)) - atlas->textureFiles.begin();
}

unsigned int TextureLoader::getAtlasTextureIndex(TextureAtlas* atlas, const char* textureName)
{
	if (atlas == nullptr) { std::cerr << "getAtlasTextureIndex: atlas == nullptr" << std::endl; return 0; }
	
	// TODO: this could be improved
	return std::find(atlas->textureFiles.begin(), atlas->textureFiles.end(), std::string(textureName)) - atlas->textureFiles.begin();
}

std::string TextureLoader::getAtlasTexturePath(TextureAtlas* atlas, glm::uvec2 coords)
{
	if (atlas == nullptr) { std::cerr << "getAtlasTexturePath: atlas == nullptr" << std::endl; return std::string("missing_blue.png"); }

	for (std::string &texture : atlas->textureFiles)
	{
		if (atlas->textureAtlasCoords[texture].x == coords.x && atlas->textureAtlasCoords[texture].y == coords.y)
		{
			return texture;
		}
	}
}

glm::uvec4 TextureLoader::getAtlasTextureCoords(TextureAtlas* atlas, std::string texturePath)
{
	if (atlas == nullptr) { std::cerr << "getAtlasTextureCoords: atlas == nullptr" << std::endl; return getAtlasTextureCoords(atlas, std::string("missing_blue.png")); }

	for (const auto& pair : atlas->textureAtlasCoords) {
		const std::string &keyPath = pair.first;
		
		if (DEBUG_TEXTURE_LOADING) { std::cout << "Present path: " << keyPath << " Given path: " << texturePath << std::endl; }

		if (keyPath == texturePath)
		{
			return pair.second;
		}
	}

	std::cerr << "getAtlasTextureCoords: Texture path [" + texturePath + "] not found in atlas " << std::endl; 
	return getAtlasTextureCoords(atlas, std::string("missing_blue.png"));
}
