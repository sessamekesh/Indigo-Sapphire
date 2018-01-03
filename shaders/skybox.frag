#version 400

in vec3 fTexCoords;
uniform samplerCube skyTexture;

layout(location = 0) out vec4 color;

void main()
{
	//gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);
    color = texture(skyTexture, normalize(fTexCoords));
}