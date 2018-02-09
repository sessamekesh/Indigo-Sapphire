#include <util/surfacemask/terrainmapcolorprobabilityfield.h>

namespace util
{
	TerrainMapColorProbabilityField::TerrainMapColorProbabilityField(
		std::shared_ptr<model::ImageData> image,
		float width, float depth,
		std::shared_ptr<util::math::Curve1To1> probabilityCurve,
		std::vector<COLOR_COMPONENT> componentsToSample,
		std::vector<COLOR_COMPONENT> componentsToConsider,
		bool normalizeBeforeSampling
	)
		: image_(image)
		, width_(width)
		, depth_(depth)
		, curve_(probabilityCurve)
		, componentsToSample_(componentsToSample)
		, componentsToConsider_(componentsToConsider)
		, normalizeBeforeSampling_(normalizeBeforeSampling)
	{}

	float TerrainMapColorProbabilityField::getProbabilityAtPoint(const glm::vec2& location)
	{
		float u = (location.x + width_) / (2.f * width_);
		float v = (location.y + depth_) / (2.f * depth_);

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
				return curve_->evaluateAt(sampleVal);
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
			return curve_->evaluateAt(normalizedVal);
		}
	}
}