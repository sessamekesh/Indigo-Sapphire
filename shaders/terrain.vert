#version 330 core

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
   vec3 worldPos = (matWorld * vec4(vPosition, 1.0)).xyz;
   gl_ClipDistance[0] = dot(worldPos - clipPlaneOrigin, clipPlaneNormal);

   gl_Position = matProj * matView * matWorld * vec4(vPosition, 1.0);
   fNormal = (matWorld * vec4(vNormal, 0.0)).xyz;
   fUV = vUV;
}