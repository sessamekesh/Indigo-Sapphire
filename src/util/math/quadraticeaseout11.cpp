#include <util/math/quadraticeaseout11.h>

namespace util
{
	namespace math
	{
		QuadraticEaseOut11::QuadraticEaseOut11(float min, float max)
			: min_(min)
			, max_(max)
		{}

		float QuadraticEaseOut11::evaluateAt(float in) const
		{
			float t = (in - min_) / (max_ - min_);
			if (t <= 0.f)
			{
				return 1.f;
			}
			else if (t >= 1.f)
			{
				return 0.f;
			}
			else
			{
				return 1.f - t * t;
			}
		}
	}
}