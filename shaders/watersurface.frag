uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;

varying vec4 fClipSpace;
varying vec2 fUV;
varying vec3 fWorldPos;

uniform float scaleFactor;
uniform float dudvSampleOffset;
uniform vec3 cameraPosition;

void main()
{
   vec2 ndc = (fClipSpace.xy / fClipSpace.w) / 2.0 + 0.5;

   vec2 reflectUV = vec2(ndc.x, -ndc.y);
   vec2 refractUV = ndc;

   // TODO SESS: DUDV does need fixing, it looks like crap right now
   vec2 dudv = (texture(dudvMap, fUV + vec2(dudvSampleOffset, 0.0)).rg * 2.0 - 1.0) * scaleFactor;

   reflectUV += dudv;
   reflectUV.x = clamp(reflectUV.x, 0.001, 0.999);
   reflectUV.y = clamp(reflectUV.y, -0.999, -0.001);
   refractUV += dudv;
   refractUV = clamp(refractUV, 0.001, 0.999);
 
   vec4 reflectColor = texture(reflectionTexture, reflectUV);
   vec4 refractColor = texture(refractionTexture, refractUV);

   vec3 toCamera = normalize(cameraPosition - fWorldPos);
   float refractiveFactor = dot(toCamera, vec3(0.0, 1.0, 0.0)); // TODO SESS: Use water normal here instead!

   vec4 rrColor = mix(reflectColor, refractColor, refractiveFactor);
   gl_FragColor = mix(rrColor, vec4(0.0, 0.3, 0.5, 1.0), 0.2);
   //gl_FragColor = texture(dudvMap, fUV + vec2(dudvSampleOffset, 0.0));
}