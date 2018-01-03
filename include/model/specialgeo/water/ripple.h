#pragma once

#include <glm/glm.hpp>

// Water ripples!
//
// When something disrupts the surface of a height field (water), a sphere of disturbance goes across the surface.
// This class models that as a wave disruption, and stores the data required to represent that in a shader.
// The origin is given in 3D world coordinates - the disruption at any given point can be found with this data.

// TODO SESS: Implement in frag shader instead

namespace model
{
	namespace specialgeo
	{
		namespace water
		{
			struct Ripple
			{
				glm::vec3 origin;
				float velocity;
				float initialPower;
				float frequency;
				float damping;
				float deathHeight;
			};
		}
	}
}