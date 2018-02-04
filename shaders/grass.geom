#version 330

//
// http://www.mbsoftworks.sk/index.php?page=tutorials&series=1&tutorial=32
//

layout(points) in;
layout(triangle_strip, max_vertices=12) out;

uniform mat4 matWorld;
uniform mat4 matView;
uniform mat4 matProj;

uniform float minPatchHeight;
uniform float maxPatchHeight;
uniform float patchSize;
uniform float windStrength;

uniform vec3 clipPlaneOrigin;
uniform vec3 clipPlaneNormal;

smooth out vec2 vTexCoord;
smooth out vec3 vWorldPos;
smooth out vec4 vEyeSpacePos;

uniform float fTimePassed;

mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

vec3 vLocalSeed;

// This function returns random number from zero to one
float randZeroOne()
{
    uint n = floatBitsToUint(vLocalSeed.y * 214013.0 + vLocalSeed.x * 2531011.0 + vLocalSeed.z * 141251.0);
    n = n * (n * n * 15731u + 789221u);
    n = (n >> 9u) | 0x3F800000u;
 
    float fRes =  2.0 - uintBitsToFloat(n);
    vLocalSeed = vec3(vLocalSeed.x + 147158.0 * fRes, vLocalSeed.y*fRes  + 415161.0 * fRes, vLocalSeed.z + 324154.0*fRes);
    return fRes;
}

int randomInt(int min, int max)
{
	float fRandomFloat = randZeroOne();
	return int(float(min)+fRandomFloat*float(max-min));
}

void main()
{
	mat4 mMV = matView * matWorld;
	mat4 mMVP = matProj * mMV;

	vec3 vGrassFieldPos = gl_in[0].gl_Position.xyz;
	
	float PIover180 = 3.14159/180.0;
	vec3 vBaseDir[3];
	vBaseDir[0] = vec3(1.0, 0.0, 0.0);
	vBaseDir[1] = vec3(float(cos(45*PIover180)), 0.0f, float(sin(45.0*PIover180)));
	vBaseDir[2] = vec3(float(cos(-45.0*PIover180)), 0.0f, float(sin(-45.0*PIover180)));

	vec3 vWindDirection = normalize(vec3(1.0, 0.0, 1.0));
	
	for (int i = 0; i < 3; i++)
	{
		// Grass patch top left vertex
		vec3 vBaseDirRotated = (rotationMatrix(vec3(0.0, 1.0, 0.0), sin(fTimePassed*0.7f)*0.1f)*vec4(vBaseDir[i], 1.0)).xyz;

		vLocalSeed = vGrassFieldPos*float(i);
		int iGrassPatch = randomInt(0, 3);

		float fGrassPatchHeight = minPatchHeight + (randZeroOne() * (maxPatchHeight - minPatchHeight));
		float fTCStartX = float(iGrassPatch) * 0.25;
		float fTCEndX = fTCStartX + 0.25;

		float fWindPower = 0.5f + sin(vGrassFieldPos.x/5 + vGrassFieldPos.z / 5+ fTimePassed * (1.2f + windStrength / 20.0f));
		if (fWindPower < 0.0)
		{
			fWindPower = fWindPower * 0.2f;
		}
		else
		{
			fWindPower = fWindPower * 0.3f;
		}

		fWindPower *= windStrength;

		// Emit patch top left vertex
		vec3 vTL = vGrassFieldPos - vBaseDirRotated * patchSize * 0.5f + vWindDirection * fWindPower;
		vTL.y += fGrassPatchHeight;
		gl_ClipDistance[0] = dot(vTL - clipPlaneOrigin, clipPlaneNormal);
		gl_Position = mMVP * vec4(vTL, 1.0);
		vTexCoord = vec2(fTCStartX, 1.0);
		vWorldPos = vTL;
		vEyeSpacePos = mMV * vec4(vTL, 1.0);
		EmitVertex();

		// Emit patch bottom left vertex
		vec3 vBL = vGrassFieldPos - vBaseDir[i] * patchSize * 0.5f;
		gl_ClipDistance[0] = dot(vBL - clipPlaneOrigin, clipPlaneNormal);
		gl_Position = mMVP * vec4(vBL, 1.0);
		vTexCoord = vec2(fTCStartX, 0.0);
		vWorldPos = vBL;
		vEyeSpacePos = mMV * vec4(vBL, 1.0);
		EmitVertex();

		// Grass patch top right vertex
		vec3 vTR = vGrassFieldPos + vBaseDirRotated*patchSize*0.5f + vWindDirection*fWindPower;
		vTR.y += fGrassPatchHeight;  
		gl_ClipDistance[0] = dot(vTR - clipPlaneOrigin, clipPlaneNormal);
		gl_Position = mMVP*vec4(vTR, 1.0);
		vTexCoord = vec2(fTCEndX, 1.0);
		vWorldPos = vTR;
		vEyeSpacePos = mMV*vec4(vTR, 1.0);
		EmitVertex();
		
		// Grass patch bottom right vertex
		vec3 vBR = vGrassFieldPos + vBaseDir[i]*patchSize*0.5f;  
		gl_ClipDistance[0] = dot(vBR - clipPlaneOrigin, clipPlaneNormal);
		gl_Position = mMVP*vec4(vBR, 1.0);
		vTexCoord = vec2(fTCEndX, 0.0);
		vWorldPos = vBR;
		vEyeSpacePos = mMV*vec4(vBR, 1.0);
		EmitVertex();

		EndPrimitive();
	}
}