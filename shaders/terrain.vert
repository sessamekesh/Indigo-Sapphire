#version 330 core

attribute vec3 vPosition;
attribute vec3 vNormal;
attribute vec2 vUV;

varying vec3 fNormal;
varying vec2 fUV;

uniform mat4 matWorld;
uniform mat4 matView;
uniform mat4 matProj;

void main()
{
   gl_Position = matProj * matView * matWorld * vec4(vPosition, 1.0);
   fNormal = (matWorld * vec4(vNormal, 0.0)).xyz;
   fUV = vUV;
}