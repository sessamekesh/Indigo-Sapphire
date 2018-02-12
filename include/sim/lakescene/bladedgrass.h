#pragma once

#include <view/grass/bladedgrasspatchshader.h>
#include <view/grass/bladedgrasspatchentity.h>
#include <model/specialgeo/heightfield.h>
#include <util/camera/camerabase.h>
#include <model/specialgeo/projection/projectionbase.h>
#include <model/geo/plane.h>
#include <optional>
#include <model/light/directionallight.h>
#include <util/threadpool.h>
#include <util/math/curve1to1.h>

// TODO SESS: Finish writing this out. This will be the entire covering of grass -
//  the entire grass covering subsystem.
// Pulling out into its own class because there is a lot of logic to ensure that
//  the right number of grass blades get drawn - trying not to overload the GPU
//  with drawing grass
// A few things that haven't at all been expressed in code, but should be done:
// - You know how grass is created by picking random points on a patch? The proportion
//   of blades actually drawn should fade out quadratically from the camera.
//    + Draw ALL if patch is within 5 meters of camera
//    + Draw NONE if patch is further than 150 meters from camera
//    + QUADRATIC FADE in between 5 and 150 meters
// - Implement projections that generate a projMatrix and also a list of boundingPlanes
// - Before rendering a patch of grass, test it against all the bounding planes. Skip if outside.
// - It might be interesting to track how many blades of grass are being drawn, versus how many are
//   visible, versus how many exist total in the scene. For shits and giggles.
// - It also might be worth making the vertex format SMALLER on the blades of grass, since we will probably
//   be using MILLIONS of them - 5MB/vtxB, current vertex format is 8 floats = 256B => 1.2GBish.
// - Or perhaps generating only a handful of patches, and instead sampling heightmap data for where to actually
//   draw the grass - though you have to be careful to generate a heightmap to match the geometry actually used.

/*
if (bladedGrassShader_->activate())
{
if (clipPlane)
{
bladedGrassShader_->setClipPlane(*clipPlane);
}

bladedGrassShader_->setViewMatrix(camera->getViewTransform());
bladedGrassShader_->setCameraPosition(camera->pos());
bladedGrassShader_->setProjMatrix(projMatrix_);
bladedGrassShader_->setLight(sunlight_);

bladedGrassEntity_->render(bladedGrassShader_);
}

...
...


bladedGrassEntity_ = std::make_shared<view::grass::BladedGrassPatchEntity>(
glm::vec3(0.f, -12.5f, 0.f),
glm::angleAxis(0.f, glm::vec3(0.f, 1.f, 0.f)),
glm::vec3(1.f, 1.f, 1.f)
);
if (!bladedGrassEntity_ || !bladedGrassEntity_->prepare(
bladedGrassShader_, pso_,
reducedHeightfield,
textures_["grassBlade"],
heightfieldMask,
glm::vec2(0.f, 0.f), glm::vec2(10.f, 10.f),
100000u,
0.005f, 0.018f,
0.05f, 0.12f,
0.f, 3.14f,
0.01f, 0.15f,
0.3f, 0.45f,
glm::vec4(0.2f, 0.2f, 0.2f, 1.f),
0.03f, 1u
)) {
log.error << "Failed to generate bladed grass entity" << util::endl;
return false;
}
*/

namespace sim
{
	namespace lake
	{
		struct BladedGrassConfig
		{
			float maxPatchRadius;
			float maxBladeDensityPerSquareMeter;
			float minBladeBaseWidth;
			float maxBladeBaseWidth;
			float minBladeHeight;
			float maxBladeHeight;
			float minBladeRotation;
			float maxBladeRotation;
			float minBladeTwistRate;
			float maxBladeTwistRate;
			float minBladeTaperRate;
			float maxBladeTaperRate;
			glm::vec4 specularColor;
			float windStrength;
			std::uint32_t randomSeed;
		};

		class BladedGrass
		{
		public:
			static BladedGrassConfig STANDARD_COMPILED_CONFIG;

		public:
			BladedGrass(
				std::shared_ptr<model::specialgeo::Heightfield> heightfield,
				std::shared_ptr<util::SurfaceProbabilityFieldBase> probabilityField,
				std::shared_ptr<util::SurfaceMaskBase> surfaceMask,
				glm::vec3 terrainBaseTransform,
				const BladedGrassConfig& config
			);
			~BladedGrass();
			BladedGrass(const BladedGrass&) = delete;
			
			bool prepare(
				const glm::vec2& startCorner,
				const glm::vec2& endCorner,
				util::PipelineState& pso,
				util::ThreadPool& threadPool
			);
			bool release();

			void getFrameRenderingData(
				std::uint32_t& o_numBladesInScene,
				std::uint32_t& o_numBladesInView,
				std::uint32_t& o_numBladesRendered
			);

			void update(float dt);
			void render(
				std::shared_ptr<util::camera::CameraBase> camera,
				std::shared_ptr<model::specialgeo::ProjectionBase> projection,
				const model::light::DirectionalLight& sunlight,
				std::optional<model::geo::Plane> clipPlane = {}
			);

		private:
			bool isReady_;

			//
			// Resources
			//
			std::shared_ptr<model::specialgeo::Heightfield> heightfield_;
			std::shared_ptr<util::SurfaceProbabilityFieldBase> probabilityField_;
			std::shared_ptr<util::SurfaceMaskBase> surfaceMask_;
			std::shared_ptr<view::Texture> grassTexture_;
			std::shared_ptr<util::math::Curve1To1> numBladesLodCurve_;
			glm::vec3 terrainBaseTransform_;
			BladedGrassConfig config_;

			//
			// Shaders
			//
			std::shared_ptr<view::grass::BladedGrassPatchShader> bladedGrassShader_;

			//
			// Entities
			//
			std::vector<std::shared_ptr<view::grass::BladedGrassPatchEntity>> grassEntities_;

			//
			// Utility Objects
			//
			util::Logger log;

			//
			// Data for Querying
			//
			std::uint32_t numBladesInScene_;
			std::uint32_t numBladesInView_;
			std::uint32_t numBladesRendered_;
		};
	}
}