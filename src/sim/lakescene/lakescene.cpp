#include <sim/lakescene/lakescene.h>
#include <model/specialgeo/metaball/debugmetaballgroup.h>
#include <resources.h>

namespace sim
{
	namespace lake
	{
		LakeScene::LakeScene(
			util::command::ParserFactory& parserFactory
		)
			: Scene("Lake Scene", 800u, 600u)
			, metaballPhongShader_(nullptr)
			, terrainShader_(nullptr)
			, skyboxShader_(nullptr)
			, solidShader_(nullptr)
			, metaballGroupModel_(nullptr)
			, metaballGroup_(nullptr)
			, terrain_(nullptr)
			, camera_(nullptr)
			, sunlight_(
				glm::vec3(0.1f, 0.1f, 0.1f), // Ambient
				glm::vec3(0.9f, 0.9f, 0.9f), // Diffuse
				glm::vec3(1.f, 1.f, 1.f), // Specular
				glm::vec3(1.f, -1.f, 0.f)) // Direction
			, skybox_(nullptr)
			, projMatrix_(glm::perspective(glm::radians(90.f), 8.f / 6.f, 0.1f, 1000.0f))
			, textures_({})
		{
			registerParser(parserFactory.floatParser());
			registerParser(parserFactory.vec3Parser());
			registerParser(parserFactory.staticCameraParser());
			registerParser(parserFactory.quaternionParser());
			registerParser(parserFactory.withWorldTransformParser());
			registerParser(parserFactory.directionalLightParser());
		}

		void LakeScene::update(float dt)
		{
			metaballGroupModel_->update(dt);

			auto ng = glm::angle(skybox_->rot());
			ng += 0.02f * dt;
			while (ng > glm::two_pi<float>())
			{
				ng -= glm::two_pi<float>();
			}
			while (ng < 0.f)
			{
				ng += 0.01f;
			}
			auto ax = glm::vec3(0.f, 1.f, 0.f);
			skybox_->rot(glm::angleAxis(ng, ax));
		}

