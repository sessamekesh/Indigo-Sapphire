attribute vec3 vPos;

uniform mat4 matWorld;
uniform mat4 matView;
uniform mat4 matProj;

uniform float tilingStrength;

varying vec4 fClipSpace;
varying vec2 fUV;
varying vec3 fWorldPos;

void main()
{
   fWorldPos = (matWorld * vec4(vPos, 1.0)).xyz;
   fUV = vec2(vPos.x, vPos.z) * tilingStrength;
   fClipSpace = matProj * matView * matWorld * vec4(vPos, 1.0);
   gl_Position = fClipSpace;
}