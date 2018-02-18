#include <model/specialgeo/projection/perspectiveprojection.h>
#include <glm/gtc/quaternion.hpp>

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
			, fovx_(2.f * glm::atan(aspectRatio * glm::tan(fovy / 2.f)))
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
			auto cameraRight = glm::normalize(glm::cross(cameraUp, cameraFwd)); // TODO SESS: Is this right?
			
			auto front = model::geo::Plane(cameraPos + cameraFwd * nearPlane_, cameraFwd);
			auto back = model::geo::Plane(cameraPos + cameraFwd * farPlane_, -cameraFwd);

			auto forwardDir = cameraFwd;
			auto halfFovy = fovy_ / 2.f;
			auto halfFovx = fovx_ / 2.f;

			auto leftPoint = glm::angleAxis(-halfFovx, cameraUp) * forwardDir; // TODO SESS: Is this right?
			auto leftNormal = glm::angleAxis(-halfFovx, cameraUp) * cameraRight; // TODO SESS: Is this right?
			auto left = model::geo::Plane(leftPoint + cameraPos, leftNormal);

			auto rightPoint = glm::angleAxis(halfFovx, cameraUp) * forwardDir; // TODO SESS: Is this right?
			auto rightNormal = glm::angleAxis(halfFovx, cameraUp) * -cameraRight; // TODO SESS: Is this right?
			auto right = model::geo::Plane(rightPoint + cameraPos, rightNormal);

			// TODO SESS: Also put in up, and down clipping planes.
			// For the perspective transformation, that may be tricky-ish.

			return { front, back, left, right };
		}
	}
}