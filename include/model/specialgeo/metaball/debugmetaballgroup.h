#pragma once

// Debug metaball group
// Creates paths and stuff that are good for looking at metaballs, but not actually
//  something that's very interesting.

#include "metaballgroup.h"
#include "boxboundedmetaballpath.h"

namespace model
{
	namespace specialgeo
	{
		namespace metaball
		{
			class DebugMetaballGroup : public MetaballGroup
			{
			public:
				DebugMetaballGroup(
					const glm::vec3& pos,
					const glm::quat& rot,
					const glm::vec3& scl,
					const glm::vec3& lowerBound,
					const glm::vec3& upperBound,
					unsigned int numMetaballs,
					float radii
				);
				~DebugMetaballGroup() = default;
				DebugMetaballGroup(const DebugMetaballGroup&) = default;
			};
		}
	}
}