#version 400

in VertexData
{
   vec2 vUV;
} VertexIn;

layout (location = 0) out vec4 color;

uniform sampler2D textureAtlas;

float pxRange = 4.0; // TODO SESS: Make this configurable.

uniform uint textureAtlasRows;
uniform uint textureAtlasCols;
uniform vec4 textColor;

// https://github.com/Chlumsky/msdfgen
float median(float r, float g, float b)
{
   return max(min(r, g), min(max(r, g), b));
}

void main()
{
   vec2 msdfUnit = pxRange / vec2((textureSize(textureAtlas, 0)));
   vec3 sampleColor = texture(textureAtlas, VertexIn.vUV).rgb;
   float sigDist = median(sampleColor.r, sampleColor.g, sampleColor.b);
   float w = fwidth(sigDist);
   float opacity = smoothstep(0.5 - w, 0.5 + w, sigDist);
   if (opacity < 0.1)
   {
      discard;
   }
   else
   {
      color = textColor;
   }
}