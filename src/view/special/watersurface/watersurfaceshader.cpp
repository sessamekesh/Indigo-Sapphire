#include <view/special/watersurface/watersurfaceshader.h>
#include <resources.h>
#include <glm/gtc/type_ptr.hpp>

namespace view
{
	namespace special
	{
		namespace watersurface
		{
			WaterSurfaceShader::WaterSurfaceShader()
				: Shader("[WaterSurfaceShader] ", SHADER_PATH("watersurface.vert"), SHADER_PATH("watersurface.frag"))
				, uniformLocations_({})
			{}

			void WaterSurfaceShader::setWorldMatrix(const glm::mat4& m)
			{
				glUniformMatrix4fv(uniformLocations_.matWorld, 1, GL_FALSE, glm::value_ptr(m));
			}

			void WaterSurfaceShader::setViewMatrix(const glm::mat4& m)
			{
				glUniformMatrix4fv(uniformLocations_.matView, 1, GL_FALSE, glm::value_ptr(m));
			}

			void WaterSurfaceShader::setProjMatrix(const glm::mat4& m)
			{
				glUniformMatrix4fv(uniformLocations_.matProj, 1, GL_FALSE, glm::value_ptr(m));
			}

			void WaterSurfaceShader::setCameraPosition(const glm::vec3& cp)
			{
				glUniform3fv(uniformLocations_.cameraPosition, 1, glm::value_ptr(cp));
			}

			void WaterSurfaceShader::setVertexAttribPointersInternal()
			{
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);
			}

			unsigned int WaterSurfaceShader::getNumVertexAttribPointers()
			{
				return 1u;
			}

			void WaterSurfaceShader::setReflectionTexture(std::shared_ptr<view::Framebuffer> fbo)
			{
				glUniform1i(uniformLocations_.reflectionTexture, 0);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, fbo->colorBufferTexture());
			}

			void WaterSurfaceShader::setRefractionTexture(std::shared_ptr<view::Framebuffer> fbo)
			{
				glUniform1i(uniformLocations_.refractionTexture, 1);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, fbo->colorBufferTexture());
			}

			void WaterSurfaceShader::setDUDVMap(std::shared_ptr<view::Texture> tex)
			{
				glUniform1i(uniformLocations_.dudvMap, 2);
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, tex->texture());
			}

			void WaterSurfaceShader::setNormalMap(std::shared_ptr<view::Texture> tex)
			{
				glUniform1i(uniformLocations_.normalMap, 3);
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, tex->texture());
			}

			void WaterSurfaceShader::setTilingStrength(float s)
			{
				glUniform1f(uniformLocations_.tilingStrength, s);
			}

			void WaterSurfaceShader::setDUDVScaleFactor(float dudvs)
			{
				glUniform1f(uniformLocations_.scaleFactor, dudvs);
			}

			void WaterSurfaceShader::setDUDVSampleOffset(float t)
			{
				glUniform1f(uniformLocations_.dudvSampleOffset, t);
			}

			void WaterSurfaceShader::setLight(const model::light::DirectionalLight& light)
			{
				glUniform3fv(uniformLocations_.lightColor, 1, glm::value_ptr(light.diffuse()));
				glUniform3fv(uniformLocations_.lightDir, 1, glm::value_ptr(light.direction()));
			}

			void WaterSurfaceShader::setWaterSurfaceOrientation(const glm::quat& rot)
			{
				glUniform4fv(uniformLocations_.quatSurfaceOrientation, 1, glm::value_ptr(rot));
			}

			void WaterSurfaceShader::setShineDamper(float s)
			{
				glUniform1f(uniformLocations_.shineDamper, s);
			}

			void WaterSurfaceShader::setReflectivity(float r)
			{
				glUniform1f(uniformLocations_.reflectivity, r);
			}

			bool WaterSurfaceShader::getUniformLocations()
			{
				uniformLocations_.matWorld = glGetUniformLocation(program_, "matWorld");
				uniformLocations_.matView = glGetUniformLocation(program_, "matView");
				uniformLocations_.matProj = glGetUniformLocation(program_, "matProj");
				uniformLocations_.reflectionTexture = glGetUniformLocation(program_, "reflectionTexture");
				uniformLocations_.refractionTexture = glGetUniformLocation(program_, "refractionTexture");
				uniformLocations_.dudvMap = glGetUniformLocation(program_, "dudvMap");
				uniformLocations_.tilingStrength = glGetUniformLocation(program_, "tilingStrength");
				uniformLocations_.scaleFactor = glGetUniformLocation(program_, "scaleFactor");
				uniformLocations_.dudvSampleOffset = glGetUniformLocation(program_, "dudvSampleOffset");
				uniformLocations_.cameraPosition = glGetUniformLocation(program_, "cameraPosition");
				uniformLocations_.normalMap = glGetUniformLocation(program_, "normalMap");
				uniformLocations_.lightColor = glGetUniformLocation(program_, "lightColor");
				uniformLocations_.lightDir = glGetUniformLocation(program_, "lightDir");
				uniformLocations_.quatSurfaceOrientation = glGetUniformLocation(program_, "quatSurfaceOrientation");
				uniformLocations_.shineDamper = glGetUniformLocation(program_, "shineDamper");
				uniformLocations_.reflectivity = glGetUniformLocation(program_, "reflectivity");

				return true;
			}
		}
	}
}