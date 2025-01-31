#pragma once

#include <util/surfacemaskbase.h>
#include <model/imagedata.h>
#include <memory>
#include <util/colorcomponentenum.h>

namespace util
{
	class TerrainMapColorMask : public SurfaceMaskBase
	{
	public:
		TerrainMapColorMask(
			std::shared_ptr<model::ImageData> image,
			float width,
			float depth,
			float sampleThreshhold,
			std::vector<COLOR_COMPONENT> componentsToSample,
			std::vector<COLOR_COMPONENT> componentsToConsider,
			bool normalizeBeforeSampling = true
		);
		TerrainMapColorMask(const TerrainMapColorMask&) = default;
		~TerrainMapColorMask() = default;

		bool getMaskValue(const glm::vec2& pos) override;

	protected:
		std::shared_ptr<model::ImageData> image_;
		float width_;
		float depth_;
		float sampleThreshold_;
		std::vector<COLOR_COMPONENT> componentsToSample_;
		std::vector<COLOR_COMPONENT> componentsToConsider_;
		bool normalizeBeforeSampling_;
	};
}