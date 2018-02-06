#include <util/surfacemask/combinationmask.h>

namespace util
{
	CombinationMask::CombinationMask(const std::vector<std::shared_ptr<SurfaceMaskBase>>& masks)
		: masks_(masks)
	{}

	bool CombinationMask::getMaskValue(const glm::vec2& pos)
	{
		bool r = true;
		for (auto&& m : masks_)
		{
			r &= m->getMaskValue(pos);
		}
		return r;
	}
}