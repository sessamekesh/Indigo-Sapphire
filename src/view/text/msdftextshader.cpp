#include <view/text/msdftextshader.h>
#include <resources.h>
#include <glm/gtc/type_ptr.hpp>

namespace view
{
	namespace text
	{
		MSDFTextShader::MSDFTextShader()
			: view::Shader("[MSDFTextShader] ", SHADER_PATH("msdftext.vert"), SHADER_PATH("msdftext.frag"))
			, uniformLocations_({})
		{}

		void MSDFTextShader::setWorldMatrix(const glm::mat4& w)
		{
			glUniformMatrix4fv(uniformLocations_.WorldMatrix, 1, GL_FALSE, glm::value_ptr(w));
		}

		void MSDFTextShader::setViewMatrix(const glm::mat4& v)
		{
			glUniformMatrix4fv(uniformLocations_.ViewMatrix, 1, GL_FALSE, glm::value_ptr(v));
		}

		void MSDFTextShader::setProjMatrix(const glm::mat4& p)
		{
			glUniformMatrix4fv(uniformLocations_.ProjMatrix, 1, GL_FALSE, glm::value_ptr(p));
		}

		void MSDFTextShader::setTextureAtlas(std::shared_ptr<view::Texture> atlas, std::uint32_t numRows, std::uint32_t numCols)
		{
			glUniform1i(uniformLocations_.TextureAtlas, 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, atlas->texture());
			glUniform1ui(uniformLocations_.TextureAtlasRows, numRows);
			glUniform1ui(uniformLocations_.TextureAtlasCols, numCols);
		}

		void MSDFTextShader::setClipPlane(const model::geo::Plane& plane)
		{
			glUniform3fv(uniformLocations_.ClipPlaneOrigin, 1, glm::value_ptr(plane.origin()));
			glUniform3fv(uniformLocations_.ClipPlaneNormal, 1, glm::value_ptr(plane.normal()));
		}

		void MSDFTextShader::setTextColor(const glm::vec4& color)
		{
			glUniform4fv(uniformLocations_.TextColor, 1, glm::value_ptr(color));
		}

		bool MSDFTextShader::getUniformLocations()
		{
			uniformLocations_.WorldMatrix = glGetUniformLocation(program_, "matWorld");
			uniformLocations_.ViewMatrix = glGetUniformLocation(program_, "matView");
			uniformLocations_.ProjMatrix = glGetUniformLocation(program_, "matProj");
			uniformLocations_.TextColor = glGetUniformLocation(program_, "textColor");
			uniformLocations_.ClipPlaneNormal = glGetUniformLocation(program_, "clipPlaneNormal");
			uniformLocations_.ClipPlaneOrigin = glGetUniformLocation(program_, "clipPlaneOrigin");
			uniformLocations_.TextureAtlas = glGetUniformLocation(program_, "textureAtlas");
			uniformLocations_.TextureAtlasRows = glGetUniformLocation(program_, "textureAtlasRows");
			uniformLocations_.TextureAtlasCols = glGetUniformLocation(program_, "textureAtlasCols");

			return true;
		}

		void MSDFTextShader::setVertexAttribPointersInternal()
		{
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
			glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
			glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(4 * sizeof(float)));
		}

		unsigned int MSDFTextShader::getNumVertexAttribPointers()
		{
			return 3u;
		}
	}
}