#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <util/Logger.h>
#include <util/pipelinestate.h>
#include <view/texture.h>
#include <memory>
#include <model/geo/plane.h>

namespace view
{
	namespace grass
	{
		class GrassShader
		{
		public:
			struct Vertex
			{
				glm::vec3 Pos;
			};

			GrassShader();
			~GrassShader();
			GrassShader(const GrassShader&) = delete;

			bool initialize();
			bool activate();

			bool setVertexAttribs(util::PipelineState& pso);

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

		private:
			bool createShader(const std::string& shaderSource, GLuint shaderType, GLuint& out);
			bool getUniformLocations();

		private:
			bool isPrepared_;

			GLuint program_;

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

			util::Logger log;
		};
	}
}