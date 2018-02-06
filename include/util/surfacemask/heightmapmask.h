#pragma once

#include <util/surfacemaskbase.h>
#include <model/specialgeo/heightfield.h>
#include <memory>

namespace util
{
	class HeightmapMask : public SurfaceMaskBase
	{
	public:
		HeightmapMask(
			std::shared_ptr<model::specialgeo::Heightfield> heightfield,
			float minHeight,
			float maxHeight
		);
		~HeightmapMask() = default;
		HeightmapMask(const HeightmapMask&) = default;

		bool getMaskValue(const glm::vec2& pos) override;

	protected:
		std::shared_ptr<model::specialgeo::Heightfield> heightfield_;
		float minHeight_;
		float maxHeight_;
	};
}