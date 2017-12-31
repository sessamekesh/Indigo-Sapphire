#pragma once

#include <view/shader.h>
#include <glm/glm.hpp>
#include <vector>
#include <view/cubemap.h>
#include <memory>

namespace view
{
	namespace special
	{
		class SkyboxShader : public Shader
		{
		public:
			struct Vertex
			{
				glm::vec3 vPos;
			};

			struct Mesh
			{
				std::vector<Vertex> verts;
				std::vector<std::uint32_t> indices;
				std::shared_ptr<view::Cubemap> cubemap;
			};

		public:
			SkyboxShader();
			~SkyboxShader() = default;
			SkyboxShader(const SkyboxShader&) = delete;

			void setWorldMatrix(const glm::mat4& m);
			void setViewMatrix(const glm::mat4& m);
			void setProjMatrix(const glm::mat4& m);
			void setCubemap(const std::shared_ptr<view::Cubemap> cubemap);

		private:
			virtual void setVertexAttribPointersInternal() override;
			virtual unsigned int getNumVertexAttribPointers() override;
			virtual bool getUniformLocations() override;

		protected:
			struct UniformLocationStruct
			{
				GLuint matWorld;
				GLuint matView;
				GLuint matProj;
				GLuint texture;
			} uniformLocations_;
		};
	}
}