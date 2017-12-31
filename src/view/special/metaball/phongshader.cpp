#include <glad/glad.h>

#include <view/special/metaball/phongshader.h>
#include <fstream>
#include <resources.h>
#include <util/io.h>
#include <glm/gtc/type_ptr.hpp>

namespace view
{
	namespace special
	{
		namespace metaball
		{
			PhongShader::PhongShader()
				: isLoaded_(false)
				, program_(0u)
				, log(util::DEBUG, util::WARN, "[pg:PhongMetaball] ")
			{}

			bool PhongShader::initialize()
			{
				if (isLoaded_)
				{
					log.warn << "Attempted to initialize an already loaded shader" << util::endl;
					return false;
				}

				auto vsTextFuture = util::readFileAsync(SHADER_PATH("metaball-phong.vert"));
				auto fsTextFuture = util::readFileAsync(SHADER_PATH("metaball-phong.frag"));

				std::string vsText = vsTextFuture.get();
				const char* ccVSText = vsText.c_str();
				GLuint vs = glCreateShader(GL_VERTEX_SHADER);
				glShaderSource(vs, 1, &ccVSText, NULL);
				glCompileShader(vs);

				GLint success = 0;
				glGetShaderiv(vs, GL_COMPILE_STATUS, &success);
				if (success == GL_FALSE)
				{
					char* errLog = new char[PhongShader::MAX_INFO_LOG_LENGTH];
					glGetShaderInfoLog(vs, PhongShader::MAX_INFO_LOG_LENGTH, nullptr, errLog);
					log.error << "Failed to compile metaball phong shader vertex shader! Error below\n=====BEGIN ERROR LOG=====" << util::endl;
					log.error << errLog;
					log.error << "\n=====END ERROR LOG=====\n";
					delete[] errLog;
					glDeleteShader(vs);
					return false;
				}

				std::string fsText = fsTextFuture.get();
				const char* ccFSText = fsText.c_str();
				GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
				glShaderSource(fs, 1, &ccFSText, NULL);
				glCompileShader(fs);
				glGetShaderiv(fs, GL_COMPILE_STATUS, &success);
				if (success == GL_FALSE)
				{
					char* errLog = new char[PhongShader::MAX_INFO_LOG_LENGTH];
					glGetShaderInfoLog(fs, PhongShader::MAX_INFO_LOG_LENGTH, nullptr, errLog);
					log.error << "Failed to compile metaball phong shader fragment shader! Error below\n=====BEGIN ERROR LOG=====\n";
					log.error << errLog;
					log.error << "\n=====END ERROR LOG=====\n";
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
					char* errLog = new char[PhongShader::MAX_INFO_LOG_LENGTH];
					glGetProgramInfoLog(program_, PhongShader::MAX_INFO_LOG_LENGTH, nullptr, errLog);
					log.error << "Failed to link metaball phong shader! Error below\n=====BEGIN ERROR LOG=====\n";
					log.error << errLog;
					log.error << "\n=====END ERROR LOG=====\n";
					delete errLog;
					glDeleteShader(vs);
					glDeleteShader(fs);
					glDeleteProgram(program_);
					return false;
				}

				glDeleteShader(vs);
				glDeleteShader(fs);

				isLoaded_ = true;

				glUseProgram(program_);
				uniformLocations_.matWorld = glGetUniformLocation(program_, "matWorld");
				uniformLocations_.matView = glGetUniformLocation(program_, "matView");
				uniformLocations_.matProj = glGetUniformLocation(program_, "matProj");

				uniformLocations_.charges = glGetUniformLocation(program_, "charges");
				uniformLocations_.numCharges = glGetUniformLocation(program_, "numCharges");
				uniformLocations_.cameraPos = glGetUniformLocation(program_, "cameraPos");
				uniformLocations_.lightDir = glGetUniformLocation(program_, "lightDir");
				uniformLocations_.ambientColor = glGetUniformLocation(program_, "ambientColor");
				uniformLocations_.diffuseColor = glGetUniformLocation(program_, "diffuseColor");
				uniformLocations_.specularColor = glGetUniformLocation(program_, "specularColor");
				uniformLocations_.mtlDiffuse = glGetUniformLocation(program_, "mtlDiffuse");
				uniformLocations_.mtlSpecular = glGetUniformLocation(program_, "mtlSpecular");
				uniformLocations_.shininess = glGetUniformLocation(program_, "shininess");
				uniformLocations_.stepSize = glGetUniformLocation(program_, "stepSize");

				return true;
			}

			PhongShader::~PhongShader()
			{
				if (isLoaded_)
				{
					glDeleteProgram(program_);
					isLoaded_ = false;
				}
			}

			bool PhongShader::activate()
			{
				if (isLoaded_)
				{
					glUseProgram(program_);
					return true;
				}
				else
				{
					log.warn << "Attempting to activate uninitialized program" << util::endl;
					return false;
				}
			}

			void PhongShader::setWorldMatrix(const glm::mat4& v)
			{
				glUniformMatrix4fv(uniformLocations_.matWorld, 1, GL_FALSE, glm::value_ptr(v));
			}

			void PhongShader::setViewMatrix(const glm::mat4& v)
			{
				glUniformMatrix4fv(uniformLocations_.matView, 1, GL_FALSE, glm::value_ptr(v));
			}

			void PhongShader::setProjMatrix(const glm::mat4& v)
			{
				glUniformMatrix4fv(uniformLocations_.matProj, 1, GL_FALSE, glm::value_ptr(v));
			}

			void PhongShader::setCharges(const std::vector<model::specialgeo::metaball::Charge>& charges)
			{
				glUniform4fv(uniformLocations_.charges, charges.size() * sizeof(model::specialgeo::metaball::Charge), reinterpret_cast<const GLfloat*>(&charges[0]));
				glUniform1i(uniformLocations_.numCharges, charges.size());
			}

			void PhongShader::setCameraPos(const glm::vec3& cp)
			{
				glUniform3fv(uniformLocations_.cameraPos, 1, glm::value_ptr(cp));
			}

			void PhongShader::setLightDir(const glm::vec3& ld)
			{
				glUniform3fv(uniformLocations_.lightDir, 1, glm::value_ptr(ld));
			}

			void PhongShader::setLight(
				const glm::vec3& ambient,
				const glm::vec3& diffuse,
				const glm::vec3& specular
			) {
				glUniform3fv(uniformLocations_.ambientColor, 1, glm::value_ptr(ambient));
				glUniform3fv(uniformLocations_.diffuseColor, 1, glm::value_ptr(diffuse));
				glUniform3fv(uniformLocations_.specularColor, 1, glm::value_ptr(specular));
			}

			void PhongShader::setMaterial(
				const glm::vec3& diffuse,
				const glm::vec3& specular,
				float shininess
			) {
				glUniform3fv(uniformLocations_.mtlDiffuse, 1, glm::value_ptr(diffuse));
				glUniform3fv(uniformLocations_.mtlSpecular, 1, glm::value_ptr(specular));
				glUniform1f(uniformLocations_.shininess, shininess);
			}

			void PhongShader::setStepSize(
				float stepSize
			) {
				glUniform1f(uniformLocations_.stepSize, stepSize);
			}

			void setPhongVertexAttribPointers()
			{
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
				glEnableVertexAttribArray(0);
			}
		}
	}
}