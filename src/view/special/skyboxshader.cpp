#include <view/special/skyboxshader.h>
#include <resources.h>
#include <glm/gtc/type_ptr.hpp>

namespace view
{
	namespace special
	{
		SkyboxShader::SkyboxShader()
			: Shader("[Skybox Shader] ", SHADER_PATH("skybox.vert"), SHADER_PATH("skybox.frag"))
		{}

		void SkyboxShader::setWorldMatrix(const glm::mat4& m)
		{
			glUniformMatrix4fv(
				uniformLocations_.matWorld,
				1, GL_FALSE,
				glm::value_ptr(m)
			);
		}

		void SkyboxShader::setViewMatrix(const glm::mat4& m)
		{
			glUniformMatrix4fv(
				uniformLocations_.matView,
				1, GL_FALSE,
				glm::value_ptr(m)
			);
		}

		void SkyboxShader::setProjMatrix(const glm::mat4& m)
		{
			glUniformMatrix4fv(
				uniformLocations_.matProj,
				1, GL_FALSE,
				glm::value_ptr(m)
			);
		}

		void SkyboxShader::setCubemap(std::shared_ptr<view::Cubemap> tex)
		{
			glActiveTexture(GL_TEXTURE0);
			glUniform1ui(uniformLocations_.texture, 0u);
			glBindTexture(GL_TEXTURE_CUBE_MAP, tex->texture());
		}

		void SkyboxShader::setVertexAttribPointersInternal()
		{
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
		}

		unsigned int SkyboxShader::getNumVertexAttribPointers()
		{
			return 1u;
		}

		bool SkyboxShader::getUniformLocations()
		{
			uniformLocations_.matWorld = glGetUniformLocation(program_, "matWorld");
			uniformLocations_.matView = glGetUniformLocation(program_, "matView");
			uniformLocations_.matProj = glGetUniformLocation(program_, "matProj");
			uniformLocations_.texture = glGetUniformLocation(program_, "skyTexture");

			return true;
		}
	}
}