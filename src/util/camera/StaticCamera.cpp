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
			: CameraBase()
			, pos_(pos)
			, lookAt_(lookAt)
			, up_(up)
		{}

		glm::vec3 StaticCamera::pos() const
		{
			return pos_;
		}

		glm::vec3 StaticCamera::lookDir(bool normalized) const
		{
			if (normalized)
			{
				return glm::normalize(lookAt_ - pos_);
			}
			else
			{
				return lookAt_ - pos_;
			}
		}

		glm::vec3 StaticCamera::lookAt() const
		{
			return lookAt_;
		}

		glm::vec3 StaticCamera::up() const
		{
			return up_;
		}

		void StaticCamera::pos(const glm::vec3& pos)
		{
			pos_ = pos;
			isDirty_ = true;
		}

		void StaticCamera::lookAt(const glm::vec3& la)
		{
			lookAt_ = la;
			isDirty_ = true;
		}

		void StaticCamera::up(const glm::vec3& up)
		{
			up_ = up;
			isDirty_ = true;
		}
	}
}