#pragma once

#include <view/entity.h>
#include <view/GenericVertex.h>
#include <view/rawentities/heightmapterrain.h>
#include <view/terrainshader/blendedterrainshader.h>
#include <model/withworldtransform.h>
#include <vector>

namespace view
{
	namespace terrainshader
	{
		class GenericBlendedTerrainEntity
			: public view::Entity<view::terrainshader::BlendedTerrainShader, view::terrainshader::BlendedTerrainShader::Vertex>
			, public model::WithWorldTransform
		{
		public:
			GenericBlendedTerrainEntity(
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
			);
			~GenericBlendedTerrainEntity();
			GenericBlendedTerrainEntity(const GenericBlendedTerrainEntity&) = delete;

			bool prepare(std::shared_ptr<view::terrainshader::BlendedTerrainShader> shader, util::PipelineState& pso);
			bool release();

			void render(std::shared_ptr<view::terrainshader::BlendedTerrainShader> shader);

		protected:
			std::shared_ptr<view::GenericMesh> rawMesh_;
			bool isReady_;

			std::shared_ptr<view::Texture> blendMap_;
			std::shared_ptr<view::Texture> redMap_;
			float redUVScale_;
			std::shared_ptr<view::Texture> greenMap_;
			float greenUVScale_;
			std::shared_ptr<view::Texture> blueMap_;
			float blueUVScale_;

			util::Logger log;

		private:
			GLuint vao_;
			GLuint vb_;
			GLuint ib_;
			std::uint32_t numIndices_;
		};
	}
}