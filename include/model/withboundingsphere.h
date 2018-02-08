#pragma once

#include <model/geo/sphere.h>

namespace model
{
	class WithBoundingSphere
	{
	public:
		virtual model::geo::Sphere getBoundingSphere() = 0;
	};
}