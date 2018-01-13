#version 400 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNorm;
layout(location = 2) in vec3 vTangent;
layout(location = 3) in vec2 vUV;

out vec3 fPos;
out vec2 fUV;
out vec3 toLight;
out vec3 toCamera;
out float err;

uniform mat4 matWorld;
uniform mat4 matView;
uniform mat4 matProj;
uniform vec3 lightDirection;
uniform vec3 cameraPosition;

uniform vec3 clipPlaneOrigin;
uniform vec3 clipPlaneNormal;

void main()
{
   fUV = vUV;
   vec4 worldPos = matWorld * vec4(vPos, 1.0);
   gl_ClipDistance[0] = dot(worldPos.xyz - clipPlaneOrigin, clipPlaneNormal);

   mat4 modelViewMatrix = matView * matWorld;
   vec4 positionRelativeToCam = modelViewMatrix * vec4(vPos, 1.0);
   vec3 surfaceNormal = (modelViewMatrix * vec4(vNorm, 0.0)).xyz;

   vec3 norm = normalize(surfaceNormal);
   vec3 tang = normalize((modelViewMatrix * vec4(vTangent, 0.0)).xyz);
   vec3 bitang = normalize(cross(norm, tang));

   mat3 matTangent = mat3(
	 tang.x, bitang.x, norm.x,
	 tang.y, bitang.y, norm.y,
	 tang.z, bitang.z, norm.z
   );

   toLight = matTangent * (modelViewMatrix * vec4(-lightDirection, 0.0)).xyz;
   toCamera = matTangent * (matView * vec4(cameraPosition - worldPos.xyz, 0.0)).xyz;

   gl_Position = matProj * matView * worldPos;
   fPos = worldPos.xyz;
}