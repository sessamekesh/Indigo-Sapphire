uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;

varying vec4 fClipSpace;

void main()
{
   // TODO SESS: Is a further conversion required to move from GL clip space into texture space, something like *2-1 or /2+0.5?
   vec2 ndc = (fClipSpace.xy / fClipSpace.w) / 2.0 + 0.5;

   // TODO SESS: Is a sampling from normalized device coordinates about an inverted Y actually required since the camera itself is inverted? SHOULD the camera be inverted?
   vec2 reflectUV = vec2(ndc.x, -ndc.y);
   vec2 refractUV = ndc;

   vec4 reflectColor = texture(reflectionTexture, reflectUV);
   vec4 refractColor = texture(refractionTexture, refractUV);

   gl_FragColor = mix(reflectColor, refractColor, 0.5);
}