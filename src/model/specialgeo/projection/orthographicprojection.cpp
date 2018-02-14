#include <model/specialgeo/projection/orthographicprojection.h>

namespace model
{
	namespace specialgeo
	{
		OrthographicProjection::OrthographicProjection(
			float left, float right, float bottom, float top, float near, float far
		)
			: left_(left)
			, right_(right)
			, bottom_(bottom)
			, top_(top)
			, near_(near)
			, far_(far)
			, projMatrix_(glm::ortho(left, right, bottom, top, near, far))
		{}

		glm::mat4 OrthographicProjection::getProjectionMatrix() const
		{
			return projMatrix_;
		}

		std::vector<model::geo::Plane> OrthographicProjection::getBoundingFrustum(std::shared_ptr<util::camera::CameraBase> camera) const
		{
			auto cameraPos = camera->pos();
			auto cameraFwd = glm::normalize(camera->lookAt() - cameraPos);
			auto cameraGeneralUp = camera->up();
			auto cameraRight = glm::normalize(glm::cross(cameraFwd, cameraGeneralUp)); // TODO SESS: Is this right?
			auto cameraUp = glm::normalize(glm::cross(cameraRight, cameraFwd)); // TODO SESS: Is this right?

			auto front = model::geo::Plane(cameraPos + cameraFwd * near_, cameraFwd);
			auto back = model::geo::Plane(cameraPos + cameraFwd * far_, -cameraFwd);
			auto left = model::geo::Plane(cameraPos - cameraRight * left_, cameraRight);
			auto right = model::geo::Plane(cameraPos + cameraRight * right_, -cameraRight);
			auto top = model::geo::Plane(cameraPos + cameraUp * top_, -cameraUp);
			auto bot = model::geo::Plane(cameraPos - cameraUp * top_, cameraUp);

			return { front, left, right, top, bot };
		}
	}
}