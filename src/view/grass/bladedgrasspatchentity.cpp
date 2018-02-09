#include <view/grass/bladedgrasspatchentity.h>
#include <random>

namespace view
{
	namespace grass
	{
		float getRandomInRange(float min, float max)
		{
			return ((rand() % 1000) * 0.001f * (max - min)) + min;
		}

		BladedGrassPatchEntity::BladedGrassPatchEntity(
			const glm::vec3& pos,
			const glm::quat& rot,
			const glm::vec3& scl
		)
			: view::Entity<BladedGrassPatchShader, BladedGrassPatchShader::Vertex>()
			, model::WithWorldTransform(pos, rot, scl)
			, model::WithBoundingSphere()
			, vao_(NULL)
			, vb_(NULL)
			, numVertices_(0u)
			, specularColor_(0.f, 0.f, 0.f, 1.f)
			, timeElapsed_(0.f)
			, windStrength_(0.f)
			, boundingSphere_(pos, 1.f)
		{}

		BladedGrassPatchEntity::~BladedGrassPatchEntity()
		{
			release();
		}

		bool BladedGrassPatchEntity::prepare(
			std::shared_ptr<BladedGrassPatchShader> shader,
			util::PipelineState& pso,
			std::shared_ptr<model::specialgeo::Heightfield> heightfield,
			std::shared_ptr<util::SurfaceProbabilityFieldBase> bladeGenerationProbability,
			std::shared_ptr<view::Texture> grassTexture,
			std::shared_ptr<util::SurfaceMaskBase> surfaceMask,
			const glm::vec2& minXZ, const glm::vec2& maxXZ,
			std::uint32_t maxNumBlades,
			float minBaseWidth, float maxBaseWidth,
			float minHeight, float maxHeight,
			float minRotation, float maxRotation,
			float minTwistRate, float maxTwistRate,
			float minTaperRate, float maxTaperRate,
			const glm::vec4& specularColor,
			float windStrength,
			std::uint32_t seed
		) {
			std::vector<BladedGrassPatchShader::Vertex> vertices;
			vertices.reserve(maxNumBlades);

			srand(seed);

			for (std::uint32_t idx = 0u; idx < maxNumBlades; idx++)
			{
				float x = getRandomInRange(minXZ.x, maxXZ.x);
				float z = getRandomInRange(minXZ.y, maxXZ.y);
				float y = heightfield->heightAtPos(x, z, 0.f);

				if (bladeGenerationProbability->getProbabilityAtPoint({ x, z }) >= getRandomInRange(0.f, 1.f))
				{
					vertices.push_back({
						{ x, y, z },
						getRandomInRange(minBaseWidth, maxBaseWidth),
						getRandomInRange(minHeight, maxHeight),
						getRandomInRange(minTaperRate, maxTaperRate),
						getRandomInRange(minRotation, maxRotation),
						getRandomInRange(minTwistRate, maxTwistRate)
					});
				}
			}

			numVertices_ = vertices.size();
			texture_ = grassTexture;
			specularColor_ = specularColor;
			timeElapsed_ = 0.f;
			windStrength_ = windStrength;

			std::uint32_t n = vertices.size();
			glm::vec3 origin(0.f, 0.f, 0.f);
			float radius = 0.f;
			for (auto&& v : vertices)
			{
				origin += v.Pos * (1.f / n);
			}
			for (auto&& v : vertices)
			{
				radius = glm::max(radius, (float)(v.Pos - origin).length());
			}
			boundingSphere_ = model::geo::Sphere(origin, radius);

			return prepareInternal(vertices, shader, pso, vao_, vb_, numVertices_);
		}

		bool BladedGrassPatchEntity::release()
		{
			numVertices_ = 0u;
			return releaseInternal(vao_, vb_);
		}

		void BladedGrassPatchEntity::update(float dt)
		{
			timeElapsed_ += dt;
		}

		void BladedGrassPatchEntity::render(std::shared_ptr<BladedGrassPatchShader> shader)
		{
			shader->setWorldMatrix(worldTransform());
			shader->setGrassBladeTexture(texture_);
			shader->setBladeSpecularColor(specularColor_);
			shader->setTimeElapsed(timeElapsed_);
			shader->setWind(windStrength_);

			shader->submitUBOs();

			glBindVertexArray(vao_);
			glDrawArrays(GL_POINTS, 0u, numVertices_);
		}

		model::geo::Sphere BladedGrassPatchEntity::getBoundingSphere()
		{
			return boundingSphere_;
		}
	}
}