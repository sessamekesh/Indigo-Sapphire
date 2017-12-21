#include <model/withworldtransform.h>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace model
{
	WithWorldTransform::WithWorldTransform(
		const glm::vec3& pos,
		const glm::quat& rot,
		const glm::vec3& scl
	)
		: pos_(pos)
		, rot_(rot)
		, scl_(scl)
		, isWorldTransformDirty_(true)
		, worldTransform_(1.f)
	{}

	glm::mat4 WithWorldTransform::worldTransform()
	{
		if (isWorldTransformDirty_)
		{
			worldTransform_ = glm::mat4();
			worldTransform_ *= glm::translate(pos_);
			worldTransform_ *= glm::toMat4(rot_);
			worldTransform_ *= glm::scale(scl_);
			isWorldTransformDirty_ = false;
		}

		return worldTransform_;
	}
}