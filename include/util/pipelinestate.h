#pragma once

// Collection of "globals" that are used to keep track of important OpenGL
//  pipeline state - mostly around what is currently bound.

namespace util
{
	class PipelineState
	{
	public:
		PipelineState();

	public:
		unsigned int BOUND_VERTEX_ATTRIB_PTR;

	public:
		void enableDepthTest();
		void disableDepthTest();
		void enableDepthMask();
		void disableDepthMask();
		void enableBlending();
		void disableBlending();

	protected:
		bool isDepthTestEnabled_;
		bool isDepthMaskOn_;
		bool isBlendingEnabled_;
	};
}