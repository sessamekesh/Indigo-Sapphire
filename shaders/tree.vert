#version 400 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNorm;
layout(location = 2) in vec2 vUV;

out vec3 fNorm;
out vec2 fUV;

uniform mat4 matWorld;
uniform mat4 matView;
uniform mat4 matProj;

uniform vec3 clipPlaneOrigin;
uniform vec3 clipPlaneNormal;

void main()
{
   vec3 worldPos = (matWorld * vec4(vPos, 1.0)).xyz;
   gl_ClipDistance[0] = dot(worldPos - clipPlaneOrigin, clipPlaneNormal);
   fUV = vUV;
   fNorm = (matWorld * vec4(vNorm, 0.0)).xyz;
   gl_Position = matProj * matView * matWorld * vec4(vPos, 1.0);
};