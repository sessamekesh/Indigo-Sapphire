#include <sim/lakescene/lakescene.h>
#include <model/specialgeo/metaball/debugmetaballgroup.h>
#include <resources.h>
#include <glm/gtc/matrix_transform.hpp>

namespace sim
{
	namespace lake
	{
		LakeScene::LakeScene(
			util::command::ParserFactory& parserFactory
		)
			: Scene("Lake Scene", 1280u, 720u)
			, metaballPhongShader_(nullptr)
			, terrainShader_(nullptr)
			, skyboxShader_(nullptr)
			, solidShader_(nullptr)
			, waterSurfaceShader_(nullptr)
			, metaballGroupModel_(nullptr)
			, waterSurfaceModel_(nullptr)
			, metaballGroup_(nullptr)
			, terrain_(nullptr)
			, mainCamera_(nullptr)
			, waterReflectionCamera_(nullptr)
			, sunlight_(
				glm::vec3(0.1f, 0.1f, 0.1f), // Ambient
				glm::vec3(0.9f, 0.9f, 0.9f), // Diffuse
				glm::vec3(1.f, 1.f, 1.f), // Specular
				glm::vec3(1.f, -1.f, 0.f)) // Direction
			, skybox_(nullptr)
			, waterSurface_(nullptr)
			, projMatrix_(glm::perspective(glm::radians(90.f), 128.f / 72.f, 0.1f, 1000.0f))
			, textures_({})
			, waterReflectionFramebuffer_(nullptr)
			, waterRefractionFramebuffer_(nullptr)
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

		void LakeScene::renderEnvironment(std::shared_ptr<util::camera::CameraBase> camera, std::optional<glm::vec3> clipNormal, std::optional<glm::vec3> clipOrigin)
		{
			// Skybox before all else
			glDepthMask(GL_FALSE);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glDisable(GL_BLEND);
			if (skyboxShader_->activate())
			{
				if (clipOrigin && clipNormal)
				{
					skyboxShader_->setClipPlane(*clipOrigin, *clipNormal);
				}

				skyboxShader_->setProjMatrix(projMatrix_);
				skyboxShader_->setViewMatrix(camera->getViewTransform());

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
				if (clipOrigin && clipNormal)
				{
					terrainShader_->setClipPlane(*clipOrigin, *clipNormal);
				}

				terrainShader_->setViewMatrix(camera->getViewTransform());
				terrainShader_->setProjMatrix(projMatrix_);
				terrainShader_->setLight(sunlight_);

				terrain_->render(terrainShader_);
			}
			else
			{
				log.warn << "Failed to activate terrain shader, not drawing terrain" << util::endl;
			}
		}

