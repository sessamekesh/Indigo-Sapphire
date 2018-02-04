#include <view/grass/GrassShader.h>
#include <util/io.h>
#include <resources.h>
#include <glm/gtc/type_ptr.hpp>

namespace view
{
	namespace grass
	{
		GrassShader::GrassShader()
			: isPrepared_(false)
			, program_(NULL)
			, uniformLocations_({})
			, log(util::DEBUG, util::DEBUG, "[GrassShader ]")
		{}

		GrassShader::~GrassShader()
		{
			if (program_)
			{
				glDeleteProgram(program_);
				program_ = NULL;
				isPrepared_ = false;
			}
		}

		bool GrassShader::initialize()
		{
			if (isPrepared_)
			{
				log.error << "Shader already initialized" << util::endl;
				return false;
			}

			auto vsTextFuture = util::readFileAsync(SHADER_PATH("grass.vert"));
			auto gsTextFuture = util::readFileAsync(SHADER_PATH("grass.geom"));
			auto fsTextFuture = util::readFileAsync(SHADER_PATH("grass.frag"));

			GLuint vs = NULL, gs = NULL, fs = NULL;
			if (!createShader(vsTextFuture.get(), GL_VERTEX_SHADER, vs))
			{
				log.error << "Failed to create vertex shader" << util::endl;
				goto ext;
			}
			if (!createShader(gsTextFuture.get(), GL_GEOMETRY_SHADER, gs))
			{
				log.error << "Failed to create geometry shader" << util::endl;
				goto ext;
			}
			if (!createShader(fsTextFuture.get(), GL_FRAGMENT_SHADER, fs))
			{
				log.error << "Failed to create fragment shader" << util::endl;
				goto ext;
			}

			program_ = glCreateProgram();
			glAttachShader(program_, vs);
			glAttachShader(program_, gs);
			glAttachShader(program_, fs);
			glLinkProgram(program_);
			GLint success = NULL;
			glGetProgramiv(program_, GL_LINK_STATUS, &success);
			if (success == GL_FALSE)
			{
				char* errLog = new char[1024];
				glGetProgramInfoLog(program_, 1024, nullptr, errLog);
				log.error << "Failed to link grass shader program! Error below\n=====BEGIN ERROR LOG=====\n";
				log.error << errLog << "\n=====END ERROR LOG=====\n";
				delete[] errLog;
				goto ext;
			}

			isPrepared_ = true;
			glDeleteShader(vs);
			glDeleteShader(gs);
			glDeleteShader(fs);

			//
			// Get uniform locations
			//
			glUseProgram(program_);
			if (!getUniformLocations())
			{
				log.error << "Failed to get uniform locations" << util::endl;
				goto ext;
			}
			glUseProgram(NULL);

			return true;
		ext:
			if (vs != NULL)
			{
				glDeleteShader(vs);
			}
			if (fs != NULL)
			{
				glDeleteShader(fs);
			}
			if (gs != NULL)
			{
				glDeleteShader(gs);
			}
			if (program_ != NULL)
			{
				glDeleteProgram(program_);
				program_ = NULL;
			}
			return false;
		}

		bool GrassShader::createShader(const std::string& shaderSource, GLuint shaderType, GLuint& out)
		{
			out = glCreateShader(shaderType);
			const char* cc = shaderSource.c_str();
			glShaderSource(out, 1, &cc, NULL);
			glCompileShader(out);
			GLint success;
			glGetShaderiv(out, GL_COMPILE_STATUS, &success);
			if (success == GL_FALSE)
			{
				char* errLog = new char[1024];
				glGetShaderInfoLog(out, 1024, nullptr, errLog);
				log.error << "Failed to compile grass ";
				switch (shaderType)
				{
				case GL_FRAGMENT_SHADER: log.error << "fragment shader"; break;
				case GL_VERTEX_SHADER: log.error << "vertex shader"; break;
				case GL_GEOMETRY_SHADER: log.error << "geometry shader"; break;
				default: log.error << "UNKNOWN_SHADER"; break;
				}
				log.error << "! Error below\n======BEGIN ERROR LOG=====\n";
				log.error << errLog;
				log.error << "\n======END ERROR LOG=====\n";
				delete[] errLog;
				return false;
			}

			return true;
		}

