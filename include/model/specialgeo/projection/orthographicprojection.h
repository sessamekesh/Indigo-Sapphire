#pragma once

#include <model/specialgeo/projection/projectionbase.h>
#include <glm/gtc/matrix_transform.hpp>

namespace model
{
	namespace specialgeo
	{
		class OrthographicProjection : public ProjectionBase
		{
		public:
			OrthographicProjection(
				float left,
				float right,
				float bottom,
				float top,
				float near,
				float far
			);
			~OrthographicProjection() = default;
			OrthographicProjection(const OrthographicProjection&) = default;

			glm::mat4 getProjectionMatrix() const override;
			std::vector<model::geo::Plane> getBoundingFrustum(std::shared_ptr<util::camera::CameraBase> camera) const override;

		protected:
			float left_;
			float right_;
			float bottom_;
			float top_;
			float near_;
			float far_;

			glm::mat4 projMatrix_;
		};
	}
}