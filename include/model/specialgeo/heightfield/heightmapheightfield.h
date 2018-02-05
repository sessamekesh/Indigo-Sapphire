#pragma once

#include <model/specialgeo/heightfield.h>
#include <model/imagedata.h>
#include <memory>

namespace model
{
	namespace specialgeo
	{
		class HeightmapHeightfield : public Heightfield
		{
		public:
			HeightmapHeightfield(
				std::shared_ptr<model::GreyscaleImageData> heightField,
				float worldWidth,
				float worldDepth,
				float maxHeight
			);
			~HeightmapHeightfield() = default;
			HeightmapHeightfield(const HeightmapHeightfield&) = default;

			float heightAtPos(float x, float z, float defaultHeight, bool blend = true) const override;
			float width() const override;
			float depth() const override;

		private:
			std::shared_ptr<model::GreyscaleImageData> heightField_;
			float width_;
			float depth_;
			float maxHeight_;
		};
	}
}