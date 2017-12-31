varying vec3 fTexCoords;
uniform samplerCube skyTexture;

void main()
{
	//gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);
    gl_FragColor = texture(skyTexture, normalize(fTexCoords));
}