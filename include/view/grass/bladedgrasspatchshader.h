#pragma once

#include <glm/glm.hpp>
#include <view/shader.h>
#include <view/texture.h>
#include <memory>
#include <model/light/directionallight.h>
#include <model/geo/plane.h>
#include <view/uniformblockobject.h>

namespace view
{
	namespace grass
	{
		class BladedGrassPatchShader : public view::Shader
		{
		public:
			struct Vertex
			{
				glm::vec3 Pos;
				float BladeBaseWidth;
				float BladeHeight;
				float BladeTaperRate;
				float Rotation;
				float TwistRate;
			};

			BladedGrassPatchShader();
			~BladedGrassPatchShader();
			BladedGrassPatchShader(const BladedGrassPatchShader&) = delete;

			void setWorldMatrix(const glm::mat4& w);
			void setViewMatrix(const glm::mat4& v);
			void setProjMatrix(const glm::mat4& p);
			void setWind(float windStrength);
			void setTimeElapsed(float timeElapsed);
			void setGrassBladeTexture(std::shared_ptr<view::Texture> tex);
			void setLight(const model::light::DirectionalLight& light);
			void setCameraPosition(const glm::vec3& p);
			void setClipPlane(const model::geo::Plane& plane);
			void setBladeSpecularColor(const glm::vec4& c);

			void submitUBOs();

		protected:
			// From view::Shader
			bool getUniformLocations() override;
			void setVertexAttribPointersInternal() override;
			unsigned int getNumVertexAttribPointers() override;
			virtual bool initializeInternal() override;

		private:
			struct UniformLocations
			{
				GLuint PerSceneBlockG;
				GLuint PerFrameBlockG;
				GLuint PerInstanceBlockG;
				GLuint PerSceneBlockF;
				GLuint PerFrameBlockF;
				GLuint texGrassBlade;
			} uniformLocations_;

			struct PerSceneBlockG
			{
				glm::mat4 matProj;
				float windStrength;
			};
			UniformBufferObject<PerSceneBlockG> perSceneBlockG_;

			struct PerFrameBlockG
			{
				glm::mat4 matView;
				glm::vec3 clipPlaneOrigin;
				float fTimeElapsed;
				glm::vec3 clipPlaneNormal;
			};
			UniformBufferObject<PerFrameBlockG> perFrameBlockG_;

			struct PerInstanceBlockG
			{
				glm::mat4 matWorld;
			};
			UniformBufferObject<PerInstanceBlockG> perInstanceBlockG_;

			struct PerSceneBlockF
			{
				glm::vec4 lightAmbient;
				glm::vec4 lightDiffuse;
				glm::vec4 lightSpecular;
				glm::vec3 lightDirection;
				glm::vec4 cBladeSpecular;
			};
			UniformBufferObject<PerSceneBlockF> perSceneBlockF_;

			struct PerFrameBlockF
			{
				glm::vec3 vCameraPos;
			};
			UniformBufferObject<PerFrameBlockF> perFrameBlockF_;
		};
	}
}