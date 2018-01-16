#version 400 core

in vec3 fNorm;
in vec2 fUV;

layout(location = 0) out vec4 color;

uniform sampler2D diffuseMap;

uniform vec3 lightAmbient;
uniform vec3 lightDiffuse;
uniform vec3 lightDirection;

// Why is the texture being so finicky?

void main()
{
   vec2 uv = vec2(fUV.x, -fUV.y);

   vec4 cDiffuse = texture(diffuseMap, uv);

   float diffuseFactor = max(dot(-lightDirection, normalize(fNorm)), 0.0);

   //gl_FragColor =
   //   vec4(lightAmbient, 1.0) * cDiffuse
	  //+ vec4(lightDiffuse, 1.0) * cDiffuse * diffuseFactor;
	gl_FragColor = cDiffuse;

   if (gl_FragColor.a < 0.2)
   {
      discard;
   }
}