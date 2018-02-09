#pragma once

#include <glm/glm.hpp>

namespace util
{
	class SurfaceProbabilityFieldBase
	{
	public:
		virtual float getProbabilityAtPoint(const glm::vec2& location) = 0;
	};
}