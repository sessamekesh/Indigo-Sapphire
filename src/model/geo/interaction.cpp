#include <model/geo/interactions.h>

namespace model
{
	namespace geo
	{
		bool isSphereTotallyWithinPlane(const model::geo::Plane& p, const model::geo::Sphere& s)
		{
			auto toSphere = s.origin() - p.origin();
			return glm::dot(toSphere, p.normal()) >= s.radius();
		}

		bool isSphereAtAllInPlane(const model::geo::Plane& p, const model::geo::Sphere& s)
		{
			auto toSphere = s.origin() - p.origin();
			return glm::dot(toSphere, p.normal()) >= -s.radius();
		}
	}
}