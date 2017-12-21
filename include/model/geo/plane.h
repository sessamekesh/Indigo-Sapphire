#pragma once

#include <glm/glm.hpp>
#include <optional>

namespace model
{
	namespace geo
	{
		class Plane
		{
		public:
			Plane(
				const glm::vec3& originPoint,
				const glm::vec3& normal
			);
			Plane(const Plane&) = default;
			~Plane() = default;

			std::optional<glm::vec3> getRayIntersection(const glm::vec3& origin, const glm::vec3& normal) const;

		private:
			glm::vec3 origin_;
			glm::vec3 normal_;
		};
	}
}