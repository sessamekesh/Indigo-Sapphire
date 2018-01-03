#pragma once

#include <util/camera/camerabase.h>

namespace util
{
	namespace camera
	{
		class StaticCamera : public CameraBase
		{
		public:
			StaticCamera(
				const glm::vec3& pos,
				const glm::vec3& lookAt,
				const glm::vec3& up
			);
			~StaticCamera() = default;
			StaticCamera(const StaticCamera&) = default;

			glm::vec3 pos() const override;
			glm::vec3 lookDir(bool normalized = false) const;
			glm::vec3 lookAt() const override;
			glm::vec3 up() const override;

			void pos(const glm::vec3& pos);
			void lookAt(const glm::vec3& lookAt);
			void up(const glm::vec3& up);

		protected:
			glm::vec3 pos_;
			glm::vec3 lookAt_;
			glm::vec3 up_;
		};
	}
}