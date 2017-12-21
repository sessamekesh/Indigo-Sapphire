// shadertype=glsl

uniform mat4 matWorld;
uniform mat4 matView;
uniform mat4 matProj;

attribute vec3 vPos;
attribute vec4 vColor;

varying vec4 fColor;

void main()
{
	gl_Position = matProj * matView * matWorld * vec4(vPos, 1.0);
	fColor = vColor;
}