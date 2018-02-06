#include <view/grass/billboardgrassentity.h>
#include <random>

// Grass patches are drawn as a point list - a geometry shader is used to generate
//  the actual grass geometry.

namespace view
{
	namespace grass
	{
		BillboardGrassEntity::BillboardGrassEntity(glm::vec3 pos, glm::quat rot, glm::vec3 scl)
			: model::WithWorldTransform(pos, rot, scl)
			, isReady_(false)
			, vao_(NULL)
			, vb_(NULL)
			, log(util::DEBUG, util::DEBUG, "[BillboardGrassEntity ]")
			, timeElapsed_(0.f)
			, numPoints_(0u)
			, alphaTest_(0.f)
			, alphaMultiplier_(0.f)
			, minPatchHeight_(0.f)
			, maxPatchHeight_(0.f)
			, patchSize_(0.f)
			, wind_(0.f)
		{}

		BillboardGrassEntity::~BillboardGrassEntity()
		{
			release();
		}

		bool BillboardGrassEntity::prepare(
			std::shared_ptr<BillboardGrassShader> shader,
			util::PipelineState& pso,
			std::shared_ptr<model::specialgeo::Heightfield> heightfield,
			std::shared_ptr<view::Texture> grassTexture,
			std::shared_ptr<util::SurfaceMaskBase> mask,
			std::uint32_t seed,
			float patchOffsetMin,
			float patchOffsetMax,
			float grassPatchHeight,
			float alphaTest,
			float alphaMultiplier,
			float minPatchHeight,
			float maxPatchHeight,
			float patchSize,
			float wind
		) {
			if (isReady_)
			{
				log << "Already prepared - releasing previous entity" << util::endl;
				release();
			}

			std::srand(seed);

			std::vector<BillboardGrassShader::Vertex> verts;
			verts.reserve(1024u);
			float dx = 5.f, dz = 5.f;
			for (float x = -heightfield->width(); x < heightfield->width(); x += (dx = patchOffsetMin + (patchOffsetMax - patchOffsetMin) * float(std::rand() % 1000) * 0.001f))
			{
				for (float z = -heightfield->depth(); z < heightfield->depth(); z += (dz = patchOffsetMin + (patchOffsetMax - patchOffsetMin) * float(std::rand() % 1000) * 0.001f))
				{
					// Fuzz the x and z values
					float finalX = x + ((std::rand() % 1000 - 500) * 0.001f * dx);
					float finalZ = z + ((std::rand() % 1000 - 500) * 0.001f * dz);

					// Skip this one if it's not in the mask.
					if (mask->getMaskValue({ finalX, finalZ }))
					{
						verts.push_back({ { finalX, heightfield->heightAtPos(finalX, finalZ, 1.f), finalZ} });
					}
				}
			}

			tex_ = grassTexture;
			alphaTest_ = alphaTest;
			alphaMultiplier_ = alphaMultiplier;
			minPatchHeight_ = minPatchHeight;
			maxPatchHeight_ = maxPatchHeight;
			patchSize_ = patchSize;
			wind_ = wind;

			return prepareInternal(verts, shader, pso, vao_, vb_, numPoints_);
		}

		bool BillboardGrassEntity::release()
		{
			return releaseInternal(vao_, vb_);
		}

		void BillboardGrassEntity::update(float dt)
		{
			timeElapsed_ += dt;
		}

		bool BillboardGrassEntity::render(std::shared_ptr<BillboardGrassShader> shader)
		{
			shader->setWorldMatrix(worldTransform());
			shader->setTimePassed(timeElapsed_);
			shader->setTexture(tex_);
			// TODO SESS: These should be configurable.
			shader->setAlphaTest(alphaTest_, alphaMultiplier_);
			shader->setColor(glm::vec4({ 1.f, 1.f, 1.f, 1.f }));
			shader->setPatchSizeData(minPatchHeight_, maxPatchHeight_, patchSize_);
			shader->setWind(wind_);

			glBindVertexArray(vao_);
			glDrawArrays(GL_POINTS, 0, numPoints_);

			return true;
		}
	}
}