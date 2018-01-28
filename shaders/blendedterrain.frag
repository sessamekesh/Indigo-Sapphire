#version 400 core

in vec3 fNormal;
in vec2 fUV;

uniform sampler2D blendMap;
uniform sampler2D redMap;
uniform sampler2D greenMap;
uniform sampler2D blueMap;

uniform float redUVScale;
uniform float greenUVScale;
uniform float blueUVScale;

uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform vec3 lightDirection;

layout(location = 0) out vec4 color;

void main()
{
   float diffuseCoefficient = max(dot(normalize(fNormal), -lightDirection), 0.0);
   
   vec4 blendMapColor = texture(blendMap, fUV);
   vec4 redMapColor = texture(redMap, fUV * redUVScale);
   vec4 greenMapColor = texture(greenMap, fUV * greenUVScale);
   vec4 blueMapColor = texture(blueMap, fUV * blueUVScale);

   vec3 matColor = redMapColor.rgb * blendMapColor.r
                   + greenMapColor.rgb * blendMapColor.g
				   + blueMapColor.rgb * blendMapColor.b;
   
   color = vec4(
             lightAmbient * matColor
           + lightDiffuse * diffuseCoefficient * matColor
		   , 1.0);
}