#include <sim/lakescene/lakescene.h>
#include <resources.h>
#include <glm/gtc/matrix_transform.hpp>
#include <input/gamepadcameracontroller.h>

#include <view/genericassmiploader.h>
#include <view/rawentities/proctree.h>
#include <util/surfacemask/terrainmapcolormask.h>
#include <util/surfacemask/combinationmask.h>
#include <util/surfacemask/heightmapmask.h>
#include <model/specialgeo/heightfield/heightmapheightfield.h>
#include <model/specialgeo/heightfield/reducedheightfield.h>

#include <model/specialgeo/projection/perspectiveprojection.h>
#include <model/specialgeo/projection/orthographicprojection.h>

// Grass things - should move into their own grass file!
#include <util/math/quadraticeaseout11.h>
#include <util/math/linear11.h>
#include <util/surfacemask/combinationprobabilityfield.h>

// TODO SESS: Start collecting footage of you actually programming/working on this for buzz reel

// TODO SESS: You have a lovely thread pool implementation, use it to start generating CPU-expensive stuff
//  in a future, and then generating the GPU stuff on the main thread when ready.
// Especially stuff that's expensive to generate, like the grass!
// Another good idea: Generate it offline, and load it from file?
// May not actually be faster. File reads are also not free. (How long to read a 56MB file vs. generate 56MB of grass data?)

