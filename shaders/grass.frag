#version 330

smooth in vec2 vTexCoord;
smooth in vec3 vWorldPos;
smooth in vec4 vEyeSpacePos;

out vec4 color;

uniform sampler2D gSampler;
uniform vec4 vColor;
uniform vec3 vEyePosition;

uniform float fAlphaTest;
uniform float fAlphaMultiplier;

void main()
{
	vec2 sc = vec2(vTexCoord.x, 1.0 - vTexCoord.y);
	vec4 vTexColor = texture2D(gSampler, sc);
	float fNewAlpha = vTexColor.a * fAlphaMultiplier;
	
	if (fNewAlpha < fAlphaTest) discard;

	fNewAlpha = min(fNewAlpha, 1.0);

	vec4 vMixedColor = vTexColor * vColor;

	color = vec4(vMixedColor.zyx, fNewAlpha);
}