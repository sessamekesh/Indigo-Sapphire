#pragma once

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <future>
#include <util/Logger.h>
#include <view/shader.h>
#include <vector>
#include <model/geo/plane.h>

namespace view
{
	namespace solidshader
	{
		struct SolidShaderVertex
		{
			glm::vec3 _vPos;
			glm::vec4 _cColor;
		};

		struct BasicGeometry
		{
			std::vector<SolidShaderVertex> verts_;
			std::vector<std::uint32_t> indices_;
		};

		class SolidShader : public view::Shader
		{
		public:
			SolidShader();
			SolidShader(const SolidShader&) = delete;
			~SolidShader() = default;

			void setWorldMatrix(const glm::mat4& v);
			void setViewMatrix(const glm::mat4& v);
			void setProjMatrix(const glm::mat4& p);
			void setClipPlane(const model::geo::Plane& clipPlane);

			static std::vector<SolidShaderVertex> translateVertices(const std::vector<view::GenericVertex>& verts, const glm::vec4& color);

			//
			// Overrides
			//
		protected:
			virtual void setVertexAttribPointersInternal() override;
			virtual unsigned int getNumVertexAttribPointers() override;
			virtual bool getUniformLocations() override;

		protected:
			struct UniformLocationsStruct {
				GLuint matWorld;
				GLuint matView;
				GLuint matProj;
				GLuint clipPlaneOrigin;
				GLuint clipPlaneNormal;
			} UniformLocations;
		};
	}
}