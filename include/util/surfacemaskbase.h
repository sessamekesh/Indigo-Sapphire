#pragma once

#include <glm/glm.hpp>

namespace util
{
	class SurfaceMaskBase
	{
	public:
		virtual bool getMaskValue(const glm::vec2& location) = 0;
	};
}