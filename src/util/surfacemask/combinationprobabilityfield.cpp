#include <util/surfacemask/combinationprobabilityfield.h>

namespace util
{
	CombinationProbabilityField::CombinationProbabilityField(
		const std::vector<std::shared_ptr<SurfaceProbabilityFieldBase>>& m
	)
		: masks_(m)
	{}

	float CombinationProbabilityField::getProbabilityAtPoint(const glm::vec2& location)
	{
		float probability = 1.f;

		for (auto&& m : masks_)
		{
			probability *= m->getProbabilityAtPoint(location);
		}

		return probability;
	}
}