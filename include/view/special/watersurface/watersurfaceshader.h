#pragma once

#include <view/shader.h>
#include <glm/glm.hpp>
#include <memory>
#include <view/texture.h>
#include <view/framebuffer.h>
#include <model/light/directionallight.h>

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

				void setCameraPosition(const glm::vec3& v);

				void setReflectionTexture(std::shared_ptr<view::Framebuffer> fbo);
				void setRefractionTexture(std::shared_ptr<view::Framebuffer> fbo);

				void setDUDVMap(std::shared_ptr<view::Texture> tex);
				void setNormalMap(std::shared_ptr<view::Texture> tex);

				void setLight(const model::light::DirectionalLight& light);
				void setWaterSurfaceOrientation(const glm::quat& rot);

				void setTilingStrength(float s);
				void setDUDVScaleFactor(float dudvs);
				void setDUDVSampleOffset(float t);

				void setShineDamper(float s);
				void setReflectivity(float v);

			private:
				virtual void setVertexAttribPointersInternal() override;
				virtual unsigned int getNumVertexAttribPointers() override;
				virtual bool getUniformLocations() override;

			private:
				struct UniformLocations
				{
					GLuint reflectionTexture;
					GLuint refractionTexture;
					GLuint dudvMap;
					GLuint matWorld;
					GLuint matView;
					GLuint matProj;
					GLuint tilingStrength;
					GLuint scaleFactor;
					GLuint dudvSampleOffset;
					GLuint cameraPosition;
					GLuint normalMap;
					GLuint lightColor;
					GLuint lightDir;
					GLuint quatSurfaceOrientation;
					GLuint shineDamper;
					GLuint reflectivity;
				} uniformLocations_;
			};
		}
	}
}