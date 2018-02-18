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

		std::optional<BladedGrassPatchEntity::CPUDeferrableWork> BladedGrassPatchEntity::prepareCPUDeferrable(
			std::shared_ptr<model::specialgeo::Heightfield> heightfield,
			std::shared_ptr<util::SurfaceProbabilityFieldBase> bladeGenerationProbability,
			std::shared_ptr<util::SurfaceMaskBase> surfaceMask,
			const glm::vec2& minXZ, const glm::vec2& maxXZ,
			std::uint32_t maxNumBlades,
			float minBaseWidth, float maxBaseWidth,
			float minHeight, float maxHeight,
			float minRotation, float maxRotation,
			float minTwistRate, float maxTwistRate,
			float minTaperRate, float maxTaperRate,
			std::uint32_t seed
		)
		{
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

			std::uint32_t n = vertices.size();
			glm::vec3 origin(0.f, 0.f, 0.f);
			float radius = 0.f;
			for (auto&& v : vertices)
			{
				origin += v.Pos * (1.f / n);
			}
			for (auto&& v : vertices)
			{
				radius = glm::max(radius, glm::sqrt(glm::dot(v.Pos - origin, v.Pos - origin)));
			}

			return BladedGrassPatchEntity::CPUDeferrableWork({ vertices, model::geo::Sphere(origin, radius) });
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
			auto deferrableWorkOpt = prepareCPUDeferrable(
				heightfield, bladeGenerationProbability, surfaceMask,
				minXZ, maxXZ, maxNumBlades, minBaseWidth, maxBaseWidth,
				minHeight, maxHeight, minRotation, maxRotation, minTwistRate, maxTwistRate,
				minTaperRate, maxTaperRate, seed);

			if (!deferrableWorkOpt)
			{
				return false;
			}

			return prepareFromDeferrable(*deferrableWorkOpt, shader, pso, grassTexture, specularColor, windStrength);
		}

		bool BladedGrassPatchEntity::prepareFromDeferrable(
			const BladedGrassPatchEntity::CPUDeferrableWork& result,
			std::shared_ptr<BladedGrassPatchShader> shader,
			util::PipelineState& pso,
			std::shared_ptr<view::Texture> grassTexture,
			const glm::vec4& specularColor,
			float windStrength
		) {
			auto verts = result.Vertices;
			auto boundingSphere = result.BoundingSphere;

			numVertices_ = verts.size();
			texture_ = grassTexture;
			specularColor_ = specularColor;
			timeElapsed_ = 0.f;
			windStrength_ = windStrength;
			boundingSphere_ = boundingSphere;

			if (numVertices_ == 0u)
			{
				return false;
			}
			else
			{
				return prepareInternal(verts, shader, pso, vao_, vb_, numVertices_);
			}
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

		std::uint32_t BladedGrassPatchEntity::numBlades() const
		{
			return numVertices_;
		}

		void BladedGrassPatchEntity::render(std::shared_ptr<BladedGrassPatchShader> shader, float percentageToDraw)
		{
			shader->setWorldMatrix(worldTransform());
			shader->setGrassBladeTexture(texture_);
			shader->setBladeSpecularColor(specularColor_);
			shader->setTimeElapsed(timeElapsed_);
			shader->setWind(windStrength_);

			shader->submitUBOs();

			unsigned int toDraw = (unsigned int)(glm::min(glm::max(0.f, percentageToDraw), 1.f) * numVertices_);

			glBindVertexArray(vao_);
			glDrawArrays(GL_POINTS, 0u, numVertices_);
		}

		model::geo::Sphere BladedGrassPatchEntity::getBoundingSphere()
		{
			return model::geo::Sphere(boundingSphere_.origin() + pos(), boundingSphere_.radius());
		}
	}
}