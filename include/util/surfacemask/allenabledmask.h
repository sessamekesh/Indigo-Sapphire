#pragma once

#include <util/surfacemaskbase.h>

namespace util
{
	class AllEnabledMask : public SurfaceMaskBase
	{
	public:
		AllEnabledMask() = default;
		AllEnabledMask(const AllEnabledMask&) = default;
		~AllEnabledMask() = default;

		bool getMaskValue(const glm::vec2& pos) override;
	};
}