#include <sim/scene.h>
#include <sstream>
#include <util/io.h>
#include <fstream>
#include <resources.h>

namespace sim
{
	Scene::Scene(
		const std::string& sceneName,
		unsigned int windowWidth,
		unsigned int windowHeight
	)
		: parsers_()
		, properties_()
		, sceneName_(sceneName)
		, log(util::DEBUG, util::WARN, std::string("[") + sceneName + "] ")
		, shadersReady_(false)
		, resourcesReady_(false)
		, propertiesReady_(false)
		, isInitialized_(false)
		, pso_({})
		, window_(nullptr)
		, windowWidth_(windowWidth)
		, windowHeight_(windowHeight)
		, isFirstFrame_(true)
		, lastFrameTime_(std::chrono::high_resolution_clock::now())
		, exit_(false)
		, isRunning_(false)
		, cq_mutex_()
		, cq_(0u)
	{}

	Scene::~Scene()
	{
		Shutdown();
	}

	bool Scene::Init()
	{
		if (isInitialized_)
		{
			log.warn << "Attempting to initialize already initialized scene, ignoring" << util::endl;
			return true;
		}

		if (!glfwInit())
		{
			log.panic << "Failed to initialize glfw!" << util::endl;
			return false;
		}

		glfwWindowHint(GL_MAJOR_VERSION, 4);
		glfwWindowHint(GL_MINOR_VERSION, 5);

		window_ = glfwCreateWindow(windowWidth_, windowHeight_, sceneName_.c_str(), NULL, NULL);
		if (!window_)
		{
			log.error << "Failed to create GLFW window" << util::endl;
			return false;
		}

		glfwMakeContextCurrent(window_);
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		glfwSwapInterval(1);

		//
		// Initialization!
		//
		shadersReady_ = initializeShaders();
		if (!shadersReady_)
		{
			log.error << "Failed to initialize shaders!" << util::endl;
			return false;
		}

		resourcesReady_ = initializeResources();
		if (!resourcesReady_)
		{
			log.error << "Failed to initialize resources!" << util::endl;
			return false;
		}

		propertiesReady_ = initializeProperties();
		if (!propertiesReady_)
		{
			log.error << "Failed to initialize properties!" << util::endl;
			return false;
		}

		isInitialized_ = true;
		log.info << "Scene initialized." << util::endl;

		return true;
	}

	bool Scene::Shutdown()
	{
		if (propertiesReady_)
		{
			if (!teardownProperties())
			{
				log.warn << "Failed to teardown properties!" << util::endl;
			}
			propertiesReady_ = false;
		}

		if (resourcesReady_)
		{
			if (!teardownResources())
			{
				log.warn << "Failed to teardown resources!" << util::endl;
			}
			resourcesReady_ = false;
		}

		if (shadersReady_)
		{
			if (!teardownShaders())
			{
				log.warn << "Failed to teardown shaders!" << util::endl;
			}
			shadersReady_ = false;
		}

		if (window_)
		{
			glfwDestroyWindow(window_);
			window_ = nullptr;
		}

		glfwTerminate();

		return true;
	}

	bool Scene::isRunning()
	{
		return isRunning_;
	}

	bool Scene::Run()
	{
		if (!isInitialized_)
		{
			log.error << "Cannot run uninitialized scene" << util::endl;
			return false;
		}

		log.info << "Running scene" << util::endl;
		isRunning_ = true;
		while (!glfwWindowShouldClose(window_) && !shouldExit())
		{
			// Process commands:
			std::vector<std::vector<std::string>> cpy;
			{
				std::lock_guard<std::mutex> l(cq_mutex_);
				cpy = cq_;
				cq_.resize(0u);
			}
			{
				for (auto&& command : cpy)
				{
					if (!processSceneGeneralCommand(command)
						&& !processCommandAsProperty(command)
						&& !processCommand(command))
					{
						log.warn << "Could not process command: ";
						for (auto&& cw : command)
						{
							log.warn << cw << " ";
						}
						log.warn << util::endl;
					}
				}
			}

			if (isFirstFrame_)
			{
				lastFrameTime_ = std::chrono::high_resolution_clock::now();
				isFirstFrame_ = false;
			}
			else
			{
				float dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - lastFrameTime_).count() / 1000.f;
				lastFrameTime_ = std::chrono::high_resolution_clock::now();
				update(dt);
			}

			render();

			// Present...
			glfwSwapBuffers(window_);
			glfwPollEvents();
		}
		isRunning_ = false;

