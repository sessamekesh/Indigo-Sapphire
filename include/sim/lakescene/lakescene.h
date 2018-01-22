#pragma once

#include <sim/scene.h>

#include <view/terrainshader/assets/importedgeo.h>
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

#include <view/solidshader/genericsolidentity.h>
#include <model/proctree/proctree.h>

// TODO SESS: Also, loading times are high enough that THREADING is a thing to consider on CPU-side stuff
//  (especially loading things from Assimp, and processing the vertices)
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

			//
			// Shaders
			//
		private:
			std::shared_ptr<view::terrainshader::TerrainShader> terrainShader_;
			std::shared_ptr<view::special::SkyboxShader> skyboxShader_;
			std::shared_ptr<view::solidshader::SolidShader> solidShader_;
			std::shared_ptr<view::special::watersurface::WaterSurfaceShader> waterSurfaceShader_;
			std::shared_ptr<view::mappedphong::MappedPhongShader> mappedPhongShader_;

			//
			// Models
			//
		private:
			std::shared_ptr<model::geo::Rectangle> waterSurfaceModel_;
			std::shared_ptr<Proctree::Tree> testProctreeModel_;

			//
			// Resources
			//
		private:
			std::shared_ptr<view::terrainshader::assets::ImportedGeo> terrain_;
			std::shared_ptr<util::camera::FlightCamera> mainCamera_;
			std::shared_ptr<util::camera::PlanarReflectionCamera> waterReflectionCamera_;
			model::light::DirectionalLight sunlight_;
			std::shared_ptr<view::special::skybox::DaylightSkybox> skybox_;
			std::shared_ptr<view::special::watersurface::FlatSurface> lakeSurface_;
			std::shared_ptr<view::mappedphong::AssimpGeo> boulderTest_;
			std::shared_ptr<view::solidshader::GenericSolidEntity> testProctreeEntity_;

			glm::mat4 projMatrix_;

			//
			// Textures
			//
		private:
			std::map<std::string, std::shared_ptr<view::Texture>> textures_;

			//
			// Framebuffers
			//
			std::shared_ptr<view::Framebuffer> waterReflectionFramebuffer_;
			std::shared_ptr<view::Framebuffer> waterRefractionFramebuffer_;

			//
			// Properties
			//
		private:
		};
	}
}