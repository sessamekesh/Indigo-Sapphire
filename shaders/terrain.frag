#version 330

in vec3 fNormal;
in vec2 fUV;

uniform sampler2D texSampler;

uniform vec3 lightDiffuseColor;
uniform vec3 lightAmbientColor;
uniform vec3 lightDir;

layout(location = 0) out vec4 color;

void main()
{
   float diffuseCoefficient = max(dot(normalize(fNormal), -lightDir), 0.0);
   vec4 texColor = texture(texSampler, fUV);

   color = vec4(
                  lightAmbientColor * texColor.xyz
                + diffuseCoefficient * lightDiffuseColor * texColor.xyz
				, 1.0);
}