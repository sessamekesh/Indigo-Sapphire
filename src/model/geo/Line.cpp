#pragma once

#include <model/geo/Line.h>

namespace model
{
	namespace geo
	{
		Line::Line(const glm::vec3& ep1, const glm::vec3& ep2, float radius)
			: ep1_(ep1)
			, ep2_(ep2)
			, radius_(radius)
		{}

		glm::vec3 Line::ep1() const
		{
			return ep1_;
		}

		glm::vec3 Line::ep2() const
		{
			return ep2_;
		}

		float Line::radius() const
		{
			return radius_;
		}
	}
}