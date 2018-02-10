#include <model/specialgeo/projection/perspectiveprojection.h>

namespace model
{
	namespace specialgeo
	{
		PerspectiveProjection::PerspectiveProjection(
			float fovy,
			float aspectRatio,
			float nearPlane,
			float farPlane
		)
			: fovy_(fovy)
			, aspectRatio_(aspectRatio)
			, nearPlane_(nearPlane)
			, farPlane_(farPlane)
			, projMatrix_(glm::perspective(fovy, aspectRatio, nearPlane, farPlane))
		{}

		glm::mat4 PerspectiveProjection::getProjectionMatrix() const
		{
			return projMatrix_;
		}

		std::vector<model::geo::Plane> PerspectiveProjection::getBoundingFrustum(std::shared_ptr<util::camera::CameraBase> camera) const
		{
			auto cameraPos = camera->pos();
			auto cameraFwd = glm::normalize(camera->lookAt() - cameraPos);
			auto cameraUp = camera->up();
			
			auto front = model::geo::Plane(cameraPos + cameraFwd * nearPlane_, cameraFwd);
			auto back = model::geo::Plane(cameraPos + cameraFwd * farPlane_, -cameraFwd);

			// TODO SESS: Also put in left, right, up, and down clipping planes.
			// For the perspective transformation, that may be tricky-ish.

			return { front, back };
		}
	}
}