		return true;
	}

	void Scene::queueCommand(std::string command)
	{
		std::stringstream ss(command);
		std::string item;
		std::vector<std::string> tokens;
		while (std::getline(ss, item, ' '))
		{
			tokens.push_back(item);
		}

		std::lock_guard<std::mutex> l(cq_mutex_);
		cq_.push_back(tokens);
	}

	//
	// Methods to be overridden by the implementing scene
	//
	void Scene::update(float dt) {}
	void Scene::render()
	{
		glClearColor(0x36 / 255.f, 0x45 / 255.f, 0x4f / 255.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	bool Scene::initializeShaders()
	{
		log.warn << "Shader initialization has not been implemented for the current scene!" << util::endl;
		return true;
	}
	bool Scene::teardownShaders()
	{
		log.warn << "Shader cleanup has not been implemented for the current scene!" << util::endl;
		return true;
	}
	bool Scene::initializeResources()
	{
		log.warn << "Resource initialization has not been implemented for the current scene!" << util::endl;
		return true;
	}
	bool Scene::teardownResources()
	{
		log.warn << "Resource cleanup has not been implemented for the current scene!" << util::endl;
		return true;
	}
	bool Scene::initializeProperties()
	{
		log.warn << "Property initialization has not been implemented for the current scene!" << util::endl;
		return true;
	}
	bool Scene::teardownProperties()
	{
		log.warn << "Property cleanup has not been implemented for the current scene!" << util::endl;
		return true;
	}
	bool Scene::shouldExit()
	{
		if (exit_)
		{
			return true;
		}

		return false;
	}
	bool Scene::processCommand(const std::vector<std::string>& terms)
	{
		return false;
	}

	bool Scene::registerParser(std::shared_ptr<util::command::ICommandParser> parser)
	{
		auto uuid = parser->getUUID();
		if (parsers_.find(uuid) != parsers_.end())
		{
			log.warn << "Cannot re-register parser " << uuid << util::endl;
			return false;
		}

		parsers_[uuid] = parser;

		return true;
	}

	bool Scene::registerProperty(std::string propName, std::string parserUUID, std::shared_ptr<void> ref)
	{
		if (parsers_.find(parserUUID) == parsers_.end())
		{
			log.error << "Could not register property " << propName << ": Parser " << parserUUID << " not registered." << util::endl;
			return false;
		}

		if (properties_.find(propName) != properties_.end())
		{
			log.error << "Property " << propName << " has already been registered, will not re-register" << util::endl;
			return false;
		}

		properties_[propName] = {
			parserUUID,
			ref
		};
		return true;
	}

	bool Scene::unregisterProperty(std::string propName)
	{
		return properties_.erase(propName) > 0u;
	}

	bool Scene::processCommandAsProperty(const std::vector<std::string>& terms)
	{
		if (terms.size() < 2u)
		{
			return false;
		}

		std::stringstream ss(terms[1]);
		std::string item;
		std::vector<std::string> tokens;
		while (std::getline(ss, item, '.'))
		{
			tokens.push_back(item);
		}
		std::string rootProp = tokens[0];

		if (properties_.find(rootProp) == properties_.end())
		{
			log.error << "No property by name " << terms[1] << util::endl;
			return false;
		}

		if (terms[0] == "GETP" || terms[0] == "getp")
		{
			auto ref = properties_[rootProp].ref;
			
			auto val = parsers_[properties_[rootProp].ParserUUID]->serializeValue(ref);
			for (auto&& v : val)
			{
				log.panic << "  ";
				for (auto&& sub : v.subVals)
				{
					log.panic << sub;
					if (sub != v.subVals.back()) log.panic << ".";
				}
				log.panic << " " << v.val << util::endl;
			}
			return true;
		}
		else if (terms[0] == "SETP" || terms[0] == "setp")
		{
			if (terms.size() < 3u)
			{
				log.error << "No value provided to set to property " << terms[1] << util::endl;
				return false;
			}

			auto ref = properties_[rootProp].ref;

			auto pass = tokens;
			pass.erase(pass.begin(), pass.begin() + 1u);
			if (!parsers_[properties_[rootProp].ParserUUID]->setValue({ pass, terms[2] }, ref))
			{
				log.error << "Failed to serialize " << terms[2] << " for property " << terms[1] << util::endl;
				return false;
			}

			return true;
		}
		else
		{
			return false;
		}
	}

	bool Scene::processSceneGeneralCommand(const std::vector<std::string>& terms)
	{
		if (terms.size() == 0u)
		{
			return true;
		}

		if (terms.size() == 1u)
		{
			if (terms[0u] == "EXIT" || terms[0u] == "QUIT"
				|| terms[0u] == "exit" || terms[0u] == "quit")
			{
				exit_ = true;
				return true;
			}

			if (terms[0u] == "LISTP" || terms[0u] == "listp")
			{
				log.panic << "Properties in scene:" << util::endl;
				for (auto&& p : properties_)
				{
					log.panic << "  " << p.first << util::endl;
				}
				return true;
			}
		}

		if (terms.size() == 2u)
		{
			if (terms[0u] == "save" || terms[0u] == "SAVE")
			{
				saveConfigToFile(std::string(CONFIG_PATH("")) + terms[1u]);
				return true;
			}
			else if (terms[0u] == "load" || terms[0u] == "LOAD")
			{
				loadConfigFromFile(std::string(CONFIG_PATH("")) + terms[1u]);
				return true;
			}
		}

		return false;
	}

	bool Scene::loadConfigFromFile(std::string fName)
	{
		if (!propertiesReady_)
		{
			log.error << "Cannot load configuration - properties are not set!" << util::endl;
			return false;
		}

		auto fileContents = util::readFileAsync(fName.c_str()).get();
		if (fileContents.size() == 0u)
		{
			log.error << "No config values saved. Exiting..." << util::endl;
			return false;
		}

		std::stringstream ss(fileContents);
		std::string item;
		std::vector<std::string> tokens;
		while (std::getline(ss, item, '\n'))
		{
			tokens.push_back(item);
		}

		bool success = true;
		for (auto&& cmd : tokens)
		{
			this->queueCommand(cmd);
		}

		return true;
	}

	bool Scene::saveConfigToFile(std::string fName)
	{
		if (!propertiesReady_)
		{
			log.error << "Cannot save configuration - properties are not set!" << util::endl;
			return false;
		}

		std::ofstream fout(fName);
		if (!fout)
		{
			log.error << "Could not open up file " << fName << " for writing" << util::endl;
			return false;
		}

		for (auto&& prop : properties_)
		{
			auto propName = prop.first;
			auto ptr = prop.second.ref;

			auto lines = parsers_[prop.second.ParserUUID]->serializeValue(ptr);

			for (auto&& line : lines)
			{
				fout << "SETP " << propName;
				for (auto&& sv : line.subVals)
				{
					fout << "." << sv;
				}
				fout << " " << line.val << std::endl;
			}
		}

		return true;
	}
}