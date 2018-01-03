#version 400

uniform mat4 matWorld;
uniform mat4 matView;
uniform mat4 matProj;

uniform vec3 clipPlaneOrigin;
uniform vec3 clipPlaneNormal;

layout (location = 0) in vec3 vPos;

out vec3 fTexCoords;

void main()
{
    gl_ClipDistance[0] = dot(vPos - clipPlaneOrigin, clipPlaneNormal);
	gl_Position = matProj * matView * matWorld * vec4(vPos, 1.0);
    fTexCoords = (vec4(vPos, 1.0)).xyz;
}