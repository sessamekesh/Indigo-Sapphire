#include <sim/lakescene/lakescene.h>
#include <resources.h>
#include <glm/gtc/matrix_transform.hpp>

#include <view/genericassmiploader.h>
#include <view/rawentities/proctree.h>

// TODO SESS:
// - Model the rock they'll be sitting on
// - Put the XBOT character on the rock
// - Start setting up the actual sequence! (camera, arrows, all that jazz)

namespace sim
{
	namespace lake
	{
		LakeScene::LakeScene(
			util::command::ParserFactory& parserFactory
		)
			: Scene("Lake Scene", 1920u, 1080u)
			, terrainShader_(nullptr)
			, skyboxShader_(nullptr)
			, solidShader_(nullptr)
			, waterSurfaceShader_(nullptr)
			, mappedPhongShader_(nullptr)
			, waterSurfaceModel_(nullptr)
			, testProctreeModel_(nullptr)
			, boulderTest_(nullptr)
			, terrain_(nullptr)
			, testProctreeEntity_(nullptr)
			, mainCamera_(nullptr)
			, waterReflectionCamera_(nullptr)
			, sunlight_(
				glm::vec3(0.1f, 0.1f, 0.1f), // Ambient
				glm::vec3(0.9f, 0.9f, 0.9f), // Diffuse
				glm::vec3(1.f, 1.f, 1.f), // Specular
				glm::vec3(1.f, -1.f, 0.f)) // Direction
			, skybox_(nullptr)
			, lakeSurface_(nullptr)
			, projMatrix_(glm::perspective(glm::radians(45.f), 128.f / 72.f, 0.1f, 15000.0f))
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
			registerParser(parserFactory.waterFlatSurfaceParser());
		}

		void LakeScene::update(float dt)
		{
			lakeSurface_->update(dt);

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

			//
			// Joystick things
			//
			float rotSpeed = 0.05f;
			float moveSpeed = 2.f;
			if (glfwJoystickPresent(GLFW_JOYSTICK_1) == GLFW_TRUE)
			{
				int count;
				const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
				if (count >= 4)
				{
					// Adjust angle first
					mainCamera_->rotateRight(-rotSpeed * axes[2]);
					mainCamera_->rotateUp(-rotSpeed * axes[3]);

					auto dir = glm::normalize(mainCamera_->lookAt() - mainCamera_->pos());
					auto right = glm::normalize(glm::cross(dir, mainCamera_->up()));
					mainCamera_->pos(
						mainCamera_->pos() + moveSpeed * (dir * axes[1] + right * axes[0])
					);
				}
			}
		}

