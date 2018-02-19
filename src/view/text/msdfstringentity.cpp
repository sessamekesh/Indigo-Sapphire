#include <view/text/msdfstringentity.h>

namespace view
{
	namespace text
	{
		MSDFStringEntity::MSDFStringEntity(
			const glm::vec3& pos,
			const glm::quat& rot,
			const glm::vec3& scl
		)
			: view::Entity<MSDFTextShader, MSDFTextShader::Vertex>()
			, model::WithWorldTransform(pos, rot, scl)
			, vao_(0u)
			, vb_(0u)
			, ib_(0u)
			, numIndices_(0u)
			, atlasTexture_(nullptr)
			, color_(0.f, 0.f, 0.f, 1.f)
		{}

		MSDFStringEntity::~MSDFStringEntity()
		{
			release();
		}

		MSDFStringEntity::DeferrableWorkItem MSDFStringEntity::prepareCPUDeferrable(
			std::string text,
			float worldGlyphWidth,
			float worldGlyphHeight,
			const model::text::TextAtlas& atlasLookup,
			glm::vec4 color
		)
		{
			float x = 0.f;
			float y = 0.f;
			std::vector<MSDFTextShader::Vertex> vertices;
			std::vector<std::uint32_t> indices;
			vertices.reserve(text.size() * 4u);
			indices.reserve(text.size() * 6u);
			for (auto&& glyph : text)
			{
				if (glyph == ' ')
				{
					x += worldGlyphWidth;
				}
				else if (glyph == '\t')
				{
					x += worldGlyphWidth * 4.f;
				}
				else if (glyph == '\n')
				{
					x = 0.f;
					y -= worldGlyphHeight;
				}
				else
				{
					std::uint32_t glyphRow = 0u;
					std::uint32_t glyphCol = 0u;

					// TODO SESS: Take into account the actual size of glyphs, scale accordingly!
					if (atlasLookup.getGlyphRowCol(glyph, glyphRow, glyphCol))
					{
						auto offset = vertices.size();

						vertices.push_back({
							glm::vec3(x, y, 0.f),
							(float)glyphCol,
							(float)glyphRow
						});
						vertices.push_back({
							glm::vec3(x + worldGlyphWidth, y, 0.f),
							(float)(glyphCol + 1u),
							(float)glyphRow
						});
						vertices.push_back({
							glm::vec3(x, y - worldGlyphHeight, 0.f),
							(float)glyphCol,
							(float)(glyphRow + 1u)
						});
						vertices.push_back({
							glm::vec3(x + worldGlyphWidth, y - worldGlyphHeight, 0.f),
							(float)(glyphCol + 1u),
							(float)(glyphRow + 1u)
						});

						// TODO SESS: Is this the right winding order?
						indices.push_back(0 + offset);
						indices.push_back(2 + offset);
						indices.push_back(1 + offset);

						indices.push_back(1 + offset);
						indices.push_back(3 + offset);
						indices.push_back(2 + offset);
					}
					else
					{
						// TODO SESS: Error message here
						// Also, perhaps use a default character?
					}

					x += worldGlyphWidth;
				}
			}

			return { vertices, indices, color, atlasLookup.rows(), atlasLookup.cols() };
		}

		bool MSDFStringEntity::prepareFromDeferrable(
			const MSDFStringEntity::DeferrableWorkItem& result,
			std::shared_ptr<MSDFTextShader> shader,
			util::PipelineState& pso,
			std::shared_ptr<view::Texture> atlasTexture
		)
		{
			if (!prepareInternal(result.vertices, result.indices, shader, pso, vao_, vb_, ib_, numIndices_))
			{
				return false;
			}

			atlasTexture_ = atlasTexture;
			atlasRows_ = result.atlasRows;
			atlasCols_ = result.atlasCols;
			color_ = result.color;

			return true;
		}

		bool MSDFStringEntity::release()
		{
			return releaseInternal(vao_, vb_, ib_);
		}

		void MSDFStringEntity::render(std::shared_ptr<MSDFTextShader> shader)
		{
			shader->setTextureAtlas(atlasTexture_, atlasRows_, atlasCols_);
			shader->setTextColor(color_);
			shader->setWorldMatrix(worldTransform());
			
			glBindVertexArray(vao_);
			glDrawElements(GL_TRIANGLES, numIndices_, GL_UNSIGNED_INT, nullptr);
		}
	}
}