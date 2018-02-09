#include <util/surfacemask/heightmapprobabilityfield.h>

namespace util
{
	HeightmapProbabilityField::HeightmapProbabilityField(
		std::shared_ptr<model::specialgeo::Heightfield> heightfield,
		std::shared_ptr<util::math::Curve1To1> curve
	)
		: heightfield_(heightfield)
		, probabilityCurve_(curve)
	{}

	float HeightmapProbabilityField::getProbabilityAtPoint(const glm::vec2& location)
	{
		float sampledHeight = heightfield_->heightAtPos(location.x, location.y, 0.f);
		return probabilityCurve_->evaluateAt(sampledHeight);
	}
}