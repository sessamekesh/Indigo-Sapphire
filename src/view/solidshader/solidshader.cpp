#include <glad/glad.h>

#include <view/solidshader/solidshader.h>
#include <resources.h>
#include <fstream>
#include <util/io.h>
#include <glm/gtc/type_ptr.hpp>

namespace view
{
	namespace solidshader
	{
		SolidShader::SolidShader()
			: _isLoaded(false)
			, _program(0u)
			, log(util::DEBUG, util::WARN, "[pg:SolidShader] ")
		{}

		bool SolidShader::initialize()
		{
			if (_isLoaded)
			{
				log.warn << "Attempted to initialize an already loaded solid shader" << util::endl;
				return false;
			}

			std::future<std::string> vsTextFuture = util::readFileAsync(SHADER_PATH("solidshader.vert"));
			std::future<std::string> fsTextFuture = util::readFileAsync(SHADER_PATH("solidshader.frag"));

			std::string vsText = vsTextFuture.get();
			if (vsText.length() == 0)
			{
				log.error << "Could not read vertex shader text!" << util::endl;
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
				char* errLog = new char[SolidShader::MAX_INFO_LOG_LENGTH];
				glGetShaderInfoLog(vs, SolidShader::MAX_INFO_LOG_LENGTH, nullptr, errLog);
				log.error << "Failed to compile solid shader vertex shader! Error below\n=====BEGIN ERROR LOG=====\n";
				log.error << errLog;
				log.error << "\n======END ERROR LOG=====" << util::endl;
				delete[] errLog;

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
				char* errLog = new char[SolidShader::MAX_INFO_LOG_LENGTH];
				glGetShaderInfoLog(fs, SolidShader::MAX_INFO_LOG_LENGTH, nullptr, errLog);
				log.error << "Failed to compile solid shader fragment shader! Error below\n=====BEGIN ERROR LOG=====\n";
				log.error << errLog;
				log.error << "\n======END ERROR LOG=====" << util::endl;
				delete[] errLog;

				return false;
			}

			_program = glCreateProgram();
			glAttachShader(_program, vs);
			glAttachShader(_program, fs);
			glLinkProgram(_program);
			glGetProgramiv(_program, GL_LINK_STATUS, &success);
			if (success == GL_FALSE)
			{
				char* errLog = new char[SolidShader::MAX_INFO_LOG_LENGTH];
				glGetProgramInfoLog(_program, SolidShader::MAX_INFO_LOG_LENGTH, nullptr, errLog);
				log.error << "Failed to link solid shader program! Error below\n=====BEGIN ERROR LOG=====\n";
				log.error << errLog;
				log.error << "\n=====END ERROR LOG=====\n";
				delete[] errLog;

				glDeleteProgram(_program);
				return false;
			}

			_isLoaded = true;
			glDeleteShader(vs);
			glDeleteShader(fs);

			glUseProgram(_program);
			UniformLocations.matWorld = glGetUniformLocation(_program, "matWorld");
			UniformLocations.matView = glGetUniformLocation(_program, "matView");
			UniformLocations.matProj= glGetUniformLocation(_program, "matProj");

			return true;
		}

		SolidShader::~SolidShader()
		{
			if (_isLoaded)
			{
				glDeleteProgram(_program);
				_isLoaded = false;
			}
		}

		bool SolidShader::activate()
		{
			if (_isLoaded)
			{
				glUseProgram(_program);
			}
			return true;
		}

		void SolidShader::setWorldMatrix(const glm::mat4& v)
		{
			glUniformMatrix4fv(UniformLocations.matWorld, 1, GL_FALSE, glm::value_ptr(v));
		}

		void SolidShader::setViewMatrix(const glm::mat4& v)
		{
			glUniformMatrix4fv(UniformLocations.matView, 1, GL_FALSE, glm::value_ptr(v));
		}

		void SolidShader::setProjMatrix(const glm::mat4& v)
		{
			glUniformMatrix4fv(UniformLocations.matProj, 1, GL_FALSE, glm::value_ptr(v));
		}
		void setVertexAttribPointers()
		{
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
			//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(0);
			//glEnableVertexAttribArray(1);
		}
	}
}