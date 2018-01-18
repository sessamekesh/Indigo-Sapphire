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

		void SkyboxShader::setClipPlane(const glm::vec3& origin, const glm::vec3& normal)
		{
			glUniform3fv(uniformLocations_.clipPlaneNormal, 1, glm::value_ptr(normal));
			glUniform3fv(uniformLocations_.clipPlaneOrigin, 1, glm::value_ptr(origin));
		}

		bool SkyboxShader::getUniformLocations()
		{
			uniformLocations_.clipPlaneNormal = glGetUniformLocation(program_, "clipPlaneNormal");
			uniformLocations_.clipPlaneOrigin = glGetUniformLocation(program_, "clipPlaneOrigin");
			uniformLocations_.matWorld = glGetUniformLocation(program_, "matWorld");
			uniformLocations_.matView = glGetUniformLocation(program_, "matView");
			uniformLocations_.matProj = glGetUniformLocation(program_, "matProj");
			uniformLocations_.texture = glGetUniformLocation(program_, "skyTexture");

			return true;
		}

		std::vector<std::uint8_t> SkyboxShader::getVertexBuffer(const std::vector<view::GenericVertex>& genericVertices)
		{
			std::vector<std::uint8_t> tr;

			std::vector<SkyboxShader::Vertex> parsedVerts;
			parsedVerts.reserve(genericVertices.size());

			for (auto&& gv : genericVertices)
			{
				parsedVerts.push_back({ gv.position });
			}

			std::size_t byteSize = genericVertices.size() * sizeof(SkyboxShader::Vertex);
			tr.resize(byteSize);
			memcpy_s(&tr[0], byteSize, &parsedVerts[0], byteSize);

			return tr;
		}
	}
}