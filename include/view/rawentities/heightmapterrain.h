#pragma once

#include <model/specialgeo/heightfield.h>
#include <view/GenericVertex.h>
#include <util/Logger.h>
#include <memory>

namespace view
{
	namespace raw
	{
		class HeightmapTerrainEntity
		{
		public:
			HeightmapTerrainEntity(
				std::shared_ptr<model::specialgeo::Heightfield> heightField
			);
			~HeightmapTerrainEntity() = default;
			HeightmapTerrainEntity(const HeightmapTerrainEntity&) = delete;

			std::shared_ptr<view::GenericMesh> getMeshData(unsigned int rows, unsigned int cols, float defaultHeight, bool blend = true) const;

		private:
			std::shared_ptr<model::specialgeo::Heightfield> heightField_;
		};
	}
}