#pragma once

#include <model/geo/plane.h>
#include <memory>
#include <view/GenericVertex.h>

namespace view
{
	namespace raw
	{
		class PlaneGenerator
		{
		public:
			static view::GenericMesh getGenericMeshData(
				const model::geo::Plane& plane,
				const glm::vec3& roughUpDir,
				const glm::vec2& dimensions
			);
		};
	}
}