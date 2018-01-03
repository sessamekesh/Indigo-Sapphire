#pragma once

#include <view/shader.h>
#include <glm/glm.hpp>
#include <memory>
#include <view/texture.h>
#include <view/framebuffer.h>

namespace view
{
	namespace special
	{
		namespace watersurface
		{
			struct Vertex
			{
				glm::vec3 vPos;
			};

			class WaterSurfaceShader : public Shader
			{
			public:
				WaterSurfaceShader();
				~WaterSurfaceShader() = default;
				WaterSurfaceShader(const WaterSurfaceShader&) = default;

				void setWorldMatrix(const glm::mat4& m);
				void setViewMatrix(const glm::mat4& m);
				void setProjMatrix(const glm::mat4& m);

				void setReflectionTexture(std::shared_ptr<view::Texture> tex);
				void setRefractionTexture(std::shared_ptr<view::Texture> tex);

				void setReflectionTexture(std::shared_ptr<view::Framebuffer> fbo);
				void setRefractionTexture(std::shared_ptr<view::Framebuffer> fbo);

			private:
				virtual void setVertexAttribPointersInternal() override;
				virtual unsigned int getNumVertexAttribPointers() override;
				virtual bool getUniformLocations() override;

			private:
				struct UniformLocations
				{
					GLuint reflectionTexture;
					GLuint refractionTexture;
					GLuint matWorld;
					GLuint matView;
					GLuint matProj;
				} uniformLocations_;
			};
		}
	}
}