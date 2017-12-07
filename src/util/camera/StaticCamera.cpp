#include <util/camera/StaticCamera.h>

namespace util
{
	namespace camera
	{
		StaticCamera::StaticCamera(
			const glm::vec3& pos,
			const glm::vec3& lookAt,
			const glm::vec3& up
		)
			: pos_(pos)
			, lookAt_(lookAt)
			, up_(up)
			, isDirty_(true)
			, matView_()
		{}

		const glm::mat4& StaticCamera::getViewTransform()
		{
			if (isDirty_)
			{
				matView_ = glm::lookAt(pos_, lookAt_, up_);

				isDirty_ = false;
			}
			return matView_;
		}
	}
}