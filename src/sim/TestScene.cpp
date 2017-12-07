#include <sim/TestScene.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>

namespace sim
{
	TestScene::TestScene()
		: _isInitialized(false)
		, _window(nullptr)
		, log(util::LOG_LEVEL::DEBUG, util::LOG_LEVEL::INFO, "[TestScene] ")
		, solidShader_(nullptr)
		, camera_(glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f, 1.f, 0.f))
		, gridModel_(nullptr)
		, lineModel_(nullptr)
		, grid_(nullptr)
		, line_(nullptr)
		, projMatrix_(glm::perspective(glm::radians(80.f), 16.f / 9.f, 0.1f, 100.f))
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
			glClearColor(0x2e / 255.f, 0x6c / 255.f, 0x7e / 255.f, 1.f);
			int width, height;
			glfwGetWindowSize(this->_window, &width, &height);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glViewport(0, 0, width, height);
			glEnable(GL_MULTISAMPLE);
			glDisable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);

			if (solidShader_->activate())
			{
				solidShader_->setViewMatrix(camera_.getViewTransform());
				solidShader_->setProjMatrix(projMatrix_);

				grid_->prepare();
				grid_->render(solidShader_);

				line_->prepare();
				line_->render(solidShader_);
			}
			else
			{
				log.warn << "Failed to activate solid shader, not drawing any solid objects this frame" << util::endl;
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
		solidShader_ = std::make_shared<view::solidshader::SolidShader>();
		if (!solidShader_->initialize())
		{
			log.error << "Failed to create solid shader" << util::endl;
			return false;
		}

		return true;
	}

	bool TestScene::teardownShaders()
	{
		if (solidShader_ != nullptr)
		{
			solidShader_ = nullptr;
		}

		return true;
	}

	bool TestScene::initializeSceneObjects()
	{
		gridModel_ = std::make_shared<model::geo::Grid>(0.5f, 0.5f, 5.f, 5.f, glm::vec4(1.f, 0.f, 0.f, 1.f), 0.1f, 0.1f, glm::vec3(-2.f, 0.f, 2.f), glm::quat(), true, glm::vec4(0.1f, 0.1f, 0.1f, 1.f));
		if (!gridModel_)
		{
			return false;
		}

		lineModel_ = std::make_shared<model::geo::Line>(glm::vec3(-1.f, 0.f, 3.f), glm::vec3(1.f, 0.f, 30.f), 0.04f);
		if (!lineModel_)
		{
			return false;
		}

		grid_ = std::make_shared<view::solidshader::geo::Grid>(gridModel_);
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

		return true;
	}

	bool TestScene::teardownSceneObjects()
	{
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