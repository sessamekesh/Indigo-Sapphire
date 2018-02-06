#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <model/specialgeo/heightfield.h>
#include <memory>
#include <util/surfacemaskbase.h>
#include <model/withworldtransform.h>
#include <util/Logger.h>
#include <view/grass/billboardgrassshader.h>
#include <view/entity.h>
#include <util/pipelinestate.h>

// http://www.mbsoftworks.sk/index.php?page=tutorials&series=1&tutorial=32

namespace view
{
	namespace grass
	{
		class BillboardGrassEntity : public view::Entity<BillboardGrassShader, BillboardGrassShader::Vertex>, public model::WithWorldTransform
		{
		public:
			BillboardGrassEntity(glm::vec3 pos, glm::quat rot, glm::vec3 scl);
			~BillboardGrassEntity();
			BillboardGrassEntity(const BillboardGrassEntity&) = delete;

			bool prepare(
				std::shared_ptr<BillboardGrassShader> shader,
				util::PipelineState& pso,
				std::shared_ptr<model::specialgeo::Heightfield> heightfield,
				std::shared_ptr<view::Texture> grassTexture,
				std::shared_ptr<util::SurfaceMaskBase> mask,
				std::uint32_t seed = 1u,
				float patchOffsetMin = 0.1f,
				float patchOffsetMax = 0.14f,
				float grassPatchHeight = 0.2f,
				float alphaTest = 0.25f,
				float alphaMultiplier = 1.5f,
				float minPatchHeight = 0.15f,
				float maxPatchHeight = 0.35f,
				float patchSize = 0.15f,
				float wind = 0.15f
			);

			void update(float dt);

			bool release();
			bool render(std::shared_ptr<BillboardGrassShader> shader);

		private:
			bool isReady_;
			GLuint vao_;
			GLuint vb_;
			std::shared_ptr<view::Texture> tex_;
			util::Logger log;
			std::uint32_t numPoints_;
			float timeElapsed_;
			float alphaTest_, alphaMultiplier_;
			float minPatchHeight_, maxPatchHeight_, patchSize_;
			float wind_;
		};
	}
}