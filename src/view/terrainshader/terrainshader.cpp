#include <view/terrainshader/terrainshader.h>
#include <resources.h>
#include <glm/gtc/type_ptr.hpp>

namespace view
{
	namespace terrainshader
	{
		TerrainShader::TerrainShader()
			: Shader("[Terrain Shader] "
			, SHADER_PATH("terrain.vert")
			, SHADER_PATH("terrain.frag"))
		{}

		void TerrainShader::setWorldMatrix(const glm::mat4& m)
		{
			glUniformMatrix4fv(
				uniformLocations_.matWorld,
				1, GL_FALSE,
				glm::value_ptr(m)
			);
		}

		void TerrainShader::setViewMatrix(const glm::mat4& m)
		{
			glUniformMatrix4fv(
				uniformLocations_.matView,
				1, GL_FALSE,
				glm::value_ptr(m)
			);
		}

		void TerrainShader::setProjMatrix(const glm::mat4& m)
		{
			glUniformMatrix4fv(
				uniformLocations_.matProj,
				1, GL_FALSE,
				glm::value_ptr(m)
			);
		}

		void TerrainShader::setTexture(const std::shared_ptr<Texture>& tex)
		{
			glUniform1ui(uniformLocations_.texture, 0u);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex->texture());
		}

		void TerrainShader::setLight(const model::light::DirectionalLight& light)
		{
			glUniform3fv(uniformLocations_.lightAmbientColor, 1, glm::value_ptr(light.ambient()));
			glUniform3fv(uniformLocations_.lightDiffuseColor, 1, glm::value_ptr(light.diffuse()));
			glUniform3fv(uniformLocations_.lightDir, 1, glm::value_ptr(light.direction()));
		}

		void TerrainShader::setVertexAttribPointersInternal()
		{
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		}

		unsigned int TerrainShader::getNumVertexAttribPointers()
		{
			return 3u;
		}

		void TerrainShader::setClipPlane(const model::geo::Plane& plane)
		{
			glUniform3fv(uniformLocations_.clipPlaneNormal, 1, glm::value_ptr(plane.normal()));
			glUniform3fv(uniformLocations_.clipPlaneOrigin, 1, glm::value_ptr(plane.origin()));
		}

		bool TerrainShader::getUniformLocations()
		{
			uniformLocations_.clipPlaneNormal = glGetUniformLocation(program_, "clipPlaneNormal");
			uniformLocations_.clipPlaneOrigin = glGetUniformLocation(program_, "clipPlaneOrigin");
			uniformLocations_.matWorld = glGetUniformLocation(program_, "matWorld");
			uniformLocations_.matView = glGetUniformLocation(program_, "matView");
			uniformLocations_.matProj = glGetUniformLocation(program_, "matProj");
			uniformLocations_.texture = glGetUniformLocation(program_, "texture");
			uniformLocations_.lightDiffuseColor = glGetUniformLocation(program_, "lightDiffuseColor");
			uniformLocations_.lightAmbientColor = glGetUniformLocation(program_, "lightAmbientColor");
			uniformLocations_.lightDir = glGetUniformLocation(program_, "lightDir");

			return true;
		}
	}
}