#pragma once

// Collection of "globals" that are used to keep track of important OpenGL
//  pipeline state - mostly around what is currently bound.

namespace util
{
	struct PipelineState
	{
	public:
		unsigned int BOUND_VERTEX_ATTRIB_PTR = 0u;
	};
}