		void LakeScene::render()
		{
			glClearColor(0x36 / 255.f, 0x45 / 255.f, 0x4f / 255.f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			int width, height;
			glfwGetWindowSize(window_, &width, &height);
			glViewport(0, 0, width, height);
			
			//
			// Generate water reflection/refraction textures
			//
			auto waterOrigin = waterSurfaceModel_->pos();
			auto waterNormal = glm::rotate(waterSurfaceModel_->rot(), glm::vec3(0.f, 1.f, 0.f));
			//glEnable(GL_CLIP_DISTANCE0);
			waterReflectionCamera_->reflectionPlane(model::geo::Plane(waterOrigin, waterNormal));
			waterReflectionFramebuffer_->bind();
			renderEnvironment(waterReflectionCamera_, waterNormal, waterOrigin);
			waterRefractionFramebuffer_->bind();
			renderEnvironment(mainCamera_, -waterNormal, waterOrigin);
			view::Framebuffer::bindDefaultFramebuffer();
			glViewport(0, 0, width, height);

			//
			// Render to main color/depth buffer
			//
			glDisable(GL_CLIP_DISTANCE0);
			renderEnvironment(mainCamera_);

			//
			// Water surface
			//
			glDisable(GL_CULL_FACE);
			if (waterSurfaceShader_->activate())
			{
				waterSurfaceShader_->setReflectionTexture(waterReflectionFramebuffer_);
				waterSurfaceShader_->setRefractionTexture(waterRefractionFramebuffer_);
				waterSurfaceShader_->setViewMatrix(mainCamera_->getViewTransform());
				waterSurfaceShader_->setProjMatrix(projMatrix_);

				waterSurface_->render(waterSurfaceShader_);
			}
			else
			{
				log.warn << "Failed to activate water surface shader, not drawing water" << util::endl;
			}

			//
			// After all opaque calls are finished, do transparent calls
			//
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			if (metaballPhongShader_->activate())
			{
				metaballPhongShader_->setViewMatrix(mainCamera_->getViewTransform());
				metaballPhongShader_->setCameraPos(mainCamera_->pos());
				metaballPhongShader_->setProjMatrix(projMatrix_);
				metaballPhongShader_->setStepSize(0.15f);
				metaballPhongShader_->setLight(
					glm::vec3(0.1f, 0.1f, 0.1f),
					glm::vec3(0.5f, 0.5f, 0.5f),
					glm::vec3(1.f, 1.f, 1.f)
				);
				metaballPhongShader_->setLightDir(
					glm::normalize(glm::vec3(1.f, -2.f, 3.f))
				);

				if (metaballGroup_->preparePhong(
					mainCamera_->pos(),
					mainCamera_->lookDir(),
					mainCamera_->up()
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

			waterSurfaceShader_ = std::make_shared<view::special::watersurface::WaterSurfaceShader>();
			if (!waterSurfaceShader_ || !waterSurfaceShader_->initialize())
			{
				log.error << "Failed to create water surface shader" << util::endl;
				return false;
			}

			return true;
		}

		bool LakeScene::teardownShaders()
		{
			metaballPhongShader_ = nullptr;
			terrainShader_ = nullptr;
			skyboxShader_ = nullptr;
			solidShader_ = nullptr;
			waterSurfaceShader_ = nullptr;

			return true;
		}

		bool LakeScene::initializeResources()
		{
			mainCamera_ = std::shared_ptr<util::camera::StaticCamera>(new util::camera::StaticCamera(
				glm::vec3(0.f, 0.f, -25.f),
				glm::vec3(0.f, 0.f, 5.f),
				glm::vec3(0.f, 1.f, 0.f)));
			waterReflectionCamera_ = std::shared_ptr<util::camera::PlanarReflectionCamera>(new util::camera::PlanarReflectionCamera(
				mainCamera_,
				model::geo::Plane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f })
			));
			if (!setupTextures())
			{
				return false;
			}

			metaballGroupModel_ = std::shared_ptr<model::specialgeo::metaball::MetaballGroup>(
				new model::specialgeo::metaball::DebugMetaballGroup(
					glm::vec3(0.f, 0.f, 10.f),
					glm::angleAxis(0.f, glm::vec3(1.f, 0.f, 0.f)),
					glm::vec3(1.f, 1.f, 1.f),
					glm::vec3(-25.f, -5.f, -1.f),
					glm::vec3(25.f, 5.f, 3.f),
					3u,
					5.f
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
			if (!metaballGroup_ || !metaballGroup_->preparePhong(mainCamera_->pos(), mainCamera_->lookDir(), mainCamera_->up()))
			{
				log.error << "Failed to initialize metaball group object" << util::endl;
				return false;
			}

			terrain_ = std::shared_ptr<view::terrainshader::assets::ImportedGeo>(
				new view::terrainshader::assets::ImportedGeo(
					glm::vec3(0.f, 5.f, 15.f),
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

			waterSurfaceModel_ = std::shared_ptr<model::geo::Rectangle>(
				new model::geo::Rectangle(
					glm::vec3(0.f, -15.f, 0.f),
					glm::angleAxis(0.f, glm::vec3(0.f, 0.f, 0.f)),
					500.f, 500.f
				)
			);
			waterSurface_ = std::shared_ptr<view::special::watersurface::Rectangle>(
				new view::special::watersurface::Rectangle(waterSurfaceModel_)
			);
			if (!waterSurface_ || !waterSurface_->prepare(waterSurfaceShader_, pso_))
			{
				log.error << "Failed to initialize water surface" << util::endl;
				return false;
			}

			waterReflectionFramebuffer_ = std::make_shared<view::Framebuffer>();
			if (!waterReflectionFramebuffer_ || !waterReflectionFramebuffer_->init(512, 512))
			{
				log.error << "Failed to initialize water reflection framebuffer" << util::endl;
				return false;
			}

			waterRefractionFramebuffer_ = std::make_shared<view::Framebuffer>();
			if (!waterRefractionFramebuffer_ || !waterRefractionFramebuffer_->init(512, 512))
			{
				log.error << "Failed to initialize water refraction framebuffer" << util::endl;
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

			if (waterSurface_)
			{
				waterSurface_->release();
				waterSurface_ = nullptr;
			}

			if (waterReflectionFramebuffer_)
			{
				waterReflectionFramebuffer_->release();
				waterReflectionFramebuffer_ = nullptr;
			}

			if (waterRefractionFramebuffer_)
			{
				waterRefractionFramebuffer_->release();
				waterRefractionFramebuffer_ = nullptr;
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
				std::reinterpret_pointer_cast<void>(mainCamera_)
			);
			registerProperty("terrain_base", util::command::WithWorldTransformParser::uuid,
				std::reinterpret_pointer_cast<void>(terrain_)
			);
			registerProperty("sunlight", util::command::DirectionalLightParser::uuid,
				std::shared_ptr<void>(&sunlight_, [](void*) {})
			);
			registerProperty("skybox", util::command::WithWorldTransformParser::uuid,
				std::reinterpret_pointer_cast<void>(skybox_)
			);
			registerProperty("water_surface", util::command::WithWorldTransformParser::uuid,
				std::reinterpret_pointer_cast<void>(waterSurfaceModel_)
			);

			return true;
		}

		bool LakeScene::teardownProperties()
		{
			unregisterProperty("camera");
			unregisterProperty("terrain_base");
			unregisterProperty("sunlight");
			unregisterProperty("skybox");
			unregisterProperty("water_surface");
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