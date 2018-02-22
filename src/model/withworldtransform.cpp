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
		, parent_(nullptr)
	{}

	glm::mat4 WithWorldTransform::worldTransform()
	{
		if (isWorldTransformDirty_)
		{
			worldTransform_ = glm::mat4();
			worldTransform_ = glm::translate(worldTransform_, pos_);
			worldTransform_ = glm::rotate(worldTransform_, glm::angle(rot_), glm::axis(rot_));
			worldTransform_ = glm::scale(worldTransform_, scl_);
			isWorldTransformDirty_ = false;
		}

		auto tr = worldTransform_;

		if (parent_)
		{
			tr = tr * parent_->worldTransform();
		}

		return tr;
	}

	void WithWorldTransform::pos(const glm::vec3 & pos)
	{
		pos_ = pos;
		isWorldTransformDirty_ = true;
	}

	glm::vec3 WithWorldTransform::pos() const
	{
		return pos_;
	}

	void WithWorldTransform::rot(const glm::quat& rot)
	{
		rot_ = rot;
		isWorldTransformDirty_ = true;
	}

	glm::quat WithWorldTransform::rot() const
	{
		return rot_;
	}

	void WithWorldTransform::scl(const glm::vec3& scl)
	{
		scl_ = scl;
		isWorldTransformDirty_ = true;
	}

	glm::vec3 WithWorldTransform::scl() const
	{
		return scl_;
	}

	void WithWorldTransform::setParent(std::shared_ptr<model::WithWorldTransform> parent)
	{
		parent_ = parent;
	}
}