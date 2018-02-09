#pragma once

namespace util
{
	namespace math
	{
		class Curve1To1
		{
		public:
			virtual float evaluateAt(float in) const = 0;
		};
	}
}