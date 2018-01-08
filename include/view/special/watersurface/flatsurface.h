#pragma once

#include <view/special/watersurface/rectangle.h>
#include <view/framebuffer.h>
#include <view/texture.h>

namespace view
{
	namespace special
	{
		namespace watersurface
		{
			class FlatSurface
			{
			public:
				FlatSurface(
					std::shared_ptr<model::geo::Rectangle> rectModel,
					std::shared_ptr<view::Texture> normalMap,
					std::shared_ptr<view::Texture> dudvMap,
					float tilingMultiplier,
					float dudvScale,
					float dudvOffset,
					float dudvOffsetVelocity,
					float shineDamper,
					float reflectivity
				);
				FlatSurface(const FlatSurface&) = delete;
				~FlatSurface();

				bool prepare(std::shared_ptr<WaterSurfaceShader> shader, util::PipelineState& pso);
				void update(float dt);
				void render(
					std::shared_ptr<WaterSurfaceShader> shader,
					std::shared_ptr<view::Framebuffer> reflectionFBO,
					std::shared_ptr<view::Framebuffer> refractionFBO
				);

				float tilingMultiplier() const;
				void tilingMultiplier(float nv);

				float dudvScale() const;
				void dudvScale(float nv);

				float dudvOffsetVelocity() const;
				void dudvOffsetVelocity(float nv);

				float shineDamper() const;
				void shineDamper(float nv);

				float reflectivity() const;
				void reflectivity(float nv);

				std::shared_ptr<model::geo::Rectangle> model() const;

			protected:
				std::shared_ptr<model::geo::Rectangle> rectModel_;
				std::shared_ptr<view::special::watersurface::Rectangle> rectView_;
				std::shared_ptr<view::Texture> normalMap_;
				std::shared_ptr<view::Texture> dudvMap_;
				float tilingMultiplier_;
				float dudvScale_;
				float dudvOffset_;
				float dudvOffsetVelocity_;
				float shineDamper_;
				float reflectivity_;
			};
		}
	}
}