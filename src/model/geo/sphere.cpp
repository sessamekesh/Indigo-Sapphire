#include <model/geo/sphere.h>

namespace model
{
	namespace geo
	{
		Sphere::Sphere(const glm::vec3& origin, float radius)
			: origin_(origin)
			, radius_(radius)
		{}

		glm::vec3 Sphere::origin() const
		{
			return origin_;
		}

		float Sphere::radius() const
		{
			return radius_;
		}
	}
}