#include <model/specialgeo/heightfield/reducedheightfield.h>
#include <glm/gtx/compatibility.hpp>
#include <algorithm>

namespace model
{
	namespace specialgeo
	{
		ReducedHeightfield::ReducedHeightfield(
			std::shared_ptr<Heightfield> base,
			std::uint32_t xDivisions,
			std::uint32_t zDivisions
		)
			: base_(base)
			, xDivisions_(xDivisions)
			, zDivisions_(zDivisions)
		{}

		float ReducedHeightfield::heightAtPos(float x, float z, float defaultHeight, bool blend) const
		{
			const float baseWidth = base_->width();
			const float baseDepth = base_->depth();

			if (x < -baseWidth || x > baseWidth)
			{
				return base_->heightAtPos(x, z, defaultHeight, blend);
			}
			else
			{
				const float u = (x + baseWidth) / (2.f * baseWidth);
				const float v = (z + baseDepth) / (2.f * baseDepth);

				unsigned int leftSubdivision = u * xDivisions_;
				unsigned int topSubdivision = v * zDivisions_;

				const float leftU = (float)leftSubdivision / xDivisions_;
				const float topV = (float)topSubdivision / zDivisions_;

				float topLeft = base_->heightAtPos(
					leftU * base_->width() * 2.f - base_->width(),
					topV * base_->depth() * 2.f - base_->depth(),
					defaultHeight, blend
				);

				if (blend)
				{
					unsigned int rightSubdivision = leftSubdivision + 1u;
					unsigned int bottomSubdivision = topSubdivision + 1u;

					const float rightU = (float)rightSubdivision / xDivisions_;
					const float bottomV = (float)bottomSubdivision / zDivisions_;

					float xpBlend = (u - leftU) / (rightU - leftU);
					float zpBlend = (v - topV) / (bottomV - topV);

					float topRight = base_->heightAtPos(
						rightU * base_->width() * 2.f - base_->width(),
						topV * base_->depth() * 2.f - base_->depth(),
						defaultHeight, blend
					);
					float bottomLeft = base_->heightAtPos(
						leftU * base_->width() * 2.f - base_->width(),
						bottomV * base_->depth() * 2.f - base_->depth(),
						defaultHeight, blend
					);
					float bottomRight = base_->heightAtPos(
						rightU * base_->width() * 2.f - base_->width(),
						bottomV * base_->depth() * 2.f - base_->depth(),
						defaultHeight, blend
					);

					return glm::lerp(
						glm::lerp(topLeft, topRight, xpBlend),
						glm::lerp(bottomLeft, bottomRight, xpBlend),
						zpBlend
					);
				}
				else
				{
					return topLeft;
				}
			}
		}

		float ReducedHeightfield::width() const
		{
			return base_->width();
		}

		float ReducedHeightfield::depth() const
		{
			return base_->depth();
		}
	}
}