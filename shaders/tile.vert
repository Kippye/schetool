#version 430 core
layout (location = 0) in vec4 aInstanceTransformData; // xPos / yPos / xSize / ySize
layout (location = 1) in vec4 aInstanceTextureData; // texSizeX / texSizeY / free / free
layout (location = 2) in vec4 aInstanceAtlasData; // atlasX / atlasY / atlasSizeX / atlasSizeY
layout (location = 3) in vec4 aInstanceColorData; // colorR / colorG / colorB / colorA (opacity * 255)
layout (location = 4) in vec4 aInstanceAdditionalData; // shouldTile / selected, highlighted / zPos / isGizmo

// to geometry shader
out VS_OUT
{
	vec3 pos;
	vec2 size;
	vec2 texSize;
	vec2 atlasCoord;
	vec2 texPixelSize;
	vec4 color;
	int shouldTile;
	float selected;
	int isGizmo;
	mat4 matrix;
} vs_out;

uniform mat4 view; // world to view
uniform mat4 projection; // view to projection

void main()
{
	vs_out.pos = vec3(aInstanceTransformData.xy, aInstanceAdditionalData.z);
	vs_out.size = aInstanceTransformData.zw;
	vs_out.texSize = aInstanceTextureData.xy;
	vs_out.atlasCoord = aInstanceAtlasData.xy;
	vs_out.texPixelSize = aInstanceAtlasData.zw;
	vs_out.color = aInstanceColorData.xyzw;
	vs_out.shouldTile = int(aInstanceAdditionalData.x);
	vs_out.selected = aInstanceAdditionalData.y;
	vs_out.isGizmo = int(aInstanceAdditionalData.w);
	vs_out.matrix = (projection * view);
};
