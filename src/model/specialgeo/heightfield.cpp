#include <model/specialgeo/heightfield.h>
#include <glm/gtx/compatibility.hpp>
#include <algorithm>

namespace model
{
	namespace specialgeo
	{
		Heightfield::Heightfield(
			std::shared_ptr<model::GreyscaleImageData> heightField,
			float worldWidth,
			float worldDepth,
			float maxHeight
		)
			: heightField_(heightField)
			, width_(worldWidth)
			, depth_(worldDepth)
			, maxHeight_(maxHeight)
		{}

		float sample(const model::GreyscaleImageData& map, int x, int y)
		{
			return map.pixels[y * map.width + x] / 255.f;
		}

		float Heightfield::heightAtPos(float x, float z, float defaultHeight, bool blend) const
		{
			float u = (x + width_) / (2.f * width_);
			float v = (z + depth_) / (2.f * depth_);

			if (u < 0.f || u > 1.f || v < 0.f || v > 1.f)
			{
				return defaultHeight;
			}
			else if (!heightField_)
			{
				return defaultHeight;
			}
			else
			{
				unsigned int xpLeft = std::min<unsigned int>(u * heightField_->width, heightField_->width - 1u);
				unsigned int ypTop = std::min<unsigned int>(v * heightField_->height, heightField_->height - 1u);
				float topLeft = sample(*heightField_, xpLeft, ypTop) * maxHeight_;

				if (blend)
				{
					unsigned int xpRight = std::min<unsigned int>(xpLeft + 1u, heightField_->width - 1u);
					float xpBlend = (u * heightField_->width) - xpLeft;

					unsigned int ypBottom = std::min<unsigned int>(ypTop + 1u, heightField_->height - 1u);
					float ypBlend = (v * heightField_->height) - ypTop;

					float topRight = sample(*heightField_, xpRight, ypTop) * maxHeight_;
					float bottomLeft = sample(*heightField_, xpLeft, ypBottom) * maxHeight_;
					float bottomRight = sample(*heightField_, xpRight, ypBottom) * maxHeight_;

					return glm::lerp(
						glm::lerp(topLeft, topRight, xpBlend),
						glm::lerp(bottomLeft, bottomRight, xpBlend),
						ypBlend
					);
				}
				else
				{
					return topLeft;
				}
			}
		}

		float Heightfield::width() const
		{
			return width_;
		}

		float Heightfield::depth() const
		{
			return depth_;
		}
	}
}