		bool GrassShader::getUniformLocations()
		{
			uniformLocations_.matWorld = glGetUniformLocation(program_, "matWorld");
			uniformLocations_.matView = glGetUniformLocation(program_, "matView");
			uniformLocations_.matProj = glGetUniformLocation(program_, "matProj");
			uniformLocations_.minPatchHeight = glGetUniformLocation(program_, "minPatchHeight");
			uniformLocations_.maxPatchHeight = glGetUniformLocation(program_, "maxPatchHeight");
			uniformLocations_.patchSize = glGetUniformLocation(program_, "patchSize");
			uniformLocations_.windStrength = glGetUniformLocation(program_, "windStrength");
			uniformLocations_.fTimePassed = glGetUniformLocation(program_, "fTimePassed");
			uniformLocations_.gSampler = glGetUniformLocation(program_, "gSampler");
			uniformLocations_.vColor = glGetUniformLocation(program_, "vColor");
			uniformLocations_.vEyePosition = glGetUniformLocation(program_, "vEyePosition");
			uniformLocations_.fAlphaTest = glGetUniformLocation(program_, "fAlphaTest");
			uniformLocations_.fAlphaMultiplier = glGetUniformLocation(program_, "fAlphaMultiplier");
			uniformLocations_.clipPlaneNormal = glGetUniformLocation(program_, "clipPlaneNormal");
			uniformLocations_.clipPlaneOrigin = glGetUniformLocation(program_, "clipPlaneOrigin");
			
			return true;
		}

		bool GrassShader::activate()
		{
			if (!isPrepared_)
			{
				return false;
			}

			glUseProgram(program_);
			return true;
		}

		bool GrassShader::setVertexAttribs(util::PipelineState& pso)
		{
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GrassShader::Vertex), nullptr);
			unsigned int ct = 1;
			for (int i = pso.BOUND_VERTEX_ATTRIB_PTR; i >= 0; i--)
			{
				glDisableVertexAttribArray(i);
			}
			for (auto i = 0; i < ct; i++)
			{
				glEnableVertexAttribArray(i);
			}
			pso.BOUND_VERTEX_ATTRIB_PTR = ct;

			return true;
		}

		void GrassShader::setWorldMatrix(const glm::mat4& w)
		{
			glUniformMatrix4fv(uniformLocations_.matWorld, 1, GL_FALSE, glm::value_ptr(w));
		}

		void GrassShader::setViewMatrix(const glm::mat4& v)
		{
			glUniformMatrix4fv(uniformLocations_.matView, 1, GL_FALSE, glm::value_ptr(v));
		}

		void GrassShader::setProjMatrix(const glm::mat4& p)
		{
			glUniformMatrix4fv(uniformLocations_.matProj, 1, GL_FALSE, glm::value_ptr(p));
		}

		void GrassShader::setPatchSizeData(float minPatchHeight, float maxPatchHeight, float patchSize)
		{
			glUniform1f(uniformLocations_.minPatchHeight, minPatchHeight);
			glUniform1f(uniformLocations_.maxPatchHeight, maxPatchHeight);
			glUniform1f(uniformLocations_.patchSize, patchSize);
		}

		void GrassShader::setWind(float windStrength)
		{
			glUniform1f(uniformLocations_.windStrength, windStrength);
		}

		void GrassShader::setTimePassed(float timePassed)
		{
			glUniform1f(uniformLocations_.fTimePassed, timePassed);
		}

		void GrassShader::setTexture(std::shared_ptr<view::Texture> tex)
		{
			glActiveTexture(GL_TEXTURE0);
			glUniform1i(uniformLocations_.gSampler, 0);
			glBindTexture(GL_TEXTURE_2D, tex->texture());
		}

		void GrassShader::setColor(const glm::vec4& color)
		{
			glUniform4fv(uniformLocations_.vColor, 1, glm::value_ptr(color));
		}
		
		void GrassShader::setEyePos(const glm::vec3& pos)
		{
			glUniform3fv(uniformLocations_.vEyePosition, 1, glm::value_ptr(pos));
		}

		void GrassShader::setAlphaTest(float test, float multiplier)
		{
			glUniform1f(uniformLocations_.fAlphaTest, test);
			glUniform1f(uniformLocations_.fAlphaMultiplier, multiplier);
		}

		void GrassShader::setClipPlane(const model::geo::Plane& plane)
		{
			glUniform3fv(uniformLocations_.clipPlaneNormal, 1, glm::value_ptr(plane.normal()));
			glUniform3fv(uniformLocations_.clipPlaneOrigin, 1, glm::value_ptr(plane.origin()));
		}
	}
}