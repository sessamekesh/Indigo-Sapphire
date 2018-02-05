#pragma once

#include <model/specialgeo/heightfield.h>
#include <memory>

namespace model
{
	namespace specialgeo
	{
		class ReducedHeightfield : public Heightfield
		{
		public:
			ReducedHeightfield(
				std::shared_ptr<Heightfield> base,
				std::uint32_t xDivisions,
				std::uint32_t zDivisions
			);
			~ReducedHeightfield() = default;
			ReducedHeightfield(const ReducedHeightfield&) = default;

			float heightAtPos(float x, float z, float defaultHeight, bool blend = true) const override;
			float width() const override;
			float depth() const override;

		private:
			std::shared_ptr<Heightfield> base_;
			std::uint32_t xDivisions_;
			std::uint32_t zDivisions_;
		};
	}
}