varying vec3 fNormal;
varying vec2 fUV;

uniform sampler2D texSampler;

uniform vec3 lightDiffuseColor;
uniform vec3 lightAmbientColor;
uniform vec3 lightDir;

void main()
{
   float diffuseCoefficient = max(dot(normalize(fNormal), -lightDir), 0.0);
   vec4 texColor = texture(texSampler, fUV);

   gl_FragColor = vec4(
                  lightAmbientColor * texColor.xyz
                + diffuseCoefficient * lightDiffuseColor * texColor.xyz
				, 1.0);
}