		void LakeScene::render()
		{
			glClearColor(0x36 / 255.f, 0x45 / 255.f, 0x4f / 255.f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			int width, height;
			glfwGetWindowSize(window_, &width, &height);
			glViewport(0, 0, width, height);
			
			// TODO SESS: Why is the skybox not rendering?
			// Skybox before all else
			glDepthMask(GL_FALSE);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glDisable(GL_BLEND);
			if (skyboxShader_->activate())
			{
				skyboxShader_->setProjMatrix(projMatrix_);
				skyboxShader_->setViewMatrix(camera_->getViewTransform());
				
				skybox_->render(skyboxShader_, pso_);
			}
			else
			{
				log.warn << "Failed to activate skybox shader, not drawing skybox" << util::endl;
			}

			//
			// Opaque shading first
			//
			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			if (terrainShader_->activate())
			{
				terrainShader_->setViewMatrix(camera_->getViewTransform());
				terrainShader_->setProjMatrix(projMatrix_);
				terrainShader_->setLight(sunlight_);

				terrain_->render(terrainShader_);
			}
			else
			{
				log.warn << "Failed to activate terrain shader, not drawing terrain" << util::endl;
			}

			//
			// After all opaque calls are finished, do transparent calls
			//
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			if (metaballPhongShader_->activate())
			{
				metaballPhongShader_->setViewMatrix(camera_->getViewTransform());
				metaballPhongShader_->setCameraPos(camera_->pos());
				metaballPhongShader_->setProjMatrix(projMatrix_);
				metaballPhongShader_->setStepSize(0.08f);
				metaballPhongShader_->setLight(
					glm::vec3(0.1f, 0.1f, 0.1f),
					glm::vec3(0.5f, 0.5f, 0.5f),
					glm::vec3(1.f, 1.f, 1.f)
				);
				metaballPhongShader_->setLightDir(
					glm::normalize(glm::vec3(1.f, -2.f, 3.f))
				);

				if (metaballGroup_->preparePhong(
					camera_->pos(),
					camera_->lookDir(),
					camera_->up()
				)) {
					metaballGroup_->renderPhong(metaballPhongShader_);
				}
			}
			else
			{
				log.warn << "Failed to activate metaball shader, not drawing metaballs" << util::endl;
			}
		}

		bool LakeScene::initializeShaders()
		{
			metaballPhongShader_ = std::make_shared<view::special::metaball::PhongShader>();
			if (!metaballPhongShader_ || !metaballPhongShader_->initialize())
			{
				log.error << "Failed to create metaball phong shader" << util::endl;
				return false;
			}

			terrainShader_ = std::make_shared<view::terrainshader::TerrainShader>();
			if (!terrainShader_ || !terrainShader_->initialize())
			{
				log.error << "Failed to create terrain shader" << util::endl;
				return false;
			}

			skyboxShader_ = std::make_shared<view::special::SkyboxShader>();
			if (!skyboxShader_ || !skyboxShader_->initialize())
			{
				log.error << "Failed to create skybox shader" << util::endl;
				return false;
			}

			solidShader_ = std::make_shared<view::solidshader::SolidShader>();
			if (!solidShader_ || !solidShader_->initialize())
			{
				return false;
			}

			return true;
		}

		bool LakeScene::teardownShaders()
		{
			metaballPhongShader_ = nullptr;
			terrainShader_ = nullptr;
			skyboxShader_ = nullptr;

			return true;
		}

		bool LakeScene::initializeResources()
		{
			camera_ = std::shared_ptr<util::camera::StaticCamera>(new util::camera::StaticCamera(
				glm::vec3(0.f, 0.f, -25.f),
				glm::vec3(0.f, 0.f, 5.f),
				glm::vec3(0.f, 1.f, 0.f)));
			if (!setupTextures())
			{
				return false;
			}

			metaballGroupModel_ = std::shared_ptr<model::specialgeo::metaball::MetaballGroup>(
				new model::specialgeo::metaball::DebugMetaballGroup(
					glm::vec3(0.f, 0.f, 10.f),
					glm::angleAxis(0.f, glm::vec3(1.f, 0.f, 0.f)),
					glm::vec3(1.f, 1.f, 1.f),
					glm::vec3(-5.f, -2.f, -1.f),
					glm::vec3(5.f, 2.f, 1.f),
					8u,
					3.f
				)
			);
			metaballGroup_ = std::shared_ptr<view::special::metaball::MetaballGroup>(
				new view::special::metaball::MetaballGroup(
					metaballGroupModel_,
					glm::vec3(0.f, 0.f, 1.f),
					glm::vec3(1.f, 1.f, 1.f),
					125.f
				)
			);
			if (!metaballGroup_ || !metaballGroup_->preparePhong(camera_->pos(), camera_->lookDir(), camera_->up()))
			{
				log.error << "Failed to initialize metaball group object" << util::endl;
				return false;
			}

			terrain_ = std::shared_ptr<view::terrainshader::assets::ImportedGeo>(
				new view::terrainshader::assets::ImportedGeo(
					glm::vec3(0.f, -5.f, 15.f),
					glm::angleAxis(glm::quarter_pi<float>(), glm::vec3(0.f, 1.f, 0.f)) *
						glm::angleAxis(glm::half_pi<float>(), glm::vec3(1.f, 0.f, 0.f)) *
						glm::angleAxis(3.14f, glm::vec3(0.f, 1.f, 0.f)),
					glm::vec3(41.f, 41.f, 41.f),
					"TerrainBase"
				)
			);

			{
				std::map<std::string, std::shared_ptr<view::Texture>> terrainTextures;
				terrainTextures["terrain_base"] = textures_["dry-grass"];
				if (!terrain_
					|| !terrain_->prepare(
						ASSET_PATH("environment/terrain_base/terrain_base.fbx"),
						{ "terrain_base" },
						terrainTextures,
						terrainShader_,
						pso_
					))
				{
					log.error << "Failed to initialize terrain geometry" << util::endl;
					return false;
				}
			}

			skybox_ = std::shared_ptr<view::special::skybox::DaylightSkybox>(
				new view::special::skybox::DaylightSkybox(
					glm::angleAxis(0.01f, glm::vec3(0.f, 1.f, 0.f)),
					glm::vec3(40.f, 40.f, 40.f)
				)
			);
			if (!skybox_ || !skybox_->prepare(skyboxShader_, pso_))
			{
				log.error << "Failed to initialize skybox" << util::endl;
				return false;
			}

			return true;
		}

		bool LakeScene::teardownResources()
		{
			if (metaballGroup_)
			{
				metaballGroup_->release();
				metaballGroup_ = nullptr;
			}

			if (terrain_)
			{
				terrain_->release();
				terrain_ = nullptr;
			}

			if (skybox_)
			{
				skybox_->release();
				skybox_ = nullptr;
			}

			if (!teardownTextures())
			{
				return false;
			}

			return true;
		}

		bool LakeScene::processCommand(const std::vector<std::string>& terms)
		{
			return false;
		}

		bool LakeScene::initializeProperties()
		{
			// This is a wee bit awkward, eh?
			registerProperty("camera", util::command::StaticCameraParser::uuid,
				std::reinterpret_pointer_cast<void>(camera_)
			);
			registerProperty("terrain_base", util::command::WithWorldTransformParser::uuid,
				std::reinterpret_pointer_cast<void>(terrain_)
			);
			registerProperty("sunlight", util::command::DirectionalLightParser::uuid,
				std::shared_ptr<void>(&sunlight_, [](void*) {})
			);
			registerProperty("skybox", util::command::WithWorldTransformParser::uuid,
				std::reinterpret_pointer_cast<void>(skybox_));

			return true;
		}

		bool LakeScene::teardownProperties()
		{
			unregisterProperty("camera");
			unregisterProperty("terrain_base");
			unregisterProperty("sunlight");
			return true;
		}

		bool LakeScene::shouldExit()
		{
			if (Scene::shouldExit())
			{
				return true;
			}

			return false;
		}

		bool LakeScene::setupTextures()
		{
			auto dryGrassOpt = model::readPNG(ASSET_PATH("texture/dry-grass.png"));
			if (!dryGrassOpt)
			{
				log.error << "Failed to load dry grass image" << util::endl;
				return false;
			}

			auto dryGrassTexture = std::shared_ptr<view::Texture>(
				new view::Texture()
			);
			if (!dryGrassTexture->init(view::Texture::RGBA, *dryGrassOpt))
			{
				log.error << "Failed to create dry grass texture" << util::endl;
				return false;
			}
			textures_["dry-grass"] = dryGrassTexture;

			return true;
		}

		bool LakeScene::teardownTextures()
		{
			if (textures_.find("dry-grass") != textures_.end())
			{
				textures_["dry-grass"]->release();
				textures_.erase("dry-grass");
			}

			return true;
		}
	}
}