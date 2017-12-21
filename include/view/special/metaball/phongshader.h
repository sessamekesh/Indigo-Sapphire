#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <GLFW/glfw3.h>
#include <util/Logger.h>

namespace view
{
	namespace special
	{
		namespace metaball
		{
			struct PhongVertex
			{
				glm::vec3 vPos;
				glm::vec4 cColor;
			};
			void setVertexAttribPointers();

			struct BasicGeometry
			{
				std::vector<PhongVertex> verts;
				std::vector<std::uint32_t> indices;
			};

			class PhongShader
			{
			public:
				PhongShader();
				~PhongShader() = default;
				PhongShader(const PhongShader&) = delete;

				bool initialize();
				bool activate();

				void setWorldMatrix(const glm::mat4& m);
				void setViewMatrix(const glm::mat4& m);
				void setProjMatrix(const glm::mat4& m);

				static const unsigned long MAX_INFO_LOG_LENGTH = 512l;

			private:
				bool isLoaded_;
				GLuint program_;
				util::Logger log;

			protected:
				struct UniformLocationsStruct
				{
					GLuint matWorld;
					GLuint matView;
					GLuint matProj;
				} uniformLocations_;
			};
		}
	}
}