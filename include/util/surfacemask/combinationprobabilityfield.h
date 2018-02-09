#pragma once

#include <util/surfacemask/surfaceprobabilityfieldbase.h>
#include <memory>
#include <vector>
#include <memory>

namespace util
{
	class CombinationProbabilityField : public SurfaceProbabilityFieldBase
	{
	public:
		CombinationProbabilityField(
			const std::vector<std::shared_ptr<SurfaceProbabilityFieldBase>>& masks
		);
		~CombinationProbabilityField() = default;
		CombinationProbabilityField(const CombinationProbabilityField&) = default;

		float getProbabilityAtPoint(const glm::vec2& location) override;

	private:
		std::vector<std::shared_ptr<SurfaceProbabilityFieldBase>> masks_;
	};
}