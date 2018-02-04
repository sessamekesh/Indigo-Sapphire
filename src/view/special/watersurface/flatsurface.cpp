#include <view/special/watersurface/flatsurface.h>

namespace view
{
	namespace special
	{
		namespace watersurface
		{
			FlatSurface::FlatSurface(
				std::shared_ptr<model::geo::Rectangle> rectModel,
				std::shared_ptr<view::Texture> normalMap,
				std::shared_ptr<view::Texture> dudvMap,
				float tilingMultiplier,
				float dudvScale,
				float dudvOffset,
				float dudvOffsetVelocity,
				float shineDamper,
				float reflectivity
			)
				: rectModel_(rectModel)
				, rectView_(nullptr)
				, normalMap_(normalMap)
				, dudvMap_(dudvMap)
				, tilingMultiplier_(tilingMultiplier)
				, dudvScale_(dudvScale)
				, dudvOffset_(dudvOffset)
				, dudvOffsetVelocity_(dudvOffsetVelocity)
				, shineDamper_(shineDamper)
				, reflectivity_(reflectivity)
			{}

			FlatSurface::~FlatSurface()
			{
				if (rectView_)
				{
					rectView_->release();
					rectView_ = nullptr;
				}
			}

			bool FlatSurface::prepare(std::shared_ptr<WaterSurfaceShader> shader, util::PipelineState& pso)
			{
				if (rectView_)
				{
					return false;
				}

				rectView_ = std::make_shared<view::special::watersurface::Rectangle>(rectModel_);
				return rectView_->prepare(shader, pso);
			}

			void FlatSurface::update(float dt)
			{
				dudvOffset_+= dudvOffsetVelocity_ * dt;
			}

			void FlatSurface::render(
				std::shared_ptr<WaterSurfaceShader> shader,
				std::shared_ptr<view::Framebuffer> reflectionFBO,
				std::shared_ptr<view::Framebuffer> refractionFBO
			) {
				shader->setReflectionTexture(reflectionFBO);
				shader->setRefractionTexture(refractionFBO);
				shader->setTilingStrength(tilingMultiplier_);
				shader->setNormalMap(normalMap_);
				shader->setDUDVMap(dudvMap_);
				shader->setDUDVScaleFactor(dudvScale_);
				shader->setDUDVSampleOffset(dudvOffset_);
				shader->setReflectivity(reflectivity_);
				shader->setShineDamper(shineDamper_);
				shader->setWaterSurfaceOrientation(rectModel_->rot());

				rectView_->render(shader);
			}

			float FlatSurface::tilingMultiplier() const
			{
				return tilingMultiplier_;
			}

			void FlatSurface::tilingMultiplier(float nv)
			{
				if (nv > 0.f)
				{
					tilingMultiplier_ = nv;
				}
			}

			float FlatSurface::dudvScale() const
			{
				return dudvScale_;
			}

			void FlatSurface::dudvScale(float nv)
			{
				if (nv > 0.f)
				{
					dudvScale_ = nv;
				}
			}

			float FlatSurface::dudvOffsetVelocity() const
			{
				return dudvOffsetVelocity_;
			}

			void FlatSurface::dudvOffsetVelocity(float nv)
			{
				dudvOffsetVelocity_ = nv;
			}

			float FlatSurface::shineDamper() const
			{
				return shineDamper_;
			}

			void FlatSurface::shineDamper(float nv)
			{
				if (nv > 0.f) shineDamper_ = nv;
			}

			float FlatSurface::reflectivity() const
			{
				return reflectivity_;
			}

			void FlatSurface::reflectivity(float nv)
			{
				if (nv > 0.f) reflectivity_ = nv;
			}
			std::shared_ptr<model::geo::Rectangle> FlatSurface::model() const
			{
				return rectModel_;
			}
		}
	}
}