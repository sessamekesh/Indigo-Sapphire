#include <sim/TestScene.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <model/specialgeo/metaball/debugmetaballgroup.h>

namespace sim
{
	TestScene::TestScene()
		: _isInitialized(false)
		, _window(nullptr)
		, log(util::LOG_LEVEL::DEBUG, util::LOG_LEVEL::INFO, "[TestScene] ")
		, solidShader_(nullptr)
		, metaballPhongShader_(nullptr)
		, camera_(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f, 1.f, 0.f))
		, gridModel_(nullptr)
		, lineModel_(nullptr)
		, metaballGroupModel_(nullptr)
		, grid_(nullptr)
		, line_(nullptr)
		, computerMonitor_(nullptr)
		, metaballGroup_(nullptr)
		, projMatrix_(glm::perspective(glm::radians(90.f), 16.f / 9.f, 0.1f, 100.f))
		, isFirstFrame_(true)
		, lastFrameTime_(std::chrono::high_resolution_clock::now())
	{}
	TestScene::~TestScene()
	{
		teardownSceneObjects();
		teardownShaders();

		if (_window)
		{
			glfwDestroyWindow(_window);
			_window = nullptr;
		}

		glfwTerminate();
	}

	bool TestScene::Init()
	{
		if (_isInitialized)
		{
			return true;
		}

		if (!glfwInit())
		{
			log.panic << "Failed to initialize glfw!" << util::endl;
			return false;
		}

		glfwWindowHint(GL_MAJOR_VERSION, 4);
		glfwWindowHint(GL_MINOR_VERSION, 5);
		glfwWindowHint(GL_SAMPLES, 4);

		_window = glfwCreateWindow(1920, 1080, "Test Scene", NULL, NULL);
		if (!_window)
		{
			log.error << "Failed to create GLFW window!" << util::endl;
			return false;
		}

		glfwMakeContextCurrent(_window);
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		glfwSwapInterval(1);

		if (!initializeShaders())
		{
			log.error << "Failed to create shaders! Exiting" << util::endl;
			return false;
		}

		if (!initializeSceneObjects())
		{
			log.error << "Failed to initialize scene objects! Exiting." << util::endl;
			return false;
		}

		this->_isInitialized = true;
		log.info << "Test scene initialized!" << util::endl;

		return true;
	}

	bool TestScene::Run()
	{
		while (!glfwWindowShouldClose(_window))
		{
			// Basic events...
			glClearColor(0x36 / 255.f, 0x45 / 255.f, 0x4f / 255.f, 1.f);
			int width, height;
			glfwGetWindowSize(this->_window, &width, &height);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glViewport(0, 0, width, height);
			glEnable(GL_MULTISAMPLE);
			glDisable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);

			if (isFirstFrame_)
			{
				lastFrameTime_ = std::chrono::high_resolution_clock::now();
				isFirstFrame_ = false;
			}
			else
			{
				float dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - lastFrameTime_).count() / 1000.f;
				lastFrameTime_ = std::chrono::high_resolution_clock::now();

				metaballGroupModel_->update(dt);
			}

			glDisable(GL_BLEND);
			if (solidShader_->activate())
			{
				solidShader_->setViewMatrix(camera_.getViewTransform());
				solidShader_->setProjMatrix(projMatrix_);

				grid_->prepare();
				grid_->render(solidShader_);

				line_->prepare();
				line_->render(solidShader_);

				//computerMonitor_->prepare();
				//computerMonitor_->render(solidShader_);
			}
			else
			{
				log.warn << "Failed to activate solid shader, not drawing any solid objects this frame" << util::endl;
			}

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			if (metaballPhongShader_->activate())
			{
				metaballPhongShader_->setViewMatrix(camera_.getViewTransform());
				metaballPhongShader_->setCameraPos(camera_.pos());
				metaballPhongShader_->setProjMatrix(projMatrix_);
				metaballPhongShader_->setStepSize(0.08f);
				metaballPhongShader_->setLight(
					glm::vec3(0.1f, 0.1f, 0.1f),
					glm::vec3(1.f, 1.f, 1.f),
					glm::vec3(1.f, 1.f, 1.f)
				);
				metaballPhongShader_->setLightDir(
					glm::normalize(glm::vec3(1.f, -2.f, 3.f))
				);

				if (metaballGroup_->preparePhong(
					camera_.pos(),
					camera_.lookDir(),
					camera_.up()
				)) {
					metaballGroup_->renderPhong(metaballPhongShader_);
				}
				else
				{
					// log.debug << "Metaball group prepare failed. Perhaps it is behind the camera?" << util::endl;
				}
			}
			else
			{
				log.warn << "Failed to activate metaball phong shader, not drawing any phong metaballs this frame" << util::endl;
			}

			// Present...
			glfwSwapBuffers(_window);
			glfwPollEvents();
		}
		log.info << "Test scene finished running!" << util::endl;

		return true;
	}

	bool TestScene::initializeShaders()
	{
		log.info << "Creating shaders...";
		solidShader_ = std::make_shared<view::solidshader::SolidShader>();
		if (!solidShader_->initialize())
		{
			log.error << "Failed to create solid shader" << util::endl;
			return false;
		}

		metaballPhongShader_ = std::make_shared<view::special::metaball::PhongShader>();
		if (!metaballPhongShader_->initialize())
		{
			log.error << "Failed to create metaball phong shader" << util::endl;
			return false;
		}
		log.info << "Shaders created";

		return true;
	}

	bool TestScene::teardownShaders()
	{
		log.warn << "Tearing down shaders..." << util::endl;
		if (metaballPhongShader_ != nullptr)
		{
			metaballPhongShader_ = nullptr;
		}

		if (solidShader_ != nullptr)
		{
			solidShader_ = nullptr;
		}
		log.warn << "Shaders destroyed" << util::endl;

		return true;
	}

	bool TestScene::initializeSceneObjects()
	{
		gridModel_ = std::shared_ptr<model::geo::Grid>(new model::geo::Grid(
			5.f, 5.f, 100.f, 100.f,
			0.085f,
			glm::vec3(-50.f, -7.f, 5.f),
			glm::angleAxis(glm::half_pi<float>(), glm::vec3(1.f, 0.f, 0.f)),
			true, glm::vec4(0.1f, 0.1f, 0.1f, 1.f)
		));
		if (!gridModel_)
		{
			return false;
		}

		lineModel_ = std::make_shared<model::geo::Line>(glm::vec3(-1.f, 0.f, 3.f), glm::vec3(1.f, 0.f, 30.f), 0.04f);
		if (!lineModel_)
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

		grid_ = std::make_shared<view::solidshader::geo::Grid>(
			gridModel_,
			16u,
			glm::vec4(0x40/255.f, 0x5e/255.f, 0x5b/255.f, 1.f) // Grid color
		);
		if (!grid_ || !grid_->prepare())
		{
			log.error << "Failed to initialize grid object." << util::endl;
			return false;
		}

		line_ = std::make_shared<view::solidshader::geo::Line>(lineModel_, 8, glm::vec4(1.f, 0.f, 0.f, 1.f));
		if (!line_ || !line_->prepare())
		{
			log.error << "Failed to initialize line object." << util::endl;
			return false;
		}

		computerMonitor_ = std::shared_ptr<view::solidshader::assets::ComputerMonitor>(
			new view::solidshader::assets::ComputerMonitor(
				glm::vec3(0.f, -7.f, 25.f),
				glm::angleAxis(0.f, glm::vec3(1.f, 0.f, 0.f)),
				glm::vec3(4.f, 4.f, 4.f)
			)
		);
		if (!computerMonitor_ || !computerMonitor_->prepare())
		{
			log.error << "Failed to initialize computer monitor object." << util::endl;
			return false;
		}

		metaballGroup_ = std::shared_ptr<view::special::metaball::MetaballGroup>(
			new view::special::metaball::MetaballGroup(
				metaballGroupModel_,
				glm::vec3(1.f, 0.f, 0.f),
				glm::vec3(1.f, 1.f, 1.f),
				100.f
			)
		);
		if (!metaballGroup_ || !metaballGroup_->preparePhong(camera_.pos(), camera_.lookDir(), camera_.up()))
		{
			log.error << "Failed to initialize metaball group object." << util::endl;
			return false;
		}

		return true;
	}

	bool TestScene::teardownSceneObjects()
	{
		if (metaballGroup_)
		{
			metaballGroup_->release();
			metaballGroup_ = nullptr;
		}
		if (computerMonitor_)
		{
			computerMonitor_->release();
			computerMonitor_ = nullptr;
		}
		if (line_)
		{
			line_->release();
			line_ = nullptr;
		}
		if (lineModel_)
		{
			lineModel_ = nullptr;
		}

		if (grid_)
		{
			grid_->release();
			grid_ = nullptr;
		}

		if (gridModel_)
		{
			gridModel_ = nullptr;
		}

		return true;
	}
}