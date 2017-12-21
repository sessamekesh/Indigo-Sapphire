#include <model/geo/plane.h>

namespace model
{
	namespace geo
	{
		Plane::Plane(
			const glm::vec3& originPoint,
			const glm::vec3& normal
		)
			: origin_(originPoint)
			, normal_(normal)
		{}

		std::optional<glm::vec3> Plane::getRayIntersection(const glm::vec3& origin, const glm::vec3& normal) const
		{
			// Find some value "t" such that (RayOrigin + RayNormal * t - PlaneOrigin) * PlaneNormal = 0
			// i.e., find a point B along the given ray such that the vector from the plane origin to B
			//  is perpendicular to the plane normal (meaning it is on the plane)
			// That above equation can be re-written:
			// (1) RayOrigin * PlaneNormal + RayNormal * PlaneNormal * t - PlaneOrigin * PlaneNormal = 0
			// (2) RayNormal * PlaneNormal * t = PlaneOrigin * PlaneNormal - RayOrigin * PlaneNormal
			// (3) t = (PlaneOrigin * PlaneNormal - RayOrigin * RayNormal) / (RayNormal * PlaneNormal)

			// If the divisor (RayNormal * PlaneNormal) is zero, that means the ray is parallel to the plane - no answer
			const float divisor = glm::dot(normal_, normal);
			if (divisor == 0.f)
			{
				// The ray is parallel to the plane. They will never intersect.
				return {};
			}

			const float t = (glm::dot(origin_, normal_) - glm::dot(origin, normal)) / divisor;

			return { origin + normal * t };
		}
	}
}