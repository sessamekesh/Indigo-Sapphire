#pragma once

#include <model/geo/plane.h>
#include <model/geo/sphere.h>

namespace model
{
	namespace geo
	{
		bool isSphereTotallyWithinPlane(const model::geo::Plane& p, const model::geo::Sphere& s);
		bool isSphereAtAllInPlane(const model::geo::Plane& p, const model::geo::Sphere& s);
	}
}