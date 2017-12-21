#pragma once

#include <model/specialgeo/metaball/metaball.h>

namespace model
{
	namespace specialgeo
	{
		namespace metaball
		{
			Metaball::Metaball(
				float chargeRadius,
				std::shared_ptr<IMetaballPath> path
			)
				: radius_(chargeRadius)
				, path_(path)
			{}

			void Metaball::update(float dt)
			{
				path_->update(dt);
			}

			Charge Metaball::getChargeData(const glm::mat4& worldTransform) const
			{
				glm::vec3 pos = worldTransform * glm::vec4(path_->getRelativePosition(), 1.f);
				auto sclvec = worldTransform * glm::vec4(1.f, 1.f, 1.f, 0.f);
				float radius = radius_ * (sclvec.x + sclvec.y + sclvec.z) * 0.33333;

				return { pos, radius };
			}

			glm::vec3 Metaball::pos(const glm::mat4& worldTransform) const
			{
				return worldTransform * glm::vec4(path_->getRelativePosition(), 1.f);
			}

			float Metaball::radius() const
			{
				return radius_;
			}
		}
	}
}