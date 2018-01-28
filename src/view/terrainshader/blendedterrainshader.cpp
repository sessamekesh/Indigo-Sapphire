#include <view/terrainshader/blendedterrainshader.h>
#include <resources.h>
#include <glm/gtc/type_ptr.hpp>

namespace view
{
	namespace terrainshader
	{
		BlendedTerrainShader::BlendedTerrainShader()
			: Shader("[Blended Terrain Shader] ", SHADER_PATH("blendedterrain.vert"), SHADER_PATH("blendedterrain.frag"))
			, uniformLocations_({})
		{}

		void BlendedTerrainShader::setWorldMatrix(const glm::mat4& m)
		{
			glUniformMatrix4fv(uniformLocations_.matWorld, 1, GL_FALSE, glm::value_ptr(m));
		}

		void BlendedTerrainShader::setViewMatrix(const glm::mat4& v)
		{
			glUniformMatrix4fv(uniformLocations_.matView, 1, GL_FALSE, glm::value_ptr(v));
		}

		void BlendedTerrainShader::setProjMatrix(const glm::mat4& p)
		{
			glUniformMatrix4fv(uniformLocations_.matProj, 1, GL_FALSE, glm::value_ptr(p));
		}

		void BlendedTerrainShader::setBlendMap(std::shared_ptr<Texture> tex)
		{
			glUniform1i(uniformLocations_.blendMap, 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex->texture());
		}

		void BlendedTerrainShader::setRedTexture(std::shared_ptr<Texture> tex, float textureScale)
		{
			glUniform1i(uniformLocations_.redMap, 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, tex->texture());
			glUniform1f(uniformLocations_.redUVScale, textureScale);
		}

		void BlendedTerrainShader::setGreenTexture(std::shared_ptr<Texture> tex, float textureScale)
		{
			glUniform1i(uniformLocations_.greenMap, 2);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, tex->texture());
			glUniform1f(uniformLocations_.greenUVScale, textureScale);
		}

		void BlendedTerrainShader::setBlueTexture(std::shared_ptr<Texture> tex, float textureScale)
		{
			glUniform1i(uniformLocations_.blueMap, 3);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, tex->texture());
			glUniform1f(uniformLocations_.blueUVScale, textureScale);
		}

		void BlendedTerrainShader::setClipPlane(const model::geo::Plane& plane)
		{
			glUniform3fv(uniformLocations_.clipPlaneNormal, 1, glm::value_ptr(plane.normal()));
			glUniform3fv(uniformLocations_.clipPlaneOrigin, 1, glm::value_ptr(plane.origin()));
		}

		void BlendedTerrainShader::setLight(const model::light::DirectionalLight& light)
		{
			glUniform3fv(uniformLocations_.lightAmbient, 1, glm::value_ptr(light.ambient()));
			glUniform3fv(uniformLocations_.lightDiffuse, 1, glm::value_ptr(light.diffuse()));
			glUniform3fv(uniformLocations_.lightDirection, 1, glm::value_ptr(light.direction()));
		}

		std::vector<BlendedTerrainShader::Vertex> BlendedTerrainShader::convertFromRaw(const std::vector<GenericVertex>& genericVertices)
		{
			std::vector<BlendedTerrainShader::Vertex> tr;
			tr.reserve(genericVertices.size());

			for (auto&& v : genericVertices)
			{
				tr.push_back({ v.position, v.normal, v.uv });
			}

			return tr;
		}

		void BlendedTerrainShader::setVertexAttribPointersInternal()
		{
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
		}

		unsigned int BlendedTerrainShader::getNumVertexAttribPointers()
		{
			return 3u;
		}

		bool BlendedTerrainShader::getUniformLocations()
		{
			uniformLocations_.matWorld = glGetUniformLocation(program_, "matWorld");
			uniformLocations_.matView = glGetUniformLocation(program_, "matView");
			uniformLocations_.matProj = glGetUniformLocation(program_, "matProj");
			uniformLocations_.blendMap = glGetUniformLocation(program_, "blendMap");
			uniformLocations_.redMap = glGetUniformLocation(program_, "redMap");
			uniformLocations_.blueMap = glGetUniformLocation(program_, "blueMap");
			uniformLocations_.greenMap = glGetUniformLocation(program_, "greenMap");
			uniformLocations_.redUVScale = glGetUniformLocation(program_, "redUVScale");
			uniformLocations_.greenUVScale = glGetUniformLocation(program_, "greenUVScale");
			uniformLocations_.blueUVScale = glGetUniformLocation(program_, "blueUVScale");
			uniformLocations_.clipPlaneNormal = glGetUniformLocation(program_, "clipPlaneNormal");
			uniformLocations_.clipPlaneOrigin = glGetUniformLocation(program_, "clipPlaneOrigin");
			uniformLocations_.lightAmbient = glGetUniformLocation(program_, "lightAmbient");
			uniformLocations_.lightDiffuse = glGetUniformLocation(program_, "lightDiffuse");
			uniformLocations_.lightDirection = glGetUniformLocation(program_, "lightDirection");

			return true;
		}
	}
}