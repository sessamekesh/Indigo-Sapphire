#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <util/Logger.h>
#include <view/solidshader/solidshader.h>
#include <view/special/metaball/phongshader.h>
#include <memory>
#include <util/camera/StaticCamera.h>
#include <view/solidshader/geo/grid.h>
#include <view/solidshader/geo/Line.h>
#include <view/solidshader/assets/computermonitor.h>
#include <model/specialgeo/metaball/metaballgroup.h>
#include <view/special/metaball/metaballgroup.h>

#include <chrono>

namespace sim
{
	class TestScene
	{
	public:
		TestScene();
		~TestScene();

		bool Init();
		bool Run();

	private:
		bool _isInitialized;

		//
		// GLFW things
		//
	private:
		GLFWwindow* _window;

		//
		// Util Things
		//
	private:
		util::Logger log;

		//
		// Shaders
		//
	private:
		std::shared_ptr<view::solidshader::SolidShader> solidShader_;
		std::shared_ptr<view::special::metaball::PhongShader> metaballPhongShader_;

		bool initializeShaders();
		bool teardownShaders();

		//
		// Cameras and Projections
		//
	private:
		util::camera::StaticCamera camera_;
		glm::mat4 projMatrix_;

		//
		// Logical objects
		//
		std::shared_ptr<model::geo::Grid> gridModel_;
		std::shared_ptr<model::geo::Line> lineModel_;
		std::shared_ptr<model::specialgeo::metaball::MetaballGroup> metaballGroupModel_;

		//
		// Renderable Objects
		//
	private:
		std::shared_ptr<view::solidshader::geo::Grid> grid_;
		std::shared_ptr<view::solidshader::geo::Line> line_;
		std::shared_ptr<view::solidshader::assets::ComputerMonitor> computerMonitor_;
		std::shared_ptr<view::special::metaball::MetaballGroup> metaballGroup_;

		bool initializeSceneObjects();
		bool teardownSceneObjects();

		// Updates
	private:
		bool isFirstFrame_;
		std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameTime_;
	};
}