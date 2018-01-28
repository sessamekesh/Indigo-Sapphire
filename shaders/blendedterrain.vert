#version 400 core

layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

out vec3 fNormal;
out vec2 fUV;

uniform vec3 clipPlaneOrigin;
uniform vec3 clipPlaneNormal;

uniform mat4 matWorld;
uniform mat4 matView;
uniform mat4 matProj;

void main()
{
   vec4 worldPos = (matWorld * vec4(vPosition, 1.0));
   gl_ClipDistance[0] = dot(worldPos.xyz - clipPlaneOrigin, clipPlaneNormal);

   fUV = vUV;
   fNormal = (matWorld * vec4(vNormal, 0.0)).xyz;
   gl_Position = matProj * matView * worldPos;
}