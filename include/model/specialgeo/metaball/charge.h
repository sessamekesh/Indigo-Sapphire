#pragma once

#include <glm/glm.hpp>

namespace model
{
	namespace specialgeo
	{
		namespace metaball
		{
			// TODO SESS:
			// - Make a "metaball (spawner/group)" interface that creates metaballs and their paths
			// - Make a "metaball" class that uses a spawner to maintain a group of metaballs
			// - Make a Blinn-Phong metaball shader (for testing) and a view Metaball class
			// - Add it all to the test scene
			struct Charge
			{
				glm::vec3 position;
				float radius;
			};
		}
	}
}