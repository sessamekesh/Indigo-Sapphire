#pragma once

#include <sim/scene.h>

#include <util/camera/StaticCamera.h>
#include <util/command/parserfactory.h>
#include <model/light/directionallight.h>
#include <view/special/skybox/daylight.h>
#include <view/special/watersurface/rectangle.h>
#include <view/special/watersurface/flatsurface.h>
#include <view/framebuffer.h>
#include <util/camera/planarreflectioncamera.h>
#include <util/camera/flightcamera.h>
#include <view/mappedphong/mappedphongshader.h>
#include <view/mappedphong/assimpgeo.h>
#include <view/rawentities/heightmapterrain.h>
#include <view/terrainshader/blendedterrainshader.h>
#include <view/terrainshader/assets/genericblendedterrainentity.h>
#include <util/camera/heightmapcamera.h>
#include <model/specialgeo/projection/projectionbase.h>
#include <sim/lakescene/bladedgrass.h>

#include <view/solidshader/genericsolidentity.h>
#include <model/proctree/proctree.h>

#include <view/grass/billboardgrassshader.h>
#include <view/grass/billboardgrassentity.h>

// TODO SESS: Each entity should have a "generate" and a "prepare" method (though be careful about naming!)
//  "generate" MAY be called before "prepare"
//  "prepare" MUST be called before using.
//  "generate" may be performed on any thread, but "prepare" must be called on the main thread
//  If "generate" has not been called when "prepare" is called, "prepare" calls "generate" and awaits result synchronously
//  "prepare" blocks until the results of "generate" are available. Again, "generate" may be called from a separate thread.
//  To make life easier, it may be worth having a "setup" method that sets internal state before calling "generate", to avoid large lambdas
// TODO SESS: Grass entities should use a heirarchial bounding geometry rough test - this prevents iterating over a huge list of things
// TODO SESS: Increase maximum density on grass entities (or probability field - actual scene is very grass-sparse)
// TODO SESS: Finish frustum culling implementation (in perspective projection) and apply to grass entity
// http://assimp.sourceforge.net/lib_html/class_assimp_1_1_importer.html - individual importers are not thread-safe
//  Create a MODEL for each thing you'll be using, as well as an ENTITY
//  This is how you should probably organize things moving forward.
// After that, you can start working on the waterbender effect
// After that, you can start working on the characters

namespace sim
{
	namespace lake
	{
		class LakeScene : public Scene
		{
		public:
			LakeScene(util::command::ParserFactory& parserFactory);
			~LakeScene() = default;
			LakeScene(const LakeScene&) = delete;

			//
			// Overridden from Scene
			//
		protected:
			void update(float dt) override;
			void render() override;
			bool initializeShaders() override;
			bool teardownShaders() override;
			bool initializeResources() override;
			bool teardownResources() override;
			bool processCommand(const std::vector<std::string>& terms) override;
			bool initializeProperties() override;
			virtual bool teardownProperties() override;
			bool shouldExit() override;

			//
			// Rendering Organization
			//
			void renderEnvironment(std::shared_ptr<util::camera::CameraBase> camera, const std::optional<model::geo::Plane>& clipPlane = {});

			//
			// Initialization Helpers
			//
		private:
			bool setupTextures();
			bool teardownTextures();
			bool loadSingleTexture(std::string texName, std::string fName);
			bool loadSingleTexture(std::string texName, std::shared_ptr<model::ImageData> imageData);
			bool setupTerrain(std::shared_ptr<model::specialgeo::Heightfield> generatedHeightfield);
			bool teardownTerrain();

			//
			// Shaders
			//
		private:
			std::shared_ptr<view::terrainshader::BlendedTerrainShader> blendedTerrainShader_;
			std::shared_ptr<view::special::SkyboxShader> skyboxShader_;
			std::shared_ptr<view::solidshader::SolidShader> solidShader_;
			std::shared_ptr<view::special::watersurface::WaterSurfaceShader> waterSurfaceShader_;
			std::shared_ptr<view::mappedphong::MappedPhongShader> mappedPhongShader_;
			std::shared_ptr<view::grass::BillboardGrassShader> grassShader_;

			//
			// System
			//
		private:
			util::ThreadPool threadPool_;

			//
			// Subsystems
			//
		private:
			std::shared_ptr<sim::lake::BladedGrass> bladedGrass_;

			//
			// Models
			//
		private:
			std::shared_ptr<model::geo::Rectangle> waterSurfaceModel_;
			std::shared_ptr<Proctree::Tree> testProctreeModel_;
			std::shared_ptr<model::specialgeo::ProjectionBase> projection_;

			//
			// Resources
			//
		private:
			std::shared_ptr<util::camera::FlightCamera> mainCamera_;
			std::shared_ptr<util::camera::HeightmapCamera> heightmapCamera_;
			std::shared_ptr<util::camera::PlanarReflectionCamera> waterReflectionCamera_;
			model::light::DirectionalLight sunlight_;
			std::shared_ptr<view::special::skybox::DaylightSkybox> skybox_;
			std::shared_ptr<view::special::watersurface::FlatSurface> lakeSurface_;
			std::shared_ptr<view::mappedphong::AssimpGeo> boulderTest_;
			std::shared_ptr<view::solidshader::GenericSolidEntity> testProctreeEntity_;
			std::shared_ptr<view::raw::HeightmapTerrainEntity> heightMapTerrainRawEntity_;
			std::shared_ptr<view::terrainshader::GenericBlendedTerrainEntity> blendedTerrainEntity_;
			
			std::vector<std::shared_ptr<view::grass::BillboardGrassEntity>> grassEntities_;

			//
			// Textures
			//
		private:
			std::map<std::string, std::shared_ptr<view::Texture>> textures_;
			std::shared_ptr<model::GreyscaleImageData> terrainHeightmap_;
			std::shared_ptr<model::ImageData> terrainBlendMapImage_;

			//
			// Framebuffers
			//
			std::shared_ptr<view::Framebuffer> waterReflectionFramebuffer_;
			std::shared_ptr<view::Framebuffer> waterRefractionFramebuffer_;

			//
			// Properties
			//

			//
			// I/O
			//
			std::shared_ptr<input::CameraControllerBase> cameraController_;
		private:
		};
	}
}