#pragma once

#include <util/surfacemask/surfaceprobabilityfieldbase.h>
#include <model/specialgeo/heightfield.h>
#include <memory>
#include <util/math/curve1to1.h>

namespace util
{
	class HeightmapProbabilityField : public SurfaceProbabilityFieldBase
	{
	public:
		HeightmapProbabilityField(
			std::shared_ptr<model::specialgeo::Heightfield> heightfield,
			std::shared_ptr<util::math::Curve1To1> probabilityCurve
		);
		HeightmapProbabilityField() = default;
		HeightmapProbabilityField(const HeightmapProbabilityField&) = default;

		float getProbabilityAtPoint(const glm::vec2& location) override;

	protected:
		std::shared_ptr<model::specialgeo::Heightfield> heightfield_;
		std::shared_ptr<util::math::Curve1To1> probabilityCurve_;
	};
}