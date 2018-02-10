#pragma once

#include <glm/glm.hpp>
#include <model/geo/plane.h>
#include <vector>
#include <util/camera/camerabase.h>
#include <memory>

namespace model
{
	namespace specialgeo
	{
		class ProjectionBase
		{
		public:
			virtual glm::mat4 getProjectionMatrix() const = 0;
			virtual std::vector<model::geo::Plane> getBoundingFrustum(std::shared_ptr<util::camera::CameraBase> camera) const = 0;
		};
	}
}