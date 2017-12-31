#pragma once

#include <util/Logger.h>
#include <util/command/parseablevalue.h>
#include <util/command/icommandparser.h>
#include <memory>
#include <view/shader.h>
#include <GLFW/glfw3.h>
#include <mutex>

#include <vector>
#include <map>
#include <chrono>

// Base scene object. Contains a lot of the overhead that will be shared among
//  many scenes, including command parsing from CLI, etc.

namespace sim
{
	class Scene
	{
	public:
		Scene(
			const std::string& sceneName,
			unsigned int windowWidth = 1920u,
			unsigned int windowHeight = 1080u
		);
		~Scene();
		Scene(const Scene&) = delete;

		bool Init();
		bool Shutdown();
		bool Run();
		bool isRunning();

		// This is the one thread-safe thing!
		void queueCommand(std::string command);

		//
		// Initialization and Frame helpers
		// In any non-trivial scene, these should all be overridden
		//
	protected:
		virtual void update(float dt);
		virtual void render();
		virtual bool initializeShaders();
		virtual bool teardownShaders();
		virtual bool initializeResources();
		virtual bool teardownResources();
		virtual bool processCommand(const std::vector<std::string>& terms);
		virtual bool initializeProperties();
		virtual bool teardownProperties();
		virtual bool shouldExit();

		bool registerProperty(std::string propName, std::string parserUUID, std::shared_ptr<void> ref);
		bool unregisterProperty(std::string propName);

		bool registerParser(std::shared_ptr<util::command::ICommandParser> parser);

		bool processCommandAsProperty(const std::vector<std::string>& terms);
		bool processSceneGeneralCommand(const std::vector<std::string>& terms);

		bool loadConfigFromFile(std::string fName);
		bool saveConfigToFile(std::string fName);
		//
		// Properties
		//
	private:
		std::map<std::string, std::shared_ptr<util::command::ICommandParser>> parsers_;
		std::map<std::string, util::command::ParseableValue> properties_;

		//
		// Utility
		//
	protected:
		std::string sceneName_;
		util::Logger log;
		bool shadersReady_;
		bool resourcesReady_;
		bool propertiesReady_;
		bool isInitialized_;
		util::PipelineState pso_;

		//
		// GLFW and game loop
		//
	protected:
		GLFWwindow* window_;
		unsigned int windowWidth_, windowHeight_;
		bool isFirstFrame_;
		std::chrono::time_point<std::chrono::high_resolution_clock> lastFrameTime_;
		bool exit_;
		bool isRunning_;

		//
		// Command queue
		//
	private:
		std::mutex cq_mutex_;
		std::vector<std::vector<std::string>> cq_;
	};
}