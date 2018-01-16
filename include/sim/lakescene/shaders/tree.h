#pragma once

#include <view/shader.h>
#include <model/light/directionallight.h>
#include <model/geo/plane.h>
#include <view/texture.h>
#include <memory>

namespace sim
{
	namespace lake
	{
		class TreeShader : public view::Shader
		{
		public:
			struct Vertex
			{
				glm::vec3 vPos;
				glm::vec3 vNorm;
				glm::vec2 vUV;
			};

		public:
			TreeShader();
			~TreeShader() = default;
			TreeShader(const TreeShader&) = delete;

			void setWorldMatrix(const glm::mat4& m);
			void setViewMatrix(const glm::mat4& m);
			void setProjMatrix(const glm::mat4& m);

			void setClipPlane(const model::geo::Plane& plane);
			void setDiffuseMap(std::shared_ptr<view::Texture> tex);
			void setLight(const model::light::DirectionalLight& light);

			//
			// Shader overrides
			//
		private:
			virtual void setVertexAttribPointersInternal() override;
			virtual unsigned int getNumVertexAttribPointers() override;
			virtual bool getUniformLocations() override;

		protected:
			struct UniformLocations
			{
				GLuint matWorld;
				GLuint matView;
				GLuint matProj;
				GLuint clipPlaneOrigin;
				GLuint clipPlaneNormal;

				GLuint diffuseMap;

				GLuint lightAmbient;
				GLuint lightDiffuse;
				GLuint lightDirection;
			} uniformLocations_;
		};
	}
}