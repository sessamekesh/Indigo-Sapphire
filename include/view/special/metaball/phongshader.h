#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <GLFW/glfw3.h>
#include <util/Logger.h>
#include <model/specialgeo/metaball/charge.h>

namespace view
{
	namespace special
	{
		namespace metaball
		{
			struct PhongVertex
			{
				glm::vec3 vPos;
			};
			void setPhongVertexAttribPointers();

			struct BasicGeometry
			{
				std::vector<PhongVertex> verts;
				std::vector<std::uint32_t> indices;
			};

			class PhongShader
			{
			public:
				PhongShader();
				~PhongShader();
				PhongShader(const PhongShader&) = delete;

				bool initialize();
				bool activate();

				void setWorldMatrix(const glm::mat4& m);
				void setViewMatrix(const glm::mat4& m);
				void setProjMatrix(const glm::mat4& m);

				void setCharges(const std::vector<model::specialgeo::metaball::Charge>& charges);
				void setCameraPos(const glm::vec3& cp);
				void setLightDir(const glm::vec3& ld);
				void setLight(const glm::vec3& ambient, const glm::vec3& diffuse, const glm::vec3& specular);
				void setMaterial(const glm::vec3& diffuse, const glm::vec3& specular, float shininess);
				void setStepSize(float stepSize);

				static const unsigned long MAX_INFO_LOG_LENGTH = 512l;

			private:
				bool isLoaded_;
				GLuint program_;
				util::Logger log;

			public:
				const static unsigned int MAX_CHARGES = 30u;

			protected:
				struct UniformLocationsStruct
				{
					GLuint matWorld;
					GLuint matView;
					GLuint matProj;
					GLuint charges;
					GLuint numCharges;
					GLuint cameraPos;
					GLuint lightDir;
					GLuint ambientColor;
					GLuint diffuseColor;
					GLuint specularColor;
					GLuint shininess;
					GLuint mtlDiffuse;
					GLuint mtlSpecular;
					GLuint stepSize;
				} uniformLocations_;
			};
		}
	}
}