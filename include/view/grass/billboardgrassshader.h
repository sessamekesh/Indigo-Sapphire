#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <util/Logger.h>
#include <util/pipelinestate.h>
#include <view/texture.h>
#include <memory>
#include <model/geo/plane.h>
#include <view/shader.h>

namespace view
{
	namespace grass
	{
		class BillboardGrassShader : public view::Shader
		{
		public:
			struct Vertex
			{
				glm::vec3 Pos;
			};

			BillboardGrassShader();
			~BillboardGrassShader() = default;
			BillboardGrassShader(const BillboardGrassShader&) = delete;

			void setWorldMatrix(const glm::mat4& w);
			void setViewMatrix(const glm::mat4& v);
			void setProjMatrix(const glm::mat4& p);
			void setPatchSizeData(float minPatchHeight, float maxPatchHeight, float patchSize);
			void setWind(float windStrength);
			void setTimePassed(float timePassed);
			void setTexture(std::shared_ptr<view::Texture> tex);
			void setColor(const glm::vec4& color);
			void setEyePos(const glm::vec3& pos);
			void setAlphaTest(float test, float multiplier);
			
			void setClipPlane(const model::geo::Plane& plane);

		protected:
			bool getUniformLocations() override;
			void setVertexAttribPointersInternal() override;
			unsigned int getNumVertexAttribPointers() override;

		private:
			struct UniformLocations
			{
				GLuint matWorld;
				GLuint matView;
				GLuint matProj;
				GLuint minPatchHeight;
				GLuint maxPatchHeight;
				GLuint patchSize;
				GLuint windStrength;
				GLuint fTimePassed;
				GLuint gSampler;
				GLuint vColor;
				GLuint vEyePosition;
				GLuint fAlphaTest;
				GLuint fAlphaMultiplier;
				GLuint clipPlaneOrigin;
				GLuint clipPlaneNormal;
			} uniformLocations_;
		};
	}
}