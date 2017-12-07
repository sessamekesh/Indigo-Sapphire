#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace util
{
	namespace camera
	{
		class StaticCamera
		{
		public:
			StaticCamera(
				const glm::vec3& pos,
				const glm::vec3& lookAt,
				const glm::vec3& up
			);
			~StaticCamera() = default;
			StaticCamera(const StaticCamera&) = default;

			const glm::mat4& getViewTransform();

		protected:
			glm::vec3 pos_;
			glm::vec3 lookAt_;
			glm::vec3 up_;

			bool isDirty_;
			glm::mat4 matView_;
		};
	}
}