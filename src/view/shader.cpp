#include <view/shader.h>
#include <util/io.h>

namespace view
{
	Shader::Shader(
		std::string logName,
		std::string vsFileName,
		std::string fsFileName
	)
		: isLoaded_(false)
		, program_(0u)
		, log(util::WARN, util::DEBUG, logName.c_str())
		, vsFname_(vsFileName)
		, fsFname_(fsFileName)
	{}

	Shader::~Shader()
	{
		if (isLoaded_)
		{
			glDeleteProgram(program_);
			program_ = NULL;
			isLoaded_ = false;
		}
	}

	bool Shader::initialize()
	{
		if (isLoaded_)
		{
			log.warn << "Attempted to initialize an already loaded shader" << util::endl;
			return false;
		}

		auto vsTextFuture = util::readFileAsync(vsFname_.c_str());
		auto fsTextFuture = util::readFileAsync(fsFname_.c_str());

		std::string vsText = vsTextFuture.get();
		if (vsText.length() == 0)
		{
			log.error << "Could not read vertex shader text" << util::endl;
			return false;
		}
		const char* ccVSText = vsText.c_str();
		GLuint vs = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vs, 1, &ccVSText, NULL);
		glCompileShader(vs);
		GLint success = 0;
		glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE)
		{
			char* errLog = new char[Shader::MAX_INFO_LOG_LENGTH];
			glGetShaderInfoLog(vs, Shader::MAX_INFO_LOG_LENGTH, nullptr, errLog);
			log.error << "Failed to compile solid shader vertex shader! Error below\n=====BEGIN ERROR LOG=====\n";
			log.error << errLog;
			log.error << "\n======END ERROR LOG=====" << util::endl;
			delete[] errLog;
			glDeleteShader(vs);

			return false;
		}

		std::string fsText = fsTextFuture.get();
		if (fsText.length() == 0)
		{
			log.error << "Could not read vertex shader text!" << util::endl;
			return false;
		}
		const char* ccFSText = fsText.c_str();
		GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fs, 1, &ccFSText, NULL);
		glCompileShader(fs);
		glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE)
		{
			char* errLog = new char[Shader::MAX_INFO_LOG_LENGTH];
			glGetShaderInfoLog(fs, Shader::MAX_INFO_LOG_LENGTH, nullptr, errLog);
			log.error << "Failed to compile solid shader fragment shader! Error below\n=====BEGIN ERROR LOG=====\n";
			log.error << errLog;
			log.error << "\n======END ERROR LOG=====" << util::endl;
			delete[] errLog;
			glDeleteShader(vs);
			glDeleteShader(fs);

			return false;
		}

		program_ = glCreateProgram();
		glAttachShader(program_, vs);
		glAttachShader(program_, fs);
		glLinkProgram(program_);
		glGetProgramiv(program_, GL_LINK_STATUS, &success);
		if (success == GL_FALSE)
		{
			char* errLog = new char[Shader::MAX_INFO_LOG_LENGTH];
			glGetProgramInfoLog(program_, Shader::MAX_INFO_LOG_LENGTH, nullptr, errLog);
			log.error << "Failed to link solid shader program! Error below\n=====BEGIN ERROR LOG=====\n";
			log.error << errLog;
			log.error << "\n=====END ERROR LOG=====\n";
			delete[] errLog;

			glDeleteProgram(program_);
			glDeleteShader(vs);
			glDeleteShader(fs);

			return false;
		}

		isLoaded_ = true;
		glDeleteShader(vs);
		glDeleteShader(fs);

		glUseProgram(program_);
		auto gotUniformsOkay = getUniformLocations();
		glUseProgram(NULL);

		if (!gotUniformsOkay)
		{
			log.error << "Error fetching uniform locations - shader may not work properly!" << util::endl;
		}

		return true;
	}

	bool Shader::activate()
	{
		if (isLoaded_)
		{
			glUseProgram(program_);
			return true;
		}
		else
		{
			log.warn << "Attempting to activate an uninitialized shader" << util::endl;
			return false;
		}
	}

	void Shader::setVertexAttribPointers(util::PipelineState& pso)
	{
		setVertexAttribPointersInternal();

		unsigned int ct = getNumVertexAttribPointers();
		for (int i = pso.BOUND_VERTEX_ATTRIB_PTR; i >= 0; i--)
		{
			glDisableVertexAttribArray(i);
		}
		for (auto i = 0; i < ct; i++)
		{
			glEnableVertexAttribArray(i);
		}
		pso.BOUND_VERTEX_ATTRIB_PTR = ct;
	}
}