		void LakeScene::renderEnvironment(std::shared_ptr<util::camera::CameraBase> camera, const std::optional<model::geo::Plane>& clipPlane)
		{
			// TODO SESS: Things like depth test, etc., should be handled in the PSO (to avoid setting redundant state)
			// Skybox before all else
			glDepthMask(GL_FALSE);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);
			if (skyboxShader_->activate())
			{
				if (clipPlane)
				{
					skyboxShader_->setClipPlane(*clipPlane);
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
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			if (terrainShader_->activate())
			{
				if (clipPlane)
				{
					terrainShader_->setClipPlane(*clipPlane);
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

			if (mappedPhongShader_->activate())
			{
				if (clipPlane)
				{
					mappedPhongShader_->setClipPlane(*clipPlane);
				}

				mappedPhongShader_->setViewMatrix(camera->getViewTransform());
				mappedPhongShader_->setProjMatrix(projMatrix_);
				mappedPhongShader_->setLight(sunlight_);
				mappedPhongShader_->setShininess(200.f);
				mappedPhongShader_->setCameraPosition(camera->pos());
				
				boulderTest_->render(mappedPhongShader_);
			}

			if (solidShader_->activate())
			{
				if (clipPlane)
				{
					solidShader_->setClipPlane(*clipPlane);
				}

				solidShader_->setViewMatrix(camera->getViewTransform());
				solidShader_->setProjMatrix(projMatrix_);
				testProctreeEntity_->render(solidShader_);
			}
		}

		void LakeScene::render()
		{
			glClearColor(0.f, 0.f, 0.f, 1.f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			int width, height;
			glfwGetWindowSize(window_, &width, &height);
			glViewport(0, 0, width, height);
			
			//
			// Generate water reflection/refraction textures
			//
			auto waterOrigin = waterSurfaceModel_->pos();
			auto waterNormal = glm::rotate(waterSurfaceModel_->rot(), glm::vec3(0.f, 1.f, 0.f));
			auto waterReflectionPlane = model::geo::Plane(waterOrigin, waterNormal);
			auto waterRefractionPlane = model::geo::Plane(waterOrigin, -waterNormal);

			glEnable(GL_CLIP_DISTANCE0);
			waterReflectionCamera_->reflectionPlane(waterReflectionPlane);
			waterReflectionFramebuffer_->bind();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			renderEnvironment(waterReflectionCamera_, waterReflectionPlane);
			waterRefractionFramebuffer_->bind();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			renderEnvironment(mainCamera_, waterRefractionPlane);
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
			glDisable(GL_BLEND);
			if (waterSurfaceShader_->activate())
			{
				waterSurfaceShader_->setViewMatrix(mainCamera_->getViewTransform());
				waterSurfaceShader_->setProjMatrix(projMatrix_);
				waterSurfaceShader_->setCameraPosition(mainCamera_->pos());
				waterSurfaceShader_->setLight(sunlight_);

				lakeSurface_->render(waterSurfaceShader_, waterReflectionFramebuffer_, waterRefractionFramebuffer_);
			}
			else
			{
				log.warn << "Failed to activate water surface shader, not drawing water" << util::endl;
			}
		}

		bool LakeScene::initializeShaders()
		{

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

			mappedPhongShader_ = std::make_shared<view::mappedphong::MappedPhongShader>();
			if (!mappedPhongShader_ || !mappedPhongShader_->initialize())
			{
				log.error << "Failed to create mapped phong shader" << util::endl;
				return false;
			}

			return true;
		}

		bool LakeScene::teardownShaders()
		{
			terrainShader_ = nullptr;
			skyboxShader_ = nullptr;
			solidShader_ = nullptr;
			waterSurfaceShader_ = nullptr;

			return true;
		}

		bool LakeScene::initializeResources()
		{
			mainCamera_ = std::shared_ptr<util::camera::FlightCamera>(new util::camera::FlightCamera(
				glm::vec3(0.f, 0.f, -75.f),
				glm::vec3(0.f, 1.f, 0.f),
				glm::vec3(0.f, 0.f, 1.f),
				0.f, 0.f
			));
			waterReflectionCamera_ = std::shared_ptr<util::camera::PlanarReflectionCamera>(new util::camera::PlanarReflectionCamera(
				mainCamera_,
				model::geo::Plane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f })
			));
			if (!setupTextures())
			{
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
					glm::vec3(0.f, -300.f, 0.f),
					glm::angleAxis(0.01f, glm::vec3(0.f, 1.f, 0.f)),
					glm::vec3(700.f, 700.f, 700.f)
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

			lakeSurface_ = std::shared_ptr<view::special::watersurface::FlatSurface>(
				new view::special::watersurface::FlatSurface(
					waterSurfaceModel_,
					textures_["lake-normal"],
					textures_["lake-dudv"],
					0.05f, // Tiling multiplier
					0.02f, // DUDV scale
					0.f, // DUDV offset
					0.05f, // DUDV offset velocity
					20.f, // Shine damper
					0.6f // Reflectivity
				)
			);
			if (!lakeSurface_ || !lakeSurface_->prepare(waterSurfaceShader_, pso_))
			{
				log.error << "Failed to initialize water surface" << util::endl;
				return false;
			}

			waterReflectionFramebuffer_ = std::make_shared<view::Framebuffer>();
			if (!waterReflectionFramebuffer_ || !waterReflectionFramebuffer_->init(1024, 1024))
			{
				log.error << "Failed to initialize water reflection framebuffer" << util::endl;
				return false;
			}

			waterRefractionFramebuffer_ = std::make_shared<view::Framebuffer>();
			if (!waterRefractionFramebuffer_ || !waterRefractionFramebuffer_->init(1024, 1024))
			{
				log.error << "Failed to initialize water refraction framebuffer" << util::endl;
				return false;
			}

			boulderTest_ = std::shared_ptr<view::mappedphong::AssimpGeo>(new view::mappedphong::AssimpGeo(
				glm::vec3(-180.f, -30.f, -184.f),
				glm::angleAxis(0.2f, glm::vec3(1.f, 0.f, 0.f)) *
					glm::angleAxis(glm::pi<float>(), glm::vec3(0.f, 1.f, 0.f)) *
					glm::angleAxis(-glm::half_pi<float>(), glm::vec3(1.f, 0.f, 0.f)),
				glm::vec3(65.f, 65.f, 65.f),
				"BoulderTest",
				textures_["boulder-normal"],
				textures_["boulder-diffuse"],
				textures_["boulder-specular"]
			));
			if (!boulderTest_ || !boulderTest_->prepare(ASSET_PATH("environment/boulder/boulder.fbx"), mappedPhongShader_, pso_))
			{
				log.error << "Failed to initialize test boulder" << util::endl;
				return false;
			}

			{
				testProctreeModel_ = std::make_shared<Proctree::Tree>();
				testProctreeModel_->mProperties.mSeed = 262;
				testProctreeModel_->mProperties.mLevels = 5;
				testProctreeModel_->mProperties.mVMultiplier = 2.36f;
				testProctreeModel_->mProperties.mTwigScale = 0.239f;
				testProctreeModel_->mProperties.mInitialBranchLength = 0.49f;
				testProctreeModel_->mProperties.mLengthFalloffFactor = 0.85f;
				testProctreeModel_->mProperties.mLengthFalloffPower = 0.99f;
				testProctreeModel_->mProperties.mClumpMax = 0.454f;
				testProctreeModel_->mProperties.mClumpMin = 0.404f;
				testProctreeModel_->mProperties.mBranchFactor = 2.45f;
				testProctreeModel_->mProperties.mDropAmount = -0.1f;
				testProctreeModel_->mProperties.mGrowAmount = 0.235f;
				testProctreeModel_->mProperties.mSweepAmount = 0.01f;
				testProctreeModel_->mProperties.mMaxRadius = 0.139f;
				testProctreeModel_->mProperties.mClimbRate = 0.371f;
				testProctreeModel_->mProperties.mTrunkKink = 0.093f;
				testProctreeModel_->mProperties.mTreeSteps = 5;
				testProctreeModel_->mProperties.mTaperRate = 0.947f;
				testProctreeModel_->mProperties.mRadiusFalloffRate = 0.73f;
				testProctreeModel_->mProperties.mTwistRate = 3.02f;
				testProctreeModel_->mProperties.mTrunkLength = 2.4f;
				testProctreeModel_->generate();

				std::shared_ptr<view::GenericMesh> tree = std::make_shared<view::GenericMesh>();
				std::shared_ptr<view::GenericMesh> branches = std::make_shared<view::GenericMesh>();
				auto t = view::raw::getProcTreeMesh(testProctreeModel_, this->log, *tree, *branches);
				if (!t)
				{
					log.error << "Failed to generate generic proctree models" << util::endl;
					return false;
				}
				testProctreeEntity_ = std::make_shared<view::solidshader::GenericSolidEntity>(
					glm::vec3(-295.f, 0.f, -280.f),
					glm::angleAxis(0.f, glm::vec3(0.f, 0.f, 1.f)),
					glm::vec3(35.f, 35.f, 35.f)
				);
				testProctreeEntity_->addMesh(tree, glm::vec4(1.f, 0.f, 0.f, 1.f));
				testProctreeEntity_->addMesh(branches, glm::vec4(0.f, 0.f, 1.f, 1.f));
				if (!testProctreeEntity_->prepare(solidShader_, pso_))
				{
					log.error << "Failed to prepare teset proctree entity" << util::endl;
					return false;
				}
			}

			return true;
		}

		bool LakeScene::teardownResources()
		{
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

			if (lakeSurface_)
			{
				lakeSurface_ = nullptr;
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
			// TODO SESS: Make a parser for the water surface as well
			registerProperty("camera", util::command::StaticCameraParser::uuid, std::static_pointer_cast<void>(mainCamera_));
			registerProperty("terrain_base", util::command::WithWorldTransformParser::uuid, std::static_pointer_cast<void>(terrain_));
			registerProperty("sunlight", util::command::DirectionalLightParser::uuid, std::shared_ptr<void>(&sunlight_, [](void*) {}));
			registerProperty("skybox", util::command::WithWorldTransformParser::uuid, std::static_pointer_cast<void>(skybox_));
			registerProperty("water", util::command::WaterFlatSurface::uuid, std::static_pointer_cast<void>(lakeSurface_));
			registerProperty("boulder", util::command::WithWorldTransformParser::uuid, std::static_pointer_cast<void>(boulderTest_));
			registerProperty("tree", util::command::WithWorldTransformParser::uuid, std::static_pointer_cast<void>(testProctreeEntity_));

			return true;
		}

		bool LakeScene::teardownProperties()
		{
			unregisterProperty("tree");
			unregisterProperty("boulder");
			unregisterProperty("water");
			unregisterProperty("skybox");
			unregisterProperty("sunlight");
			unregisterProperty("terrain_base");
			unregisterProperty("camera");
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

		bool LakeScene::loadSingleTexture(std::string texName, std::string fName)
		{
			if (textures_.find(texName) != textures_.end())
			{
				log.error << "Texture " << texName << " already registered." << util::endl;
				return false;
			}

			auto opt = model::readPNG(fName);
			if (!opt)
			{
				log.error << "Failed to load image: " << fName << util::endl;
				return false;
			}

			auto texture = std::make_shared<view::Texture>();
			if (!texture->init(view::Texture::RGBA, *opt))
			{
				log.error << "Failed to create texture: " << texName << util::endl;
				return false;
			}

			textures_[texName] = texture;

			return true;
		}

		bool LakeScene::setupTextures()
		{
			if (!loadSingleTexture("dry-grass", ASSET_PATH("texture/dry-grass.png"))) return false;
			if (!loadSingleTexture("lake-dudv", ASSET_PATH("texture/water-dudv.png"))) return false;
			if (!loadSingleTexture("lake-normal", ASSET_PATH("texture/water-normal.png"))) return false;
			if (!loadSingleTexture("boulder-normal", ASSET_PATH("environment/boulder/NormalMap.png"))) return false;
			if (!loadSingleTexture("boulder-specular", ASSET_PATH("environment/boulder/SpecularMap.png"))) return false;
			if (!loadSingleTexture("boulder-diffuse", ASSET_PATH("environment/boulder/DiffuseMap.png"))) return false;
			if (!loadSingleTexture("tree0-bark", ASSET_PATH("environment/trees/pine0/Red_Pine_Bark_diffuse.png"))) return false;
			if (!loadSingleTexture("tree0-leaves", ASSET_PATH("environment/trees/pine0/Pine_Large_diffuse.PNG"))) return false;

			return true;
		}

		bool LakeScene::teardownTextures()
		{
			textures_.clear();

			return true;
		}
	}
}