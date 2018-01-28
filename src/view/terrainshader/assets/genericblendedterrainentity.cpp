#include <view/terrainshader/assets/genericblendedterrainentity.h>

namespace view
{
	namespace terrainshader
	{
		GenericBlendedTerrainEntity::GenericBlendedTerrainEntity(
			const glm::vec3& pos,
			const glm::quat& rot,
			const glm::vec3& scl,
			std::shared_ptr<view::GenericMesh> rawMesh,
			std::shared_ptr<view::Texture> blendMap,
			std::shared_ptr<view::Texture> redMap,
			float redUVScale,
			std::shared_ptr<view::Texture> greenMap,
			float greenUVScale,
			std::shared_ptr<view::Texture> blueMap,
			float blueUVScale
		)
			: view::Entity<view::terrainshader::BlendedTerrainShader, view::terrainshader::BlendedTerrainShader::Vertex>()
			, model::WithWorldTransform(pos, rot, scl)
			, rawMesh_(rawMesh)
			, isReady_(false)
			, blendMap_(blendMap)
			, redMap_(redMap)
			, redUVScale_(redUVScale)
			, greenMap_(greenMap)
			, greenUVScale_(greenUVScale)
			, blueMap_(blueMap)
			, blueUVScale_(blueUVScale)
			, log(util::DEBUG, util::ERR, "[GenericBlendedTerrainEntity] ")
			, vao_(0u)
			, vb_(0u)
			, ib_(0u)
			, numIndices_(0u)
		{}

		GenericBlendedTerrainEntity::~GenericBlendedTerrainEntity()
		{
			release();
		}

		bool GenericBlendedTerrainEntity::prepare(std::shared_ptr<view::terrainshader::BlendedTerrainShader> shader, util::PipelineState& pso)
		{
			if (isReady_)
			{
				log.warn << "Preparing an already prepared entity - releasing. This may indicate a programming error." << util::endl;
				release();
			}

			auto vertices = view::terrainshader::BlendedTerrainShader::convertFromRaw(rawMesh_->vertices);

			if (!prepareInternal(vertices, rawMesh_->indices, shader, pso, vao_, vb_, ib_, numIndices_))
			{
				log.error << "Failed to prepare the mesh - prepareInternal failed" << util::endl;
				return false;
			}

			return true;
		}

		bool GenericBlendedTerrainEntity::release()
		{
			numIndices_ = 0u;
			return releaseInternal(vao_, vb_, ib_);
		}

		void GenericBlendedTerrainEntity::render(std::shared_ptr<view::terrainshader::BlendedTerrainShader> shader)
		{
			shader->setWorldMatrix(worldTransform());
			shader->setBlendMap(blendMap_);
			shader->setRedTexture(redMap_, redUVScale_);
			shader->setGreenTexture(greenMap_, greenUVScale_);
			shader->setBlueTexture(blueMap_, blueUVScale_);

			glBindVertexArray(vao_);
			glDrawElements(GL_TRIANGLES, numIndices_, GL_UNSIGNED_INT, nullptr);
		}
	}
}