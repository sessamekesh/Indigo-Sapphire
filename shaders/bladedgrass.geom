#version 400

layout(points) in;
layout(triangle_strip, max_vertices=8) out;

in VertexData
{
    float fBladeBaseWidth;
	float fBladeHeight;
	float fBladeTaperRate;
	float fRotation;
	float fTwistRate;
} VertexIn[1];

out VertexData
{
    vec2 UV;
	vec3 vPos;
	vec3 vNormal;
} VertexOut;

layout (std140) uniform PerSceneBlockG
{
    mat4 matProj;
	float windStrength;
};

layout (std140) uniform PerFrameBlockG
{
    mat4 matView;
	vec3 clipPlaneOrigin;
	float fTimeElapsed;
	vec3 clipPlaneNormal;
};

layout (std140) uniform PerInstanceBlockG
{
    mat4 matWorld;
};

mat3 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat3(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c         );
}

void main()
{
    vec3 vGrassFieldPos = gl_in[0].gl_Position.xyz;

	vec3 vWindDirection = normalize(vec3(1.0, 0.0, 1.0));

	mat4 mVP = matProj * matView;

	float yLevel = 0.0;
	float rotation = VertexIn[0].fRotation;
	float width = VertexIn[0].fBladeBaseWidth;
	for (int i = 0; i < 4; i++)
	{
	    mat3 matRot = rotationMatrix(vec3(0.0, 1.0, 0.0), rotation);
		float windPower = sin(vGrassFieldPos.x / 5 + vGrassFieldPos.z / 5 + fTimeElapsed + (windStrength / 20.0f)) * 0.6f;

		vec3 up = vec3(0.0, 1.0, 0.0);
		vec3 wind = vWindDirection * windPower * (float(i) / 3);
		vec3 offset = normalize(up + wind) * yLevel;

	    vec3 leftVert = vGrassFieldPos + matRot * vec3(-width, 0.0, 0.0);
		vec3 rightVert = vGrassFieldPos + matRot * vec3(width, 0.0, 0.0);
		leftVert += offset;
		rightVert += offset;

		// Send in left vertex
		vec3 vWorldPos = (matWorld * vec4(leftVert, 1.0)).xyz;
		gl_Position = mVP * vec4(vWorldPos, 1.0);
		VertexOut.vPos = vWorldPos;
		VertexOut.UV = vec2(0.0, float(i) / 3.0);
		VertexOut.vNormal = matRot * vec3(0.0, 0.0, 1.0);
		gl_ClipDistance[0] = dot(vWorldPos - clipPlaneOrigin, clipPlaneNormal);
		EmitVertex();

		// Send in right vertex
		vWorldPos = (matWorld * vec4(rightVert, 1.0)).xyz;
		gl_Position = mVP * vec4(vWorldPos, 1.0);
		VertexOut.vPos = vWorldPos;
		VertexOut.UV = vec2(1.0, float(i) / 3.0);
		VertexOut.vNormal = matRot * vec3(0.0, 0.0, 1.0);
		gl_ClipDistance[0] = dot(vWorldPos - clipPlaneOrigin, clipPlaneNormal);
		EmitVertex();

		// Update values used in calculation
		yLevel += VertexIn[0].fBladeHeight / 3.0;
		rotation += VertexIn[0].fTwistRate;
		width *= (1.0 - VertexIn[0].fBladeTaperRate);
	}
	EndPrimitive();
}