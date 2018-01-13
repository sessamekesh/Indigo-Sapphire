#pragma once

#include <sim/scene.h>

#include <model/specialgeo/metaball/metaballgroup.h>
#include <view/special/metaball/metaballgroup.h>
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

// TODO SESS: start working on the rocks
// After that, you can start working on the trees
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
			void renderEnvironment(std::shared_ptr<util::camera::CameraBase> camera, std::optional<glm::vec3> clipNormal = std::nullopt, std::optional<glm::vec3> clipOrigin = std::nullopt);
			void renderMetaballs(std::shared_ptr<util::camera::CameraBase> camera);

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
			std::shared_ptr<view::special::metaball::PhongShader> metaballPhongShader_;
			std::shared_ptr<view::terrainshader::TerrainShader> terrainShader_;
			std::shared_ptr<view::special::SkyboxShader> skyboxShader_;
			std::shared_ptr<view::solidshader::SolidShader> solidShader_;
			std::shared_ptr<view::special::watersurface::WaterSurfaceShader> waterSurfaceShader_;

			//
			// Models
			//
		private:
			std::shared_ptr<model::specialgeo::metaball::MetaballGroup> metaballGroupModel_;
			std::shared_ptr<model::geo::Rectangle> waterSurfaceModel_;

			//
			// Resources
			//
		private:
			std::shared_ptr<view::special::metaball::MetaballGroup> metaballGroup_;
			std::shared_ptr<view::terrainshader::assets::ImportedGeo> terrain_;
			std::shared_ptr<util::camera::FlightCamera> mainCamera_;
			std::shared_ptr<util::camera::PlanarReflectionCamera> waterReflectionCamera_;
			model::light::DirectionalLight sunlight_;
			std::shared_ptr<view::special::skybox::DaylightSkybox> skybox_;
			std::shared_ptr<view::special::watersurface::FlatSurface> lakeSurface_;
			
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