#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <model/specialgeo/heightfield.h>
#include <memory>
#include <util/surfacemaskbase.h>
#include <model/withworldtransform.h>
#include <util/Logger.h>
#include <view/grass/GrassShader.h>
#include <util/pipelinestate.h>

// http://www.mbsoftworks.sk/index.php?page=tutorials&series=1&tutorial=32

namespace view
{
	namespace grass
	{
		class GrassEntity : public model::WithWorldTransform
		{
		public:
			GrassEntity(glm::vec3 pos,glm::quat rot,glm::vec3 scl);
			GrassEntity(const GrassEntity&) = delete;
			~GrassEntity();

			bool prepare(
				std::shared_ptr<view::grass::GrassShader> shader,
				util::PipelineState& pso,
				std::shared_ptr<model::specialgeo::Heightfield> heightfield,
				std::shared_ptr<view::Texture> grassTexture,
				std::shared_ptr<util::SurfaceMaskBase> mask,
				std::uint32_t seed = 1u,
				float patchOffsetMin = 0.1f,
				float patchOffsetMax = 0.14f,
				float grassPatchHeight = 0.2f
			);

			bool release();
			void update(float dt);

			bool render(std::shared_ptr<view::grass::GrassShader> shader);

		private:
			bool isReady_;
			GLuint vao_;
			GLuint vb_;
			std::shared_ptr<view::Texture> tex_;
			util::Logger log;
			std::uint32_t numPoints_;
			float timeElapsed_;
		};
	}
}