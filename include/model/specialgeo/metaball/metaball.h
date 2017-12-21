#pragma once

#include "charge.h"
#include "imetaballpath.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>

namespace model
{
	namespace specialgeo
	{
		namespace metaball
		{
			class Metaball
			{
			public:
				Metaball(
					float chargeRadius,
					std::shared_ptr<IMetaballPath> path
				);

				~Metaball() = default;
				Metaball(const Metaball&) = default;
				void update(float dt);

				Charge getChargeData(const glm::mat4& worldTransform = glm::mat4(1.f)) const;

				glm::vec3 pos(const glm::mat4& worldTransform = glm::mat4(1.f)) const;
				float radius() const;

			private:
				float radius_;
				std::shared_ptr<IMetaballPath> path_;
			};
		}
	}
}