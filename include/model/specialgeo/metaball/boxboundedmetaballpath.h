#pragma once

#include "imetaballpath.h"

namespace model
{
	namespace specialgeo
	{
		namespace metaball
		{
			class BoxBoundedMetaballPath : public IMetaballPath
			{
			public:
				BoxBoundedMetaballPath(
					const glm::vec3& boxMin,
					const glm::vec3& boxMax,
					const glm::vec3& initialVelocity,
					const glm::vec3& initialPosition
				);
				~BoxBoundedMetaballPath() = default;
				BoxBoundedMetaballPath(const BoxBoundedMetaballPath&) = default;

				virtual void update(float dt) override;
				virtual glm::vec3 getRelativePosition() override;

			protected:
				glm::vec3 position_;
				glm::vec3 velocity_;
				glm::vec3 boxMin_;
				glm::vec3 boxMax_;
			};
		}
	}
}