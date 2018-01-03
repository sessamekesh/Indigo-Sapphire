#include <model/specialgeo/metaball/debugmetaballgroup.h>
#include <random>

namespace model
{
	namespace specialgeo
	{
		namespace metaball
		{
			DebugMetaballGroup::DebugMetaballGroup(
				const glm::vec3& pos,
				const glm::quat& rot,
				const glm::vec3& scl,
				const glm::vec3& lowerBound,
				const glm::vec3& upperBound,
				unsigned int numMetaballs,
				float radii
			)
				: MetaballGroup(pos, rot, scl, numMetaballs)
			{
				std::random_device rd;
				std::mt19937 mt(rd());
				std::uniform_real_distribution<float> xdist(0.f, upperBound.x - lowerBound.x);
				std::uniform_real_distribution<float> ydist(0.f, upperBound.y - lowerBound.y);
				std::uniform_real_distribution<float> zdist(0.f, upperBound.z - lowerBound.z);

				for (auto i = 0u; i < numMetaballs; i++)
				{
					addMetaball(
						model::specialgeo::metaball::Metaball(
							radii,
							std::shared_ptr<model::specialgeo::metaball::IMetaballPath>(
								new BoxBoundedMetaballPath(
									lowerBound, upperBound,
									glm::vec3(xdist(mt) / 8.f, ydist(mt) / 3.f, zdist(mt) / 3.f),
									glm::vec3(xdist(mt) + lowerBound.x, ydist(mt) + lowerBound.y, zdist(mt) + lowerBound.z)
								)
							)
						)
					);
				}
			}
		}
	}
}