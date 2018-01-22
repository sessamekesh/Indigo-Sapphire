#pragma once

#include <view/entity.h>
#include <view/GenericVertex.h>
#include <view/solidshader/solidshader.h>
#include <model/withworldtransform.h>
#include <vector>

namespace view
{
	namespace solidshader
	{
		class GenericSolidEntity : public view::Entity<view::solidshader::SolidShader, view::solidshader::SolidShaderVertex>, public model::WithWorldTransform
		{
		public:
			GenericSolidEntity(
				const glm::vec3& pos,
				const glm::quat& rot,
				const glm::vec3& scl
			);
			~GenericSolidEntity();
			GenericSolidEntity(const GenericSolidEntity&) = delete;

			void addMesh(std::shared_ptr<view::GenericMesh> m, const glm::vec4& color);

			bool prepare(std::shared_ptr<view::solidshader::SolidShader> shader, util::PipelineState& pso);
			bool release();

			void render(std::shared_ptr<view::solidshader::SolidShader> shader);

		private:
			struct GLResource
			{
				GLResource() : vao(0u), vb(0u), ib(0u) {}

				GLuint vao;
				GLuint vb;
				GLuint ib;
				std::uint32_t numIndices;

				bool isPrepared() { return vao != 0u && vb != 0u && ib != 0u; }
			};

			struct RenderData
			{
				GLResource glr;
				glm::vec4 color;
			};

			std::vector<std::pair<std::shared_ptr<view::GenericMesh>, RenderData>> meshes_;
		};
	}
}