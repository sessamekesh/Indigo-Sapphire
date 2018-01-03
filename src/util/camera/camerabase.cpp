#include <util/camera/camerabase.h>
#include <glm/gtc/matrix_transform.hpp>

namespace util
{
	namespace camera
	{
		CameraBase::CameraBase()
			: isDirty_(true)
			, matView_(glm::mat4(1.f))
		{}

		const glm::mat4& CameraBase::getViewTransform()
		{
			if (isDirty_)
			{
				matView_ = glm::lookAt(pos(), lookAt(), up());
				isDirty_ = false;
			}
			return matView_;
		}
	}
}