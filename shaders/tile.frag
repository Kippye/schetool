#version 430 core

out vec4 FragColor;

in vec2 TexCoord;
flat in vec2 bottomLeftTexCoord;
flat in vec2 tileSize;
flat in vec2 textureSize;
flat in vec2 atlasCoord;
flat in vec2 texPixelSize;
flat in vec2 atlasSize;
flat in vec4 color;
flat in int shouldTile;
flat in float selected;
flat in float posZ;
// free
flat in int isGizmo;

uniform sampler2D texture1;
uniform float time;

void main()
{
	vec2 finalCoords = TexCoord;
	vec4 selectionColor = vec4(selected / 4.0f);

	// convert these TexCoords into a local 0-1 range to make things easier to understand
	vec2 localTexCoord = (TexCoord - bottomLeftTexCoord) / (texPixelSize / atlasSize);

	vec2 scaledTexCoord;
	
	// TEXTUREMODE_TILE tiles create a new tile for every unit of size, divided by TextureSize
	if (shouldTile == 1)
	{
		scaledTexCoord = localTexCoord * tileSize / textureSize;
	}
	// TEXTUREMODE_STRETCH tiles stretch a texture for the entire size, only divide the texture by TextureSize
	else
	{
		scaledTexCoord = localTexCoord / textureSize;
	}

	if (texPixelSize.x == 0 || texPixelSize.y == 0)
	{
		FragColor = vec4(1.0f, 0.0f, 0.862, 1.0f);
	}

	// map the scaled coordinates to a 0-1 range
	vec2 tiledTexCoord = mod(scaledTexCoord, vec2(1.0f, 1.0f));

	// convert from the "local" texture coordinates to atlas coordinates
	finalCoords = bottomLeftTexCoord + tiledTexCoord * (texPixelSize / atlasSize);

	if (isGizmo == 0)
	{
		vec4 modifiedColor = texture(texture1, finalCoords) * color + (selectionColor * (min(sin(time * 2.0f) + 1.5f, 2.0f)));
		if (modifiedColor.w <= 0.1f)
		{
			discard;
		}
		FragColor = modifiedColor;
	}
	else
	{
		vec4 modifiedColor = texture(texture1, finalCoords) * color;
 		if (modifiedColor.w <= 0.1f)
		{
			discard;
		}
		FragColor = modifiedColor + (selectionColor);
	}
};
