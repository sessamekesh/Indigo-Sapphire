#pragma once

#include <glm/glm.hpp>
#include <model/imagedata.h>
#include <memory>

namespace model
{
	namespace specialgeo
	{
		class Heightfield
		{
		public:
			Heightfield(
				std::shared_ptr<model::GreyscaleImageData> heightField,
				float worldWidth,
				float worldDepth,
				float maxHeight
			);
			~Heightfield() = default;
			Heightfield(const Heightfield&) = default;

			float heightAtPos(float x, float z, float defaultHeight, bool blend = true) const;
			float width() const;
			float depth() const;

		private:
			std::shared_ptr<model::GreyscaleImageData> heightField_;
			float width_;
			float depth_;
			float maxHeight_;
		};
	}
}