#pragma once

#include <glm/glm.hpp>

namespace model
{
	namespace specialgeo
	{
		namespace metaball
		{
			class IMetaballPath
			{
			public:
				virtual void update(float dt) = 0;
				virtual glm::vec3 getRelativePosition() = 0;
			};
		}
	}
}