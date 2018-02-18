#pragma once

#include <model/specialgeo/projection/projectionbase.h>
#include <glm/gtc/matrix_transform.hpp>

namespace model
{
	namespace specialgeo
	{
		class PerspectiveProjection : public ProjectionBase
		{
		public:
			PerspectiveProjection(
				float fovy,
				float aspectRatio,
				float nearPlane,
				float farPlane
			);
			~PerspectiveProjection() = default;
			PerspectiveProjection(const PerspectiveProjection&) = default;

			glm::mat4 getProjectionMatrix() const override;
			std::vector<model::geo::Plane> getBoundingFrustum(std::shared_ptr<util::camera::CameraBase> camera) const override;

		protected:
			float fovy_;
			float fovx_;
			float aspectRatio_;
			float nearPlane_;
			float farPlane_;

			glm::mat4 projMatrix_;
		};
	}
}