// TODO SESS: Next steps
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
			, cameraController_(nullptr)
			, blendedTerrainShader_(nullptr)
			, skyboxShader_(nullptr)
			, solidShader_(nullptr)
			, waterSurfaceShader_(nullptr)
			, mappedPhongShader_(nullptr)
			, grassShader_(nullptr)
			, threadPool_(8u) // TODO SESS: This should be based on the number of CPU cores perhaps?
			, bladedGrass_(nullptr)
			, waterSurfaceModel_(nullptr)
			, testProctreeModel_(nullptr)
			, projection_(nullptr)
			, boulderTest_(nullptr)
			, testProctreeEntity_(nullptr)
			, heightMapTerrainRawEntity_(nullptr)
			, blendedTerrainEntity_(nullptr)
			, mainCamera_(nullptr)
			, heightmapCamera_(nullptr)
			, waterReflectionCamera_(nullptr)
			, sunlight_(
				glm::vec3(0.1f, 0.1f, 0.1f), // Ambient
				glm::vec3(0.9f, 0.9f, 0.9f), // Diffuse
				glm::vec3(1.f, 1.f, 1.f), // Specular
				glm::vec3(1.f, -1.f, 0.f)) // Direction
			, skybox_(nullptr)
			, lakeSurface_(nullptr)
			, textures_({})
			, terrainHeightmap_(nullptr)
			, waterReflectionFramebuffer_(nullptr)
			, waterRefractionFramebuffer_(nullptr)
			, textureGenFramebuffer_(nullptr)
			, heightmapMaxHeight_(20.f)
			, terrainOffsetY_(-12.5f)
			, terrainDimensions_(150.f, 150.f)
			, grassProbabilityMask_(nullptr)
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
			bladedGrass_->update(dt);

			lakeSurface_->update(dt);
			for (auto&& ge : grassEntities_)
			{
				ge->update(dt);
			}
			cameraController_->tick();

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

			heightmapCamera_->update(dt);

			//
			// Joystick things
			//
			float rotSpeed = 1.8f;
			float moveSpeed = 4.4f;
			if (glfwJoystickPresent(GLFW_JOYSTICK_1) == GLFW_TRUE)
			{
				int count;
				const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &count);
				if (count >= 4)
				{
					// Adjust angle first
					mainCamera_->rotateRight(-rotSpeed * dt * axes[2]);
					mainCamera_->rotateUp(-rotSpeed * dt * axes[3]);

					auto dir = glm::normalize(mainCamera_->lookAt() - mainCamera_->pos());
					auto right = glm::normalize(glm::cross(dir, mainCamera_->up()));
					mainCamera_->pos(
						mainCamera_->pos() + moveSpeed * dt * (dir * axes[1] + right * axes[0])
					);
				}
			}
		}

		void LakeScene::renderTerrain(std::shared_ptr<util::camera::CameraBase> camera, std::shared_ptr<model::specialgeo::ProjectionBase> projection, const std::optional<model::geo::Plane>& clipPlane)
		{
			pso_.disableBlending();
			pso_.enableDepthTest();
			pso_.enableDepthMask();
			if (blendedTerrainShader_->activate())
			{
				if (clipPlane)
				{
					blendedTerrainShader_->setClipPlane(*clipPlane);
				}

				blendedTerrainShader_->setViewMatrix(camera->getViewTransform());
				blendedTerrainShader_->setProjMatrix(projection->getProjectionMatrix());
				blendedTerrainShader_->setLight(sunlight_);

				blendedTerrainEntity_->render(blendedTerrainShader_);
			}
		}

		void LakeScene::renderEnvironment(std::shared_ptr<util::camera::CameraBase> camera, std::shared_ptr<model::specialgeo::ProjectionBase> projection, const std::optional<model::geo::Plane>& clipPlane)
		{
			// TODO SESS: Things like depth test, etc., should be handled in the PSO (to avoid setting redundant state)
			// Skybox before all else
			pso_.disableDepthTest();
			pso_.disableDepthMask();
			pso_.disableBlending();
			if (skyboxShader_->activate())
			{
				if (clipPlane)
				{
					skyboxShader_->setClipPlane(*clipPlane);
				}

				skyboxShader_->setProjMatrix(projection->getProjectionMatrix());
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
			pso_.enableDepthTest();
			pso_.enableDepthMask();
			if (mappedPhongShader_->activate())
			{
				if (clipPlane)
				{
					mappedPhongShader_->setClipPlane(*clipPlane);
				}

				mappedPhongShader_->setViewMatrix(camera->getViewTransform());
				mappedPhongShader_->setProjMatrix(projection->getProjectionMatrix());
				mappedPhongShader_->setLight(sunlight_);
				mappedPhongShader_->setShininess(200.f);
				mappedPhongShader_->setCameraPosition(camera->pos());
				
				boulderTest_->render(mappedPhongShader_);
			}

			// TODO SESS: There's a bug here, somehow.
			renderTerrain(camera, projection, clipPlane);

			if (grassShader_->activate())
			{
				if (clipPlane)
				{
					grassShader_->setClipPlane(*clipPlane);
				}

				grassShader_->setViewMatrix(camera->getViewTransform());
				grassShader_->setProjMatrix(projection->getProjectionMatrix());
				grassShader_->setEyePos(camera->pos());

				for (auto&& ge : grassEntities_)
				{
					ge->render(grassShader_);
				}
			}

			if (solidShader_->activate())
			{
				if (clipPlane)
				{
					solidShader_->setClipPlane(*clipPlane);
				}

				solidShader_->setViewMatrix(camera->getViewTransform());
				solidShader_->setProjMatrix(projection->getProjectionMatrix());
				testProctreeEntity_->render(solidShader_);
				//genericSolidTerrain_->render(solidShader_);
			}

			bladedGrass_->render(
				camera, projection, sunlight_, clipPlane
			);
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
			renderEnvironment(waterReflectionCamera_, projection_, waterReflectionPlane);
			waterRefractionFramebuffer_->bind();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			//renderEnvironment(mainCamera_, waterRefractionPlane);
			renderEnvironment(heightmapCamera_, projection_, waterRefractionPlane);
			view::Framebuffer::bindDefaultFramebuffer();
			glViewport(0, 0, width, height);

			//
			// Render to main color/depth buffer
			//
			glDisable(GL_CLIP_DISTANCE0);
			//renderEnvironment(mainCamera_);
			renderEnvironment(heightmapCamera_, projection_);

			//
			// Water surface
			//
			glDisable(GL_CULL_FACE);
			pso_.disableBlending();
			if (waterSurfaceShader_->activate())
			{
				//waterSurfaceShader_->setViewMatrix(mainCamera_->getViewTransform());
				waterSurfaceShader_->setViewMatrix(heightmapCamera_->getViewTransform());
				waterSurfaceShader_->setProjMatrix(projection_->getProjectionMatrix());
				//waterSurfaceShader_->setCameraPosition(mainCamera_->pos());
				waterSurfaceShader_->setCameraPosition(heightmapCamera_->pos());
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
			blendedTerrainShader_ = std::make_shared<view::terrainshader::BlendedTerrainShader>();
			if (!blendedTerrainShader_ || !blendedTerrainShader_->initialize())
			{
				log.error << "Failed to create blended terrain shader" << util::endl;
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

			grassShader_ = std::make_shared<view::grass::BillboardGrassShader>();
			if (!grassShader_ || !grassShader_->initialize())
			{
				log.error << "Failed to create grass shader" << util::endl;
				return false;
			}

			return true;
		}

		bool LakeScene::teardownShaders()
		{
			blendedTerrainShader_ = nullptr;
			skyboxShader_ = nullptr;
			solidShader_ = nullptr;
			waterSurfaceShader_ = nullptr;

			return true;
		}

		bool LakeScene::initializeResources()
		{
			if (!setupTextures())
			{
				return false;
			}

			//
			// Cameras
			//
			// TODO SESS: Move camera initialization to its own method, since there is a lot of cameras/angles
			mainCamera_ = std::shared_ptr<util::camera::FlightCamera>(new util::camera::FlightCamera(
				glm::vec3(0.f, 2.f, 0.f),
				glm::vec3(0.f, 1.f, 0.f),
				glm::vec3(0.f, 0.f, 1.f),
				0.f, 0.f
			));

			auto heightfield = std::shared_ptr<model::specialgeo::Heightfield>(
				new model::specialgeo::HeightmapHeightfield(terrainHeightmap_, terrainDimensions_.x / 2.f, terrainDimensions_.y / 2.f, heightmapMaxHeight_)
			);
			auto reducedHeightfield = std::shared_ptr<model::specialgeo::Heightfield>(
				new model::specialgeo::ReducedHeightfield(heightfield, 380u, 380u)
			);

			cameraController_ = std::make_shared<input::GamepadCameraController>(0);
			heightmapCamera_ = std::shared_ptr<util::camera::HeightmapCamera>(new util::camera::HeightmapCamera(
				glm::vec3(0.f, 2.f, 0.f),
				glm::vec3(0.f, 1.f, 0.f),
				glm::vec3(0.f, 0.f, 1.f),
				cameraController_,
				reducedHeightfield,
				-10.8f,
				0.f, 0.f, 4.3f, 1.8f
			));

			waterReflectionCamera_ = std::shared_ptr<util::camera::PlanarReflectionCamera>(new util::camera::PlanarReflectionCamera(
				heightmapCamera_,
				model::geo::Plane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f })
			));

			//
			// Projections
			//
			projection_ = std::make_shared<model::specialgeo::PerspectiveProjection>(glm::radians(45.f), 128.f / 72.f, 0.1f, 15000.0f);

			//
			// Resources
			//
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
					glm::vec3(0.f, 0.f, 0.f),
					glm::angleAxis(0.f, glm::vec3(0.f, 0.f, 0.f)),
					500.f, 500.f
				)
			);

			lakeSurface_ = std::shared_ptr<view::special::watersurface::FlatSurface>(
				new view::special::watersurface::FlatSurface(
					waterSurfaceModel_,
					textures_["lake-normal"],
					textures_["lake-dudv"],
					0.25f, // Tiling multiplier
					0.02f, // DUDV scale
					0.f, // DUDV offset
					0.05f, // DUDV offset velocity
					200.f, // Shine damper
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

			textureGenFramebuffer_ = std::make_shared<view::Framebuffer>();

			if (!setupTerrain(reducedHeightfield))
			{
				log.error << "Failed to setup terrain" << util::endl;
				return false;
			}

			return true;
		}

		bool LakeScene::teardownResources()
		{			
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
			// TODO SESS: Commands should instead be registered, and probably handled via a trie or something.
			if (terms.size() == 3u)
			{
				if (terms[0u] == "gen" && terms[1u] == "terrain-heightmap")
				{
					auto fName = terms[2u];
					auto texSize = 1024u;
					model::GreyscaleImageData heightmap;
					generateHeightmap(texSize, [this](auto camera, auto projection)->void {

						glClearColor(0.f, 0.f, 0.f, 1.f);
						glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
						
						renderTerrain(camera, projection);

					}, heightmap);

					auto path = std::string(RENDER_PATH("") + fName);
					if (!model::writePNG(heightmap, path))
					{
						log.error << "Failed to write PNG to file " << path << "!" << util::endl;
						return false;
					}
					else
					{
						return true;
					}
				}
				else if (terms[0u] == "gen" && terms[1u] == "grass-probability-field")
				{
					auto fName = terms[2u];
					auto texSize = 1024u;
					model::GreyscaleImageData probabilityField;
					probabilityField.height = texSize;
					probabilityField.width = texSize;
					probabilityField.pixels.resize(texSize * texSize);

					for (auto row = 0u; row < texSize; row++)
					{
						for (auto col = 0u; col < texSize; col++)
						{
							auto pxIdx = row * texSize + col;

							glm::vec2 loc;
							loc.x = (((float)row / (float)texSize) * 2.f - 1.f) * terrainDimensions_.x * 0.5f;
							loc.y = (((float)col / (float)texSize) * 2.f - 1.f) * terrainDimensions_.y * 0.5f;
							auto probability = grassProbabilityMask_->getProbabilityAtPoint(loc);

							probabilityField.pixels[pxIdx] = probability * 255.f;
						}
					}

					auto path = std::string(RENDER_PATH("") + fName);
					if (!model::writePNG(probabilityField, path))
					{
						log.error << "Failed to write PNG to file " << path << "!" << util::endl;
						return false;
					}
					else
					{
						return true;
					}
				}
			}

			return false;
		}

		bool LakeScene::initializeProperties()
		{
			// This is a wee bit awkward, eh?
			registerProperty("camera", util::command::StaticCameraParser::uuid, std::static_pointer_cast<void>(mainCamera_));
			registerProperty("terrain_base", util::command::WithWorldTransformParser::uuid, std::static_pointer_cast<void>(blendedTerrainEntity_));
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

		bool LakeScene::loadSingleTexture(std::string texName, std::shared_ptr<model::ImageData> imageData)
		{
			if (textures_.find(texName) != textures_.end())
			{
				log.error << "Texture " << texName << " already registered." << util::endl;
				return false;
			}

			auto texture = std::make_shared<view::Texture>();
			if (!texture->init(view::Texture::RGBA, *imageData))
			{
				log.error << "Failed to create texture: " << texName << util::endl;
				return false;
			}

			textures_[texName] = texture;

			return true;
		}

		bool LakeScene::setupTextures()
		{
			if (!loadSingleTexture("lake-dudv", ASSET_PATH("texture/water-dudv.png"))) return false;
			if (!loadSingleTexture("lake-normal", ASSET_PATH("texture/water-normal.png"))) return false;
			if (!loadSingleTexture("boulder-normal", ASSET_PATH("environment/boulder/NormalMap.png"))) return false;
			if (!loadSingleTexture("boulder-specular", ASSET_PATH("environment/boulder/SpecularMap.png"))) return false;
			if (!loadSingleTexture("boulder-diffuse", ASSET_PATH("environment/boulder/DiffuseMap.png"))) return false;
			if (!loadSingleTexture("tree0-bark", ASSET_PATH("environment/trees/pine0/Red_Pine_Bark_diffuse.png"))) return false;
			if (!loadSingleTexture("tree0-leaves", ASSET_PATH("environment/trees/pine0/Pine_Large_diffuse.PNG"))) return false;
			if (!loadSingleTexture("terrainDirt", ASSET_PATH("texture/dry-grass.png"))) return false;
			if (!loadSingleTexture("terrainGrass", ASSET_PATH("texture/grasstexture.png"))) return false;
			if (!loadSingleTexture("terrainMud", ASSET_PATH("texture/road.png"))) return false;
			if (!loadSingleTexture("grassPack", ASSET_PATH("texture/grassPack.png"))) return false;
			if (!loadSingleTexture("grassBlade", ASSET_PATH("texture/grassBlade.png"))) return false;

			{
				auto opt = model::readGreyscalePNG(ASSET_PATH("environment/terrain_base/heightmap.png"));
				if (!opt)
				{
					log.error << "Failed to load terrain heightmap" << util::endl;
					return false;
				}
				terrainHeightmap_ = std::make_shared<model::GreyscaleImageData>();
				*terrainHeightmap_ = *opt;
			}

			{
				auto opt = model::readPNG(ASSET_PATH("environment/terrain_base/terraintypemap.png"));
				if (!opt)
				{
					log.error << "Failed to load terrain blend map" << util::endl;
					return false;
				}
				terrainBlendMapImage_ = std::make_shared<model::ImageData>(*opt);
			}
			if (!loadSingleTexture("terrainBlendMap", terrainBlendMapImage_)) return false;
			return true;
		}

		bool LakeScene::teardownTextures()
		{
			textures_.clear();

			return true;
		}

		bool LakeScene::setupTerrain(std::shared_ptr<model::specialgeo::Heightfield> generatedHeightfield)
		{
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

			{
				heightMapTerrainRawEntity_ = std::make_shared<view::raw::HeightmapTerrainEntity>(generatedHeightfield);

				blendedTerrainEntity_ = std::shared_ptr<view::terrainshader::GenericBlendedTerrainEntity>(new view::terrainshader::GenericBlendedTerrainEntity(
					glm::vec3(0.f, terrainOffsetY_, 0.f),
					glm::angleAxis(0.f, glm::vec3(1.f, 0.f, 0.f)),
					glm::vec3(1.f, 1.f, 1.f),
					heightMapTerrainRawEntity_->getMeshData(380u, 380u, 1.f, true),
					textures_["terrainBlendMap"],
					textures_["terrainDirt"],
					glm::max(terrainDimensions_.x, terrainDimensions_.y),
					textures_["terrainGrass"],
					glm::max(terrainDimensions_.x, terrainDimensions_.y),
					textures_["terrainMud"],
					glm::max(terrainDimensions_.x, terrainDimensions_.y)
				));
				if (!blendedTerrainEntity_ || !blendedTerrainEntity_->prepare(blendedTerrainShader_, pso_))
				{
					log.error << "Failed to generate blended terrain entity" << util::endl;
					return false;
				}
			}

			auto aboveWaterHeightfieldMask = std::shared_ptr<util::HeightmapMask>(
				new util::HeightmapMask(generatedHeightfield, -terrainOffsetY_, 1000.f)
			);

			auto probabilityCurve = std::make_shared<util::math::Linear11>(0.5f, 0.f, 0.95f, 1.f);
			grassProbabilityMask_ = std::shared_ptr<util::TerrainMapColorProbabilityField>(new util::TerrainMapColorProbabilityField(
				terrainBlendMapImage_, // Image data of the blended terrain
				terrainDimensions_.x / 2.f, terrainDimensions_.y / 2.f,
				probabilityCurve, // 1:1 probability curve (linear?)
				{ util::COLOR_COMPONENT_G },
				{ util::COLOR_COMPONENT_B, util::COLOR_COMPONENT_R, util::COLOR_COMPONENT_G }
			));
			bladedGrass_ = std::shared_ptr<sim::lake::BladedGrass>(new sim::lake::BladedGrass(
				generatedHeightfield,
				grassProbabilityMask_,
				aboveWaterHeightfieldMask,
				glm::vec3(0.f, terrainOffsetY_, 0.f),
				sim::lake::BladedGrass::STANDARD_COMPILED_CONFIG
			));

			if (!(bladedGrass_ && bladedGrass_->prepare(-terrainDimensions_ * 0.5f, terrainDimensions_ * 0.5f, pso_, threadPool_)))
			{
				log.error << "Failed to create bladed grass subsystem" << util::endl;
				return false;
			}

			{
				std::vector<float> threshholds = { 0.2f, 0.4f };
				std::vector<float> offsetMins = { 1.0f, 0.5f };
				std::vector<float> offsetMaxs = { 2.6f, 0.9f };
				std::vector<std::uint32_t> seeds = { 1, 2 };
				grassEntities_.reserve(2u);
				// TODO SESS:
				// - Different types of vegetation, to give variety.
				// - Combine with a "heightfield" sample, to make sure no grass grows below water
				//   (That will also require a "combinationmask", to combine TerrainMapColorMask
				//      and HeightfieldMask)
				for (std::uint32_t idx = 0u; idx < threshholds.size(); idx++)
				{
					grassEntities_.push_back(
						std::make_shared<view::grass::BillboardGrassEntity>(
							glm::vec3(0.f, terrainOffsetY_, 0.f),
							glm::angleAxis(0.f, glm::vec3(0.f, 1.f, 0.f)),
							glm::vec3(1.f, 1.f, 1.f)
							)
					);
					auto terrainColorMask = std::shared_ptr<util::TerrainMapColorMask>(
						new util::TerrainMapColorMask(
							terrainBlendMapImage_,
							terrainDimensions_.x * 0.5f, terrainDimensions_.y * 0.5f,
							threshholds[idx],
							{ util::COLOR_COMPONENT_G },
							{ util::COLOR_COMPONENT_R, util::COLOR_COMPONENT_G, util::COLOR_COMPONENT_B },
							true
						)
						);
					auto combinedMask = std::shared_ptr<util::SurfaceMaskBase>(
						new util::CombinationMask({ aboveWaterHeightfieldMask, terrainColorMask })
						);
					if (!grassEntities_[idx] || !grassEntities_[idx]->prepare(
						grassShader_, pso_,
						generatedHeightfield, textures_["grassPack"],
						combinedMask,
						seeds[idx], offsetMins[idx], offsetMaxs[idx], 1.05f
					))
					{
						log.error << "Failed to generate grass entity " << idx << util::endl;
						return false;
					}
				}
			}
		}

		bool LakeScene::teardownTerrain()
		{
			if (blendedTerrainEntity_)
			{
				blendedTerrainEntity_->release();
				blendedTerrainEntity_ = nullptr;
			}

			if (boulderTest_)
			{
				boulderTest_->release();
				boulderTest_ = nullptr;
			}

			for (auto&& grassEntity : grassEntities_)
			{
				grassEntity->release();
				grassEntity = nullptr;
			}
			grassEntities_.resize(0u);

			heightMapTerrainRawEntity_ = nullptr;

			if (testProctreeEntity_)
			{
				testProctreeEntity_->release();
				testProctreeEntity_ = nullptr;
			}

			if (testProctreeModel_)
			{
				testProctreeModel_ = nullptr;
			}

			if (bladedGrass_)
			{
				bladedGrass_->release();
				bladedGrass_ = nullptr;
			}

			return true;
		}

		bool LakeScene::generateHeightmap(std::uint32_t textureWidth, std::function<void(std::shared_ptr<util::camera::CameraBase>, std::shared_ptr<model::specialgeo::ProjectionBase>)> actions, model::GreyscaleImageData& o_img)
		{
			if (!textureGenFramebuffer_->init(textureWidth, textureWidth))
			{
				log.error << "Failed to initialize texture generation framebuffer!" << util::endl;
				return false;
			}

			textureGenFramebuffer_->bind();
			auto camera = std::shared_ptr<util::camera::CameraBase>(new util::camera::StaticCamera(glm::vec3(0.f, heightmapMaxHeight_ + terrainOffsetY_, 0.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, -1.f)));
			float zNear = 0.f;
			float zFar = heightmapMaxHeight_;
			auto projection = std::shared_ptr<model::specialgeo::ProjectionBase>(new model::specialgeo::OrthographicProjection(-terrainDimensions_.x / 2.f, terrainDimensions_.x / 2.f, -terrainDimensions_.y / 2.f, terrainDimensions_.y / 2.f, zNear, zFar));

			actions(camera, projection);

			o_img.height = textureWidth;
			o_img.width = textureWidth;
			o_img.pixels.clear();
			o_img.pixels.resize(textureWidth * textureWidth);
			glReadPixels(0u, 0u, textureWidth, textureWidth, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, &o_img.pixels[0]);

			// TODO SESS: Do GPU side post-processing instead of CPU-side post processing
			for (auto row = 0u; row < textureWidth / 2u; row++)
			{
				for (auto col = 0u; col < textureWidth; col++)
				{
					auto invRow = textureWidth - 1u - row;
					std::swap(o_img.pixels[row * textureWidth + col], o_img.pixels[invRow * textureWidth + col]);
				}
			}
			for (auto idx = 0ull; idx < textureWidth * textureWidth; idx++)
			{
				o_img.pixels[idx] = 255u - o_img.pixels[idx];
			}

			view::Framebuffer::bindDefaultFramebuffer();

			return true;
		}
	}
}