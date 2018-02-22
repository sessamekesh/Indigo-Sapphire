#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>

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

		void setParent(std::shared_ptr<model::WithWorldTransform> parent);

		void pos(const glm::vec3& pos);
		glm::vec3 pos() const;
		void rot(const glm::quat& rot);
		glm::quat rot() const;
		void scl(const glm::vec3& scl);
		glm::vec3 scl() const;

	protected:
		glm::vec3 pos_;
		glm::quat rot_;
		glm::vec3 scl_;
		bool isWorldTransformDirty_;
		glm::mat4 worldTransform_;

		std::shared_ptr<model::WithWorldTransform> parent_;
	};
}