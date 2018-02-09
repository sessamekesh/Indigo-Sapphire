#pragma once

#include <util/math/curve1to1.h>

namespace util
{
	namespace math
	{
		class Linear11 : public Curve1To1
		{
		public:
			Linear11(float pt1, float val1, float pt2, float val2);
			Linear11(const Linear11&) = default;
			~Linear11() = default;

			float evaluateAt(float in) const override;

		private:
			float min_;
			float max_;
			float start_;
			float end_;
		};
	}
}