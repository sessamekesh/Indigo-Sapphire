#version 400

layout (location = 0) in vec3 vPos;
layout (location = 1) in float vAtlasX;
layout (location = 2) in float vAtlasY;

out VertexData
{
   vec2 vUV;
} VertexOut;

uniform mat4 matWorld;
uniform mat4 matView;
uniform mat4 matProj;

uniform uint textureAtlasRows;
uniform uint textureAtlasCols;

uniform vec3 clipPlaneNormal;
uniform vec3 clipPlaneOrigin;

void main()
{
   vec4 worldPos = matWorld * vec4(vPos, 1.0);
   gl_ClipDistance[0] = dot(worldPos.xyz - clipPlaneOrigin, clipPlaneNormal);

   VertexOut.vUV = vec2(
      vAtlasX / float(textureAtlasRows),
	  vAtlasY / float(textureAtlasCols)
   );
   gl_Position = matProj * matView * worldPos;
}