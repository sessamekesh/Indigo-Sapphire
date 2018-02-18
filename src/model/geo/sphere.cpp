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

		Sphere Sphere::getCombinedSphere(const model::geo::Sphere& s1, const model::geo::Sphere& s2)
		{
			auto secondFromFirst = glm::normalize(s2.origin() - s1.origin());
			auto farPoint1 = s2.origin() + secondFromFirst * s2.radius();
			auto farPoint2 = s1.origin() - secondFromFirst * s1.radius();

			auto origin = (farPoint1 + farPoint2) * 0.5f;
			auto radius = glm::length(farPoint2 - farPoint1);

			return Sphere(origin, radius);
		}
	}
}