uniform mat4 matWorld;
uniform mat4 matView;
uniform mat4 matProj;

attribute vec3 vPos;

varying vec3 fTexCoords;

void main()
{
	gl_Position = matProj * matView * matWorld * vec4(vPos, 1.0);
    fTexCoords = (vec4(vPos, 1.0)).xyz;
}