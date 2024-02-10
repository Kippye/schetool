#version 430 core
layout (triangle_strip, max_vertices = 4) out;
// to frag shader
out vec2 TexCoord;
flat out vec2 bottomLeftTexCoord;
flat out vec2 tileSize;
flat out vec2 textureSize;
flat out vec2 atlasCoord;
flat out vec2 texPixelSize;
flat out vec2 atlasSize;
flat out vec4 color;
flat out int shouldTile;
flat out float selected;
flat out float posZ;
flat out int isGizmo;
// from vec shader
layout (points) in;
in VS_OUT
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
} gs_in[];

uniform vec2 inTexAtlasSize;
uniform bool perspective = true; // whether or not to use the Z position in matrix calculations

// x = atlasCoord.X / atlasSize.X + texPixelSize.X / atlasSize.X

vec2 getTexCoord(vec2 cornerPos)
{
	return gs_in[0].atlasCoord / inTexAtlasSize + (gs_in[0].texPixelSize / inTexAtlasSize * vec2(cornerPos.x, -cornerPos.y));// + vec2(0.0f, gs_in[0].texPixelSize.y / inTexAtlasSize.y);
}

vec2 getBottomLeftTexCoord()
{
	return getTexCoord(vec2(0.0f, 0.0f));
}

void main() {
	// BOTTOM LEFT
	gl_Position = gs_in[0].matrix * vec4(gs_in[0].pos.x, gs_in[0].pos.y, perspective ? gs_in[0].pos.z : 0.0f, 1.0f);
		TexCoord = getTexCoord(vec2(0.0f, 0.0f));
		bottomLeftTexCoord = getBottomLeftTexCoord();
		atlasSize = inTexAtlasSize;
		tileSize = gs_in[0].size;
		textureSize = gs_in[0].texSize;
		atlasCoord = gs_in[0].atlasCoord;
		texPixelSize = gs_in[0].texPixelSize;
		color = gs_in[0].color;
		shouldTile = gs_in[0].shouldTile;
		selected = gs_in[0].selected;
		posZ = perspective ? gs_in[0].pos.z : 0.0f;
		isGizmo = gs_in[0].isGizmo;
    EmitVertex();
	// BOTTOM RIGHT
	gl_Position = gs_in[0].matrix * vec4(gs_in[0].pos.x + gs_in[0].size.x, gs_in[0].pos.y, perspective ? gs_in[0].pos.z : 0.0f, 1.0f);
		TexCoord = getTexCoord(vec2(1.0f, 0.0f));
		bottomLeftTexCoord = getBottomLeftTexCoord();
		atlasSize = inTexAtlasSize;
		tileSize = gs_in[0].size;
		textureSize = gs_in[0].texSize;
		atlasCoord = gs_in[0].atlasCoord;
		texPixelSize = gs_in[0].texPixelSize;
		color = gs_in[0].color;
		shouldTile = gs_in[0].shouldTile;
		selected = gs_in[0].selected;
		posZ = perspective ? gs_in[0].pos.z : 0.0f;
		isGizmo = gs_in[0].isGizmo;
	EmitVertex();
	// TOP LEFT
	gl_Position = gs_in[0].matrix * vec4(gs_in[0].pos.x, gs_in[0].pos.y + gs_in[0].size.y, perspective ? gs_in[0].pos.z : 0.0f, 1.0f);
		TexCoord = getTexCoord(vec2(0.0f, 1.0f));
		bottomLeftTexCoord = getBottomLeftTexCoord();
		atlasSize = inTexAtlasSize;
		tileSize = gs_in[0].size;
		textureSize = gs_in[0].texSize;
		atlasCoord = gs_in[0].atlasCoord;
		texPixelSize = gs_in[0].texPixelSize;
		color = gs_in[0].color;
		shouldTile = gs_in[0].shouldTile;
		selected = gs_in[0].selected;
		posZ = perspective ? gs_in[0].pos.z : 0.0f;
		isGizmo = gs_in[0].isGizmo;
	EmitVertex();
	// TOP RIGHT
	gl_Position = gs_in[0].matrix * vec4(gs_in[0].pos.x + gs_in[0].size.x, gs_in[0].pos.y + gs_in[0].size.y, perspective ? gs_in[0].pos.z : 0.0f, 1.0f);
		TexCoord = getTexCoord(vec2(1.0f, 1.0f));
		bottomLeftTexCoord = getBottomLeftTexCoord();
		atlasSize = inTexAtlasSize;
		tileSize = gs_in[0].size;
		textureSize = gs_in[0].texSize;
		atlasCoord = gs_in[0].atlasCoord;
		texPixelSize = gs_in[0].texPixelSize;
		color = gs_in[0].color;
		shouldTile = gs_in[0].shouldTile;
		selected = gs_in[0].selected;
		posZ = perspective ? gs_in[0].pos.z : 0.0f;
		isGizmo = gs_in[0].isGizmo;
	EmitVertex();

    EndPrimitive();
}
