#pragma once

#include <glm/glm.hpp>
#include <view/shader.h>
#include <util/Logger.h>
#include <util/pipelinestate.h>
#include <vector>
#include <memory>
#include <view/texture.h>
#include <model/light/directionallight.h>

namespace view
{
	namespace terrainshader
	{
		struct Vertex
		{
			glm::vec3 vPos;
			glm::vec3 vNorm;
			glm::vec2 vUV;
		};

		struct Mesh
		{
			std::vector<Vertex> verts;
			std::vector<std::uint32_t> indices;
			std::shared_ptr<Texture> texture;
		};

		class TerrainShader : public Shader
		{
		public:
			TerrainShader();
			~TerrainShader() = default;
			TerrainShader(const TerrainShader&) = delete;

			void setWorldMatrix(const glm::mat4& m);
			void setViewMatrix(const glm::mat4& m);
			void setProjMatrix(const glm::mat4& m);
			void setTexture(const std::shared_ptr<Texture>& tex);
			void setLight(const model::light::DirectionalLight& light);

			void setClipPlane(const glm::vec3& origin, const glm::vec3& normal);

			virtual std::vector<std::uint8_t> getVertexBuffer(const std::vector<view::GenericVertex>& genericVertices) override;

		private:
			virtual void setVertexAttribPointersInternal() override;
			virtual unsigned int getNumVertexAttribPointers() override;
			virtual bool getUniformLocations() override;

		protected:
			struct UniformLocationsStruct
			{
				GLuint clipPlaneOrigin;
				GLuint clipPlaneNormal;
				GLuint matWorld;
				GLuint matView;
				GLuint matProj;
				GLuint texture;
				GLuint lightDiffuseColor;
				GLuint lightAmbientColor;
				GLuint lightDir;
			} uniformLocations_;
		};
		// TODO SESS: Continue with making terrain shader, and then
		//  terrainobject (which loads from Assimp file)
	}
}