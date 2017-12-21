#include <model/specialgeo/metaball/boxboundedmetaballpath.h>

namespace model
{
	namespace specialgeo
	{
		namespace metaball
		{
			BoxBoundedMetaballPath::BoxBoundedMetaballPath(
				const glm::vec3& boxMin,
				const glm::vec3& boxMax,
				const glm::vec3& initialVelocity,
				const glm::vec3& initialPosition
			)
				: boxMin_(boxMin)
				, boxMax_(boxMax)
				, position_(initialPosition)
				, velocity_(initialVelocity)
			{}

			void BoxBoundedMetaballPath::update(float dt)
			{
				position_ += velocity_ * dt;
				if (position_.x > boxMax_.x)
				{
					velocity_.x *= -1.f;
					position_.x -= (boxMax_.x - position_.x) * 2.f;
				}

				if (position_.y > boxMax_.y)
				{
					velocity_.y *= -1.f;
					position_.y -= (boxMax_.y - position_.y) * 2.f;
				}

				if (position_.z > boxMax_.z)
				{
					velocity_.z *= -1.f;
					position_.z -= (boxMax_.z - position_.z) * 2.f;
				}

				if (position_.x < boxMin_.x)
				{
					velocity_.x *= -1.f;
					position_.x += (position_.x - boxMin_.x) * 2.f;
				}

				if (position_.y < boxMin_.y)
				{
					velocity_.y *= -1.f;
					position_.y += (position_.y - boxMin_.y) * 2.f;
				}

				if (position_.z < boxMin_.z)
				{
					velocity_.z *= -1.f;
					position_.z += (position_.z - boxMin_.z) * 2.f;
				}
			}

			glm::vec3 BoxBoundedMetaballPath::getRelativePosition()
			{
				return position_;
			}
		}
	}
}