#pragma once

#include "metaball.h"
#include <model/geo/rectangle.h>
#include <vector>
#include <optional>
#include <glm/gtx/quaternion.hpp>
#include <model/withworldtransform.h>

namespace model
{
	namespace specialgeo
	{
		namespace metaball
		{
			class MetaballGroup : public WithWorldTransform
			{
			public:
				// TODO SESS: A metaball group contains a group of metaballs,
				//  and has methods for finding a billboard that can be raytraced
				//  (given the position of an observer, and assuming orthographic projection)
				//  The depth will be at the nearest point of all charges to camera,
				//  and it will be an AABB of all charges (w/radii) projected onto that billboard
				// This will be used for now to render raytraced things with minimal frag passes
				MetaballGroup(
					const glm::vec3& pos,
					const glm::quat& orientation,
					const glm::vec3& scl,
					unsigned int anticipatedNumberOfMetaballs = 1u
				);
				~MetaballGroup() = default;
				MetaballGroup(const MetaballGroup&) = default;

				std::optional<geo::Rectangle> getOrthoBindingRect(const glm::vec3& viewerPos, const glm::vec3& lookAtDirection, const glm::vec3& viewerUp);

				void update(float dt);

				Metaball getMetaball(const int idx) const;
				void removeMetaballAt(const int idx);
				unsigned int addMetaball(const Metaball& metaball);

				std::vector<Charge> getChargeData();

			private:
				std::vector<Metaball> activeMetaballs_;
			};
		}
	}
}