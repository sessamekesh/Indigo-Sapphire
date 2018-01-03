#pragma once

#include <util/camera/camerabase.h>

#include <memory>
#include <model/geo/plane.h>

namespace util
{
	namespace camera
	{
		class PlanarReflectionCamera : public CameraBase
		{
		public:
			PlanarReflectionCamera(
				std::shared_ptr<CameraBase> srcCamera,
				const model::geo::Plane& reflectionPlane
			);
			~PlanarReflectionCamera() = default;
			PlanarReflectionCamera(const PlanarReflectionCamera&) = default;

			const glm::mat4& getViewTransform() override;

			glm::vec3 pos() const override;
			glm::vec3 lookAt() const override;
			glm::vec3 up() const override;

			void reflectionPlane(const model::geo::Plane& reflectionPlane);
			model::geo::Plane reflectionPlane() const;

		protected:
			std::shared_ptr<CameraBase> srcCamera_;
			model::geo::Plane reflectionPlane_;

			glm::vec3 lastSrcPos_;
			glm::vec3 lastSrcUp_;
			glm::vec3 lastSrcLookAt_;
		};
	}
}