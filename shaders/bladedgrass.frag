#version 400

in VertexData
{
    vec2 UV;
	vec3 vPos;
	vec3 vNormal;
} VertexIn;

layout(location = 0) out vec4 color;

// TODO SESS: Touch-up the layout of these uniform blocks, according to the words of the Superbible

layout (std140) uniform PerSceneBlockF
{
    vec4 lightAmbient;
	vec4 lightDiffuse;
	vec4 lightSpecular;
	vec3 lightDirection;
	vec4 cBladeSpecular;
};

layout (std140) uniform PerFrameBlockF
{
    vec3 vCameraPos;
};

uniform sampler2D texGrassBlade;

void main()
{
    vec2 uv = vec2(VertexIn.UV.x, 1.0 - VertexIn.UV.y);
	vec4 vTexColor = texture2D(texGrassBlade, uv);

	vec4 ambientComponent = vTexColor * lightAmbient;
	float diffuseCoefficient = max(dot(-lightDirection, VertexIn.vNormal), 0.0);
	vec4 diffuseComponent = vTexColor * lightDiffuse * diffuseCoefficient;
	float specCoefficient;
	if (diffuseCoefficient > 0.0)
	{
	    vec3 nToLight = normalize(-lightDirection);
		vec3 nToCamera = normalize(VertexIn.vPos - vCameraPos);
	    vec3 lightReflectionDir = reflect(nToLight, VertexIn.vNormal);
		specCoefficient = max(dot(lightReflectionDir, nToCamera), 0.0);
	}
	vec4 specularComponent = cBladeSpecular * lightSpecular * specCoefficient;

	//color = vec4(ambientComponent.rgb + diffuseComponent.rgb + specularComponent.rgb, 1.0);
	color = vec4(ambientComponent.rgb + diffuseComponent.rgb, 1.0);
}