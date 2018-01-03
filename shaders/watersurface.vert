attribute vec3 vPos;

uniform mat4 matWorld;
uniform mat4 matView;
uniform mat4 matProj;

varying vec4 fClipSpace;

void main()
{
   fClipSpace = matProj * matView * matWorld * vec4(vPos, 1.0);
   gl_Position = fClipSpace;
}