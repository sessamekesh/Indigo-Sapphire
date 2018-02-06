#include <util/surfacemask/heightmapmask.h>

namespace util
{
	HeightmapMask::HeightmapMask(
		std::shared_ptr<model::specialgeo::Heightfield> heightfield,
		float minHeight,
		float maxHeight
	)
		: heightfield_(heightfield)
		, minHeight_(minHeight)
		, maxHeight_(maxHeight)
	{}

	bool HeightmapMask::getMaskValue(const glm::vec2& pos)
	{
		if (pos.x < -heightfield_->width() || pos.x > heightfield_->width())
		{
			return false;
		}
		else if (pos.y < -heightfield_->depth() || pos.y > heightfield_->depth())
		{
			return false;
		}
		else
		{
			float sampledHeight = heightfield_->heightAtPos(pos.x, pos.y, maxHeight_ + 50.f, true);
			return sampledHeight >= minHeight_ && sampledHeight <= maxHeight_;
		}
	}
}