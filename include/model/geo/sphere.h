#pragma once

#include <glm/glm.hpp>

namespace model
{
	namespace geo
	{
		class Sphere
		{
		public:
			Sphere(const glm::vec3& origin, float radius);
			Sphere(const Sphere&) = default;
			~Sphere() = default;

			glm::vec3 origin() const;
			float radius() const;

			static Sphere getCombinedSphere(const model::geo::Sphere& s1, const model::geo::Sphere& s2);

		private:
			glm::vec3 origin_;
			float radius_;
		};
	}
}