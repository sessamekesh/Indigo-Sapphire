#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <future>
#include <GLFW/glfw3.h>
#include <util/Logger.h>

namespace view
{
	namespace solidshader
	{
		struct SolidShaderVertex
		{
			glm::vec3 _vPos;
			glm::vec4 _cColor;
		};
		void setVertexAttribPointers();

		class SolidShader
		{
		public:
			SolidShader();
			~SolidShader();
			bool initialize();

			bool activate();

			void setWorldMatrix(const glm::mat4& v);
			void setViewMatrix(const glm::mat4& v);
			void setProjMatrix(const glm::mat4& p);

		private:
			bool _isLoaded;
			GLuint _program;
			util::Logger log;

			static const unsigned long MAX_INFO_LOG_LENGTH = 512l;

		protected:
			struct UniformLocationsStruct {
				GLuint matWorld;
				GLuint matView;
				GLuint matProj;
			} UniformLocations;
		};
	}
}