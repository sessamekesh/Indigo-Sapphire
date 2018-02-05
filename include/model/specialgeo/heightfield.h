#pragma once

#include <glm/glm.hpp>

namespace model
{
	namespace specialgeo
	{
		class Heightfield
		{
		public:
			virtual float heightAtPos(float x, float z, float defaultHeight, bool blend = true) const = 0;
			virtual float width() const = 0;
			virtual float depth() const = 0;
		};
	}
}