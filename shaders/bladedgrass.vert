#version 330

layout (location = 0) in vec3 vPos;
layout (location = 1) in float vfBladeBaseWidth;
layout (location = 2) in float vfBladeHeight;
layout (location = 3) in float vfBladeTaperRate;
layout (location = 4) in float vfRotation;
layout (location = 5) in float vfTwistRate;

out VertexData
{
    float fBladeBaseWidth;
	float fBladeHeight;
	float fBladeTaperRate;
	float fRotation;
	float fTwistRate;
} VertexOut;

void main()
{
    VertexOut.fBladeBaseWidth = vfBladeBaseWidth;
	VertexOut.fBladeHeight = vfBladeHeight;
	VertexOut.fBladeTaperRate = vfBladeTaperRate;
	VertexOut.fRotation = vfRotation;
	VertexOut.fTwistRate = vfTwistRate;
	gl_Position = vec4(vPos, 1.0);
}