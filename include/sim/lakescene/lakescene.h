#pragma once

#include <sim/scene.h>

#include <model/specialgeo/metaball/metaballgroup.h>
#include <view/special/metaball/metaballgroup.h>
#include <view/terrainshader/assets/importedgeo.h>
#include <util/camera/StaticCamera.h>
#include <util/command/parserfactory.h>
#include <model/light/directionallight.h>
#include <view/special/skybox/daylight.h>

// TODO SESS: Put in a skybox around the entire scene now
// After that, you can start working on the water surface
// After that, you can start working on the rocks
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
			// Helpers
			//
		private:
			bool setupTextures();
			bool teardownTextures();

			//
			// Shaders
			//
		private:
			std::shared_ptr<view::special::metaball::PhongShader> metaballPhongShader_;
			std::shared_ptr<view::terrainshader::TerrainShader> terrainShader_;
			std::shared_ptr<view::special::SkyboxShader> skyboxShader_;
			std::shared_ptr<view::solidshader::SolidShader> solidShader_;

			//
			// Models
			//
		private:
			std::shared_ptr<model::specialgeo::metaball::MetaballGroup> metaballGroupModel_;

			//
			// Resources
			//
		private:
			std::shared_ptr<view::special::metaball::MetaballGroup> metaballGroup_;
			std::shared_ptr<view::terrainshader::assets::ImportedGeo> terrain_;
			std::shared_ptr<util::camera::StaticCamera> camera_;
			model::light::DirectionalLight sunlight_;
			std::shared_ptr<view::special::skybox::DaylightSkybox> skybox_;
			
			glm::mat4 projMatrix_;

			//
			// Textures
			//
		private:
			std::map<std::string, std::shared_ptr<view::Texture>> textures_;
		};
	}
}