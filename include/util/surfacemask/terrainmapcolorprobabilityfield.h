#pragma once

#include <util/surfacemask/surfaceprobabilityfieldbase.h>
#include <model/imagedata.h>
#include <memory>
#include <util/colorcomponentenum.h>
#include <util/math/curve1to1.h>

namespace util
{
	class TerrainMapColorProbabilityField : public SurfaceProbabilityFieldBase
	{
	public:
		TerrainMapColorProbabilityField(
			std::shared_ptr<model::ImageData> image,
			float width, float depth,
			std::shared_ptr<util::math::Curve1To1> probabilityCurve,
			std::vector<COLOR_COMPONENT> componentsToSample,
			std::vector<COLOR_COMPONENT> componentsToConsider,
			bool normalizeBeforeSampling = true
		);
		TerrainMapColorProbabilityField(const TerrainMapColorProbabilityField&) = default;
		~TerrainMapColorProbabilityField() = default;

		float getProbabilityAtPoint(const glm::vec2& location) override;

	protected:
		std::shared_ptr<model::ImageData> image_;
		float width_;
		float depth_;
		std::shared_ptr<util::math::Curve1To1> curve_;
		std::vector<COLOR_COMPONENT> componentsToSample_;
		std::vector<COLOR_COMPONENT> componentsToConsider_;
		bool normalizeBeforeSampling_;
	};
}