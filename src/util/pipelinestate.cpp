#include <util/pipelinestate.h>
#include <glad/glad.h>

namespace util
{
	PipelineState::PipelineState()
		: BOUND_VERTEX_ATTRIB_PTR(0u)
		, isDepthTestEnabled_(false)
		, isDepthMaskOn_(false)
		, isBlendingEnabled_(false)
	{}

	void PipelineState::enableDepthTest()
	{
		if (isDepthTestEnabled_) return;
		glEnable(GL_DEPTH_TEST);
		isDepthTestEnabled_ = true;
	}

	void PipelineState::disableDepthTest()
	{
		if (!isDepthTestEnabled_) return;
		glDisable(GL_DEPTH_TEST);
		isDepthTestEnabled_ = false;
	}

	void PipelineState::enableDepthMask()
	{
		if (isDepthMaskOn_) return;
		glDepthMask(GL_TRUE);
		isDepthMaskOn_ = true;
	}

	void PipelineState::disableDepthMask()
	{
		if (!isDepthMaskOn_) return;
		glDepthMask(GL_FALSE);
		isDepthMaskOn_ = false;
	}

	void PipelineState::enableBlending()
	{
		if (isBlendingEnabled_) return;
		glEnable(GL_BLEND);
		isBlendingEnabled_ = true;
	}

	void PipelineState::disableBlending()
	{
		if (!isBlendingEnabled_) return;
		glDisable(GL_BLEND);
		isBlendingEnabled_ = false;
	}
}