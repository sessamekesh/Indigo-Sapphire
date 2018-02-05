#include <util/surfacemask/terrainmapcolormask.h>

namespace util
{
	TerrainMapColorMask::TerrainMapColorMask(
		std::shared_ptr<model::ImageData> image,
		float width,
		float depth,
		float sampleThreshhold,
		std::vector<COLOR_COMPONENT> componentsToSample,
		std::vector<COLOR_COMPONENT> componentsToConsider,
		bool normalizeBeforeSampling
	)
		: image_(image)
		, width_(width)
		, depth_(depth)
		, sampleThreshold_(sampleThreshhold)
		, componentsToSample_(componentsToSample)
		, componentsToConsider_(componentsToConsider)
		, normalizeBeforeSampling_(normalizeBeforeSampling)
	{}

	bool TerrainMapColorMask::getMaskValue(const glm::vec2& pos)
	{
		float u = (pos.x + width_) / (2.f * width_);
		float v = (pos.y + depth_) / (2.f * depth_);

		if (u < 0.f || u > 1.f || v < 0.f || v > 1.f)
		{
			return false;
		}
		else
		{
			unsigned int pxX = u * image_->width;
			unsigned int pxY = v * image_->height;

			float sampleVal = 0.f;

			float r = image_->pixels[(pxY * image_->width + pxX) * 4u] / 255.f;
			float g = image_->pixels[(pxY * image_->width + pxX) * 4u + 1u] / 255.f;
			float b = image_->pixels[(pxY * image_->width + pxX) * 4u + 2u] / 255.f;
			float a = image_->pixels[(pxY * image_->width + pxX) * 4u + 3u] / 255.f;

			for (auto&& c : componentsToSample_)
			{
				switch (c)
				{
				case COLOR_COMPONENT_R: sampleVal += r; break;
				case COLOR_COMPONENT_G: sampleVal += g; break;
				case COLOR_COMPONENT_B: sampleVal += b; break;
				case COLOR_COMPONENT_A: sampleVal += a; break;
				}
			}

			if (!normalizeBeforeSampling_)
			{
				return sampleVal >= sampleThreshold_;
			}

			float considerVal = 0.f;
			for (auto&& c : componentsToConsider_)
			{
				switch (c)
				{
				case COLOR_COMPONENT_R: considerVal += r; break;
				case COLOR_COMPONENT_G: considerVal += g; break;
				case COLOR_COMPONENT_B: considerVal += b; break;
				case COLOR_COMPONENT_A: considerVal += a; break;
				}
			}

			// Nothing to consider? Leave.
			if (considerVal == 0.f)
			{
				return false;
			}

			// Normalize
			float normalizedVal = sampleVal / considerVal;
			return normalizedVal >= sampleThreshold_;
		}
	}
}