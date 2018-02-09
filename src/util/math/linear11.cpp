#include <util/math/linear11.h>
#include <algorithm>

namespace util
{
	namespace math
	{
		Linear11::Linear11(float pt1, float val1, float pt2, float val2)
			: min_(std::min(pt1, pt2))
			, max_(std::max(pt1, pt2))
			, start_(pt1 < pt2 ? val1 : val2)
			, end_(pt1 < pt2 ? val2 : val1)
		{}

		float Linear11::evaluateAt(float in) const
		{
			float t = (in - min_) / (max_ - min_);
			if (t < 0.f)
			{
				return start_;
			}
			else if (t > 1.f)
			{
				return end_;
			}
			else
			{
				return start_ + (end_ - start_) * t;
			}
		}
	}
}