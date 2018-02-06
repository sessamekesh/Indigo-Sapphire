#pragma once

#include <util/surfacemaskbase.h>
#include <vector>
#include <memory>

namespace util
{
	class CombinationMask : public SurfaceMaskBase
	{
	public:
		CombinationMask(
			const std::vector<std::shared_ptr<SurfaceMaskBase>>& masks
		);
		~CombinationMask() = default;
		CombinationMask(const CombinationMask&) = default;

		bool getMaskValue(const glm::vec2& pos) override;

	private:
		std::vector<std::shared_ptr<SurfaceMaskBase>> masks_;
	};
}