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

			void WaterSurfaceShader::setVertexAttribPointersInternal()
			{
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);
			}

			unsigned int WaterSurfaceShader::getNumVertexAttribPointers()
			{
				return 1u;
			}

			void WaterSurfaceShader::setReflectionTexture(std::shared_ptr<view::Texture> tex)
			{
				glActiveTexture(GL_TEXTURE0);
				glUniform1ui(uniformLocations_.reflectionTexture, 0u);
				glBindTexture(GL_TEXTURE_2D, tex->texture());
			}

			void WaterSurfaceShader::setRefractionTexture(std::shared_ptr<view::Texture> tex)
			{
				glActiveTexture(GL_TEXTURE1);
				glUniform1ui(uniformLocations_.refractionTexture, 1u);
				glBindTexture(GL_TEXTURE_2D, tex->texture());
			}

			void WaterSurfaceShader::setReflectionTexture(std::shared_ptr<view::Framebuffer> fbo)
			{
				glUniform1ui(uniformLocations_.reflectionTexture, 0u);
				glBindTexture(GL_TEXTURE_2D, fbo->colorBufferTexture());
				glActiveTexture(GL_TEXTURE0);
			}

			void WaterSurfaceShader::setRefractionTexture(std::shared_ptr<view::Framebuffer> fbo)
			{
				glUniform1ui(uniformLocations_.refractionTexture, 1u);
				glBindTexture(GL_TEXTURE_2D, fbo->colorBufferTexture());
				glActiveTexture(GL_TEXTURE1);
			}

			bool WaterSurfaceShader::getUniformLocations()
			{
				uniformLocations_.matWorld = glGetUniformLocation(program_, "matWorld");
				uniformLocations_.matView = glGetUniformLocation(program_, "matView");
				uniformLocations_.matProj = glGetUniformLocation(program_, "matProj");
				uniformLocations_.reflectionTexture = glGetUniformLocation(program_, "reflectionTexture");
				uniformLocations_.refractionTexture = glGetUniformLocation(program_, "refractionTexture");

				return true;
			}
		}
	}
}