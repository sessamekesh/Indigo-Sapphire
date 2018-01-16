#include <sim/lakescene/shaders/tree.h>
#include <resources.h>
#include <glm/gtc/type_ptr.hpp>

namespace sim
{
	namespace lake
	{
		TreeShader::TreeShader()
			: view::Shader("[TreeShader ]", SHADER_PATH("tree.vert"), SHADER_PATH("tree.frag"))
			, uniformLocations_({})
		{}

		void TreeShader::setWorldMatrix(const glm::mat4& m)
		{
			glUniformMatrix4fv(uniformLocations_.matWorld, 1, GL_FALSE, glm::value_ptr(m));
		}

		void TreeShader::setViewMatrix(const glm::mat4& m)
		{
			glUniformMatrix4fv(uniformLocations_.matView, 1, GL_FALSE, glm::value_ptr(m));
		}

		void TreeShader::setProjMatrix(const glm::mat4& m)
		{
			glUniformMatrix4fv(uniformLocations_.matProj, 1, GL_FALSE, glm::value_ptr(m));
		}

		void TreeShader::setClipPlane(const model::geo::Plane& p)
		{
			glUniform3fv(uniformLocations_.clipPlaneNormal, 1, glm::value_ptr(p.normal()));
			glUniform3fv(uniformLocations_.clipPlaneOrigin, 1, glm::value_ptr(p.origin()));
		}

		void TreeShader::setDiffuseMap(std::shared_ptr<view::Texture> tex)
		{
			glUniform1i(uniformLocations_.diffuseMap, 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex->texture());
		}

		void TreeShader::setLight(const model::light::DirectionalLight& light)
		{
			glUniform3fv(uniformLocations_.lightAmbient, 1, glm::value_ptr(light.ambient()));
			glUniform3fv(uniformLocations_.lightDiffuse, 1, glm::value_ptr(light.diffuse()));
			glUniform3fv(uniformLocations_.lightDirection, 1, glm::value_ptr(light.direction()));
		}

		void TreeShader::setVertexAttribPointersInternal()
		{
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
		}

		unsigned int TreeShader::getNumVertexAttribPointers()
		{
			return 3u;
		}

		bool TreeShader::getUniformLocations()
		{
			uniformLocations_.matWorld = glGetUniformLocation(program_, "matWorld");
			uniformLocations_.matView = glGetUniformLocation(program_, "matView");
			uniformLocations_.matProj = glGetUniformLocation(program_, "matProj");
			uniformLocations_.clipPlaneOrigin = glGetUniformLocation(program_, "clipPlaneOrigin");
			uniformLocations_.clipPlaneNormal = glGetUniformLocation(program_, "clipPlaneNormal");
			uniformLocations_.diffuseMap = glGetUniformLocation(program_, "diffuseMap");
			uniformLocations_.lightAmbient = glGetUniformLocation(program_, "lightAmbient");
			uniformLocations_.lightDiffuse = glGetUniformLocation(program_, "lightDiffuse");
			uniformLocations_.lightDirection = glGetUniformLocation(program_, "lightDirection");

			return true;
		}
	}
}