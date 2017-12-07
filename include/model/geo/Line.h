#pragma once

//
// Approximation of a 1D line as a 3D cylinder in 3D space.
//

#include <glm/vec3.hpp>

namespace model
{
	namespace geo
	{
		class Line
		{
		public:
			Line(const glm::vec3 &ep1, const glm::vec3 &ep2, float radius);
			~Line() = default;
			Line(const Line&) = default;

			glm::vec3 ep1() const;
			glm::vec3 ep2() const;
			float radius() const;

		protected:
			glm::vec3 ep1_;
			glm::vec3 ep2_;
			float radius_;
		};
	}
}