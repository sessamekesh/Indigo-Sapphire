#pragma once

#include <glm/glm.hpp>

namespace util
{
	namespace camera
	{
		class CameraBase
		{
		public:
			CameraBase();
			~CameraBase() = default;
			CameraBase(const CameraBase&) = default;

			virtual const glm::mat4& getViewTransform();

			virtual glm::vec3 pos() const = 0;
			virtual glm::vec3 lookAt() const = 0;
			virtual glm::vec3 up() const = 0;

		protected:
			bool isDirty_;
			glm::mat4 matView_;
		};
	}
}