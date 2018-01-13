#include <view/mappedphong/mappedphongshader.h>
#include <glm/gtc/type_ptr.hpp>
#include <resources.h>

namespace view
{
	namespace mappedphong
	{
		MappedPhongShader::MappedPhongShader()
			: Shader("[MappedPhong Shader] ", SHADER_PATH("mappedphong.vert"), SHADER_PATH("mappedphong.frag"))
			, uniformLocation_({})
		{}

		void MappedPhongShader::setWorldMatrix(const glm::mat4& m)
		{
			glUniformMatrix4fv(uniformLocation_.matWorld, 1, GL_FALSE, glm::value_ptr(m));
		}

		void MappedPhongShader::setViewMatrix(const glm::mat4& v)
		{
			glUniformMatrix4fv(uniformLocation_.matView, 1, GL_FALSE, glm::value_ptr(v));
		}

		void MappedPhongShader::setProjMatrix(const glm::mat4& p)
		{
			glUniformMatrix4fv(uniformLocation_.matProj, 1, GL_FALSE, glm::value_ptr(p));
		}

		void MappedPhongShader::setClipPlane(const model::geo::Plane& plane)
		{
			glUniform3fv(uniformLocation_.clipPlaneNormal, 1, glm::value_ptr(plane.normal()));
			glUniform3fv(uniformLocation_.clipPlaneOrigin, 1, glm::value_ptr(plane.origin()));
		}

		void MappedPhongShader::setNormalMap(std::shared_ptr<Texture> tex)
		{
			glUniform1i(uniformLocation_.normalMap, 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex->texture());
		}

		void MappedPhongShader::setDiffuseMap(std::shared_ptr<Texture> tex)
		{
			glUniform1i(uniformLocation_.diffuseMap, 1);
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, tex->texture());
		}

		void MappedPhongShader::setSpecularMap(std::shared_ptr<Texture> tex)
		{
			glUniform1i(uniformLocation_.specularMap, 2);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, tex->texture());
		}

		void MappedPhongShader::setLight(const model::light::DirectionalLight& light)
		{
			glUniform3fv(uniformLocation_.lightAmbient, 1, glm::value_ptr(light.ambient()));
			glUniform3fv(uniformLocation_.lightDiffuse, 1, glm::value_ptr(light.diffuse()));
			glUniform3fv(uniformLocation_.lightSpecular, 1, glm::value_ptr(light.specular()));
			glUniform3fv(uniformLocation_.lightDirection, 1, glm::value_ptr(light.direction()));
		}

		void MappedPhongShader::setCameraPosition(const glm::vec3& cameraPos)
		{
			glUniform3fv(uniformLocation_.cameraPosition, 1, glm::value_ptr(cameraPos));
		}

		void MappedPhongShader::setShininess(float shininess)
		{
			glUniform1f(uniformLocation_.shininess, shininess);
		}

		void MappedPhongShader::setVertexAttribPointersInternal()
		{
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MappedPhongShader::Vertex), nullptr);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MappedPhongShader::Vertex), (void*)(3 * sizeof(float)));
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(MappedPhongShader::Vertex), (void*)(6 * sizeof(float)));
			glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(MappedPhongShader::Vertex), (void*)(9 * sizeof(float)));
		}

		unsigned int MappedPhongShader::getNumVertexAttribPointers()
		{
			return 4;
		}

		bool MappedPhongShader::getUniformLocations()
		{
			uniformLocation_.matWorld = glGetUniformLocation(program_, "matWorld");
			uniformLocation_.matView = glGetUniformLocation(program_, "matView");
			uniformLocation_.matProj = glGetUniformLocation(program_, "matProj");
			uniformLocation_.clipPlaneOrigin = glGetUniformLocation(program_, "clipPlaneOrigin");
			uniformLocation_.clipPlaneNormal = glGetUniformLocation(program_, "clipPlaneNormal");

			uniformLocation_.normalMap = glGetUniformLocation(program_, "normalMap");
			uniformLocation_.diffuseMap = glGetUniformLocation(program_, "diffuseMap");
			uniformLocation_.specularMap = glGetUniformLocation(program_, "specularMap");

			uniformLocation_.shininess = glGetUniformLocation(program_, "shininess");
			uniformLocation_.lightAmbient = glGetUniformLocation(program_, "lightAmbient");
			uniformLocation_.lightDiffuse = glGetUniformLocation(program_, "lightDiffuse");
			uniformLocation_.lightSpecular = glGetUniformLocation(program_, "lightSpecular");
			uniformLocation_.lightDirection = glGetUniformLocation(program_, "lightDirection");
			uniformLocation_.cameraPosition = glGetUniformLocation(program_, "cameraPosition");

			return true;
		}
	}
}