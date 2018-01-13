#pragma once

#include <view/shader.h>
#include <view/texture.h>
#include <model/geo/plane.h>
#include <glm/glm.hpp>
#include <memory>
#include <model/light/directionallight.h>

namespace view
{
	namespace mappedphong
	{
		class MappedPhongShader : public Shader
		{
		public:
			struct Vertex
			{
				glm::vec3 vPos;
				glm::vec3 vNorm;
				glm::vec3 vTangent;
				glm::vec2 vUV;
			};

		public:
			MappedPhongShader();
			~MappedPhongShader() = default;
			MappedPhongShader(const MappedPhongShader&) = delete;

			void setWorldMatrix(const glm::mat4& m);
			void setViewMatrix(const glm::mat4& v);
			void setProjMatrix(const glm::mat4& p);
			void setClipPlane(const model::geo::Plane& plane);

			void setNormalMap(std::shared_ptr<Texture> tex);
			void setDiffuseMap(std::shared_ptr<Texture> tex);
			void setSpecularMap(std::shared_ptr<Texture> tex);

			void setLight(const model::light::DirectionalLight& light);
			void setCameraPosition(const glm::vec3& cameraPos);
			void setShininess(float shininess);

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

				GLuint normalMap;
				GLuint diffuseMap;
				GLuint specularMap;

				GLuint shininess;
				GLuint lightAmbient;
				GLuint lightDiffuse;
				GLuint lightSpecular;
				GLuint lightDirection;
				GLuint cameraPosition;
			} uniformLocation_;
		};
	}
}