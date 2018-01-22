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
			: Shader("[SolidShader] ", SHADER_PATH("solidshader.vert"), SHADER_PATH("solidshader.frag"))
		{}
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

		std::vector<SolidShaderVertex> SolidShader::translateVertices(const std::vector<view::GenericVertex>& verts, const glm::vec4& color)
		{
			std::vector<SolidShaderVertex> tr;
			tr.reserve(verts.size());

			for (auto&& v : verts)
			{
				tr.push_back({
					v.position, color
				});
			}

			return tr;
		}

		void SolidShader::setVertexAttribPointersInternal()
		{
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), nullptr);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
		}

		unsigned int SolidShader::getNumVertexAttribPointers()
		{
			return 2u;
		}

		void SolidShader::setClipPlane(const model::geo::Plane& plane)
		{
			glUniform3fv(UniformLocations.clipPlaneNormal, 1, glm::value_ptr(plane.normal()));
			glUniform3fv(UniformLocations.clipPlaneOrigin, 1, glm::value_ptr(plane.origin()));
		}

		bool SolidShader::getUniformLocations()
		{
			UniformLocations.matWorld = glGetUniformLocation(program_, "matWorld");
			UniformLocations.matView = glGetUniformLocation(program_, "matView");
			UniformLocations.matProj = glGetUniformLocation(program_, "matProj");
			UniformLocations.clipPlaneOrigin = glGetUniformLocation(program_, "clipPlaneOrigin");
			UniformLocations.clipPlaneNormal = glGetUniformLocation(program_, "clipPlaneNormal");
			return true;
		}
	}
}