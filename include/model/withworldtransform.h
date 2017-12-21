#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace model
{
	class WithWorldTransform
	{
	public:
		WithWorldTransform(
			const glm::vec3& pos,
			const glm::quat& rot,
			const glm::vec3& scl
		);
		~WithWorldTransform() = default;
		WithWorldTransform(const WithWorldTransform&) = default;

		glm::mat4 worldTransform();

	protected:
		glm::vec3 pos_;
		glm::quat rot_;
		glm::vec3 scl_;
		bool isWorldTransformDirty_;
		glm::mat4 worldTransform_;
	};
}