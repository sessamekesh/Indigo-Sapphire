#pragma once

#include <glm/glm.hpp>
#include <view/shader.h>
#include <view/texture.h>
#include <memory>
#include <view/texture.h>
#include <model/geo/plane.h>

namespace view
{
	namespace text
	{
		class MSDFTextShader : public view::Shader
		{
		public:
			struct Vertex
			{
				glm::vec3 Pos;
				float AtlasX;
				float AtlasY;
			};

		public:
			MSDFTextShader();
			~MSDFTextShader() = default;
			MSDFTextShader(const MSDFTextShader&) = delete;

			void setWorldMatrix(const glm::mat4& w);
			void setViewMatrix(const glm::mat4& v);
			void setProjMatrix(const glm::mat4& p);

			void setTextureAtlas(std::shared_ptr<view::Texture> atlas, std::uint32_t numRows, std::uint32_t numCols);
			void setClipPlane(const model::geo::Plane& plane);
			void setTextColor(const glm::vec4& color);

		protected:
			// From view::Shader
			bool getUniformLocations() override;
			void setVertexAttribPointersInternal() override;
			unsigned int getNumVertexAttribPointers() override;

		private:
			struct UniformLocations
			{
				GLuint WorldMatrix;
				GLuint ViewMatrix;
				GLuint ProjMatrix;
				GLuint TextColor;
				GLuint ClipPlaneOrigin;
				GLuint ClipPlaneNormal;
				GLuint TextureAtlas;
				GLuint TextureAtlasRows;
				GLuint TextureAtlasCols;
			} uniformLocations_;
		};
	}
}