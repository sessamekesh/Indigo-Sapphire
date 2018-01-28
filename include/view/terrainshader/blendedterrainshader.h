#pragma once

#include <view/shader.h>
#include <glm/glm.hpp>
#include <view/texture.h>
#include <memory>
#include <model/geo/plane.h>
#include <model/light/directionallight.h>

namespace view
{
	namespace terrainshader
	{
		class BlendedTerrainShader : public Shader
		{
		public:
			struct Vertex
			{
				glm::vec3 vPos;
				glm::vec3 vNorm;
				glm::vec2 vUV;
			};

		public:
			BlendedTerrainShader();
			~BlendedTerrainShader() = default;
			BlendedTerrainShader(const BlendedTerrainShader&) = delete;

			void setWorldMatrix(const glm::mat4& m);
			void setViewMatrix(const glm::mat4& v);
			void setProjMatrix(const glm::mat4& p);

			void setBlendMap(std::shared_ptr<Texture> tex);
			void setRedTexture(std::shared_ptr<Texture> tex, float textureScale);
			void setGreenTexture(std::shared_ptr<Texture> tex, float textureScale);
			void setBlueTexture(std::shared_ptr<Texture> tex, float textureScale);

			void setClipPlane(const model::geo::Plane& plane);
			void setLight(const model::light::DirectionalLight& light);

			// Since this shader introduces no additional data to vertices that aren't present in the generic format,
			//  the shader itself can control the conversion process.
			static std::vector<Vertex> convertFromRaw(const std::vector<GenericVertex>& genericVertices);

		protected:
			void setVertexAttribPointersInternal() override;
			unsigned int getNumVertexAttribPointers() override;
			bool getUniformLocations() override;

		protected:
			struct UniformLocationsStruct
			{
				GLuint matWorld;
				GLuint matView;
				GLuint matProj;
				
				GLuint blendMap;
				GLuint redMap;
				GLuint blueMap;
				GLuint greenMap;
				GLuint redUVScale;
				GLuint greenUVScale;
				GLuint blueUVScale;

				GLuint clipPlaneNormal;
				GLuint clipPlaneOrigin;

				GLuint lightAmbient;
				GLuint lightDiffuse;
				GLuint lightDirection;
			} uniformLocations_;
		};
	}
}