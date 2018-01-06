uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;

varying vec4 fClipSpace;
varying vec2 fUV;
varying vec3 fWorldPos;

uniform float scaleFactor;
uniform float dudvSampleOffset;
uniform float shineDamper;
uniform float reflectivity;

uniform vec3 cameraPosition;

uniform vec3 lightColor;
uniform vec3 lightDir;
uniform vec4 quatSurfaceOrientation;

// https://twistedpairdevelopment.wordpress.com/2013/02/11/rotating-a-vector-by-a-quaternion-in-glsl/
vec3 rotate_vector(vec4 quat, vec3 vec)
{
   return vec + 2.0 * cross( cross( vec, quat.xyz ) + quat.w * vec, quat.xyz );
}

void main()
{
   vec2 ndc = (fClipSpace.xy / fClipSpace.w) / 2.0 + 0.5;

   vec2 reflectUV = vec2(ndc.x, -ndc.y);
   vec2 refractUV = ndc;

   // https://www.youtube.com/watch?v=7T5o4vZXAvI
   vec2 dudv = texture(dudvMap, vec2(fUV.x + dudvSampleOffset, fUV.y)).rg * 0.1;
   dudv = fUV + vec2(dudv.x, dudv.y + dudvSampleOffset);
   vec2 totalDistortion = (texture(dudvMap, dudv).rg * 2.0 - 1.0) * scaleFactor;

   reflectUV += totalDistortion;
   reflectUV.x = clamp(reflectUV.x, 0.001, 0.999);
   reflectUV.y = clamp(reflectUV.y, -0.999, -0.001);
   refractUV += totalDistortion;
   refractUV = clamp(refractUV, 0.001, 0.999);
 
   vec4 reflectColor = texture(reflectionTexture, reflectUV);
   vec4 refractColor = texture(refractionTexture, refractUV);

   vec3 toCamera = normalize(cameraPosition - fWorldPos);
   float refractiveFactor = dot(toCamera, vec3(0.0, 1.0, 0.0)); // TODO SESS: Use water normal here instead!

   vec4 rrColor = mix(reflectColor, refractColor, refractiveFactor);

   vec4 normalMapColor = texture(normalMap, totalDistortion);
   vec3 normal = normalize(rotate_vector(quatSurfaceOrientation, vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b, normalMapColor.g * 2.0 - 1.0)));

   vec3 reflectedLight = reflect(normalize(-lightDir), normal);
   float specular = max(dot(reflectedLight, toCamera), 0.0);
   specular = pow(specular, shineDamper);
   vec3 specularHighlights = lightColor * specular * reflectivity;

   gl_FragColor = mix(rrColor, vec4(0.0, 0.3, 0.5, 1.0), 0.2) + vec4(specularHighlights, 0.0);
   //gl_FragColor = texture(dudvMap, fUV + vec2(dudvSampleOffset, 0.0));
}