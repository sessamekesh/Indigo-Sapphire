#pragma once

#include <util/math/curve1to1.h>

namespace util
{
	namespace math
	{
		class QuadraticEaseOut11 : public Curve1To1
		{
		public:
			QuadraticEaseOut11(float min, float max);
			QuadraticEaseOut11(const QuadraticEaseOut11&) = default;
			~QuadraticEaseOut11() = default;

			float evaluateAt(float in) const override;

		private:
			float min_;
			float max_;
		};
	}
}