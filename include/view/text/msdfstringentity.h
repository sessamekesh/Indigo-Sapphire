#pragma once

#include <glm/glm.hpp>
#include <model/withworldtransform.h>
#include <memory>
#include <view/entity.h>
#include <view/text/msdftextshader.h>
#include <model/text/textatlas.h>

// TODO SESS: This is for STATIC strings only!
// You should be able to do DYNAMIC strings as well! Update the VBO X/Y positions... so maybe store those in their own buffers?

namespace view
{
	namespace text
	{
		class MSDFStringEntity
			: public view::Entity<MSDFTextShader, MSDFTextShader::Vertex>
			, public model::WithWorldTransform
		{
		public:
			MSDFStringEntity(
				const glm::vec3& pos,
				const glm::quat& rot,
				const glm::vec3& scl
			);
			~MSDFStringEntity();
			MSDFStringEntity(const MSDFStringEntity&) = delete;

			struct DeferrableWorkItem
			{
				std::vector<MSDFTextShader::Vertex> vertices;
				std::vector<std::uint32_t> indices;
				glm::vec4 color;
				std::uint32_t atlasRows;
				std::uint32_t atlasCols;
			};

			DeferrableWorkItem prepareCPUDeferrable(
				std::string text,
				float worldGlyphWidth,
				float worldGlyphHeight,
				const model::text::TextAtlas& atlasLookup,
				glm::vec4 color
			);

			bool prepareFromDeferrable(
				const DeferrableWorkItem& result,
				std::shared_ptr<MSDFTextShader> shader,
				util::PipelineState& pso,
				std::shared_ptr<view::Texture> atlasTexture
			);

			bool release();

			void render(std::shared_ptr<MSDFTextShader> shader);

		private:
			GLuint vao_;
			GLuint vb_;
			GLuint ib_;
			std::uint32_t numIndices_;
			std::shared_ptr<view::Texture> atlasTexture_;
			std::uint32_t atlasRows_;
			std::uint32_t atlasCols_;
			glm::vec4 color_;
		};
	}
}