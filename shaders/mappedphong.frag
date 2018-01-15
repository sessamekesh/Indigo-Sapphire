
#version 400 core

in vec3 fPos;
in vec2 fUV;
in vec3 toLight;
in vec3 toCamera;

layout(location = 0) out vec4 color;

uniform mat4 matWorld;

uniform sampler2D normalMap;
uniform sampler2D diffuseMap;
uniform sampler2D specularMap;

uniform float shininess;
uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform vec3 lightSpecular;

void main()
{
   vec2 uv = vec2(fUV.x, -fUV.y);
   color = vec4(0.0, 0.0, 0.0, 1.0);

   vec3 matDiffuse = texture(diffuseMap, uv).xyz;
   vec3 matSpecular = texture(specularMap, uv).xyz;

   vec3 sampledNormal = texture(normalMap, uv).xyz;
   sampledNormal.xyz = sampledNormal.xyz * 2.0 - 1.0;

   vec3 normal = normalize((matWorld * vec4(sampledNormal, 0.0)).xyz);
   vec3 nToLight = normalize(toLight);
   vec3 nToCamera = normalize(toCamera);

   float diffuseFactor = max(dot(nToLight, normal), 0.0);
   float specFactor = 0.0;
   if (diffuseFactor > 0.0)
   {
      vec3 lightReflectionDir = reflect(-nToLight, normal);
	  float specularAngle = max(dot(lightReflectionDir, nToCamera), 0.0);
	  specFactor = pow(specularAngle, shininess);
   }

   vec3 partialColor = lightAmbient * matDiffuse;
   partialColor += diffuseFactor * lightDiffuse * matDiffuse;
   partialColor += specFactor * lightSpecular * matSpecular;

   //color = vec4(matDiffuse, 1.0);
   color = vec4(partialColor, 1.0);
   //color = vec4(1.0, 0.0, 0.0, 1.0);
}