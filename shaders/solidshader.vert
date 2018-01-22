// shadertype=glsl

uniform mat4 matWorld;
uniform mat4 matView;
uniform mat4 matProj;

uniform vec3 clipPlaneOrigin;
uniform vec3 clipPlaneNormal;

attribute vec3 vPos;
attribute vec4 vColor;

varying vec4 fColor;

void main()
{
    vec4 worldPos = matWorld * vec4(vPos, 1.0);
    gl_ClipDistance[0] = dot(worldPos.xyz - clipPlaneOrigin, clipPlaneNormal);
	gl_Position = matProj * matView * worldPos;
	fColor = vColor;
}