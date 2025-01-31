#pragma once

#include <glm/glm.hpp>
#include <model/withworldtransform.h>
#include <model/withboundingsphere.h>
#include <memory>
#include <view/grass/bladedgrasspatchshader.h>
#include <view/entity.h>
#include <model/withboundingsphere.h>
#include <model/specialgeo/heightfield.h>
#include <view/texture.h>
#include <util/surfacemaskbase.h>
#include <util/surfacemask/surfaceprobabilityfieldbase.h>

namespace view
{
	namespace grass
	{
		class BladedGrassPatchEntity
			: public view::Entity<BladedGrassPatchShader, BladedGrassPatchShader::Vertex>
			, public model::WithWorldTransform
			, public model::WithBoundingSphere
		{
		public:
			BladedGrassPatchEntity(
				const glm::vec3& pos,
				const glm::quat& rot,
				const glm::vec3& scl
			);
			~BladedGrassPatchEntity();
			BladedGrassPatchEntity(const BladedGrassPatchEntity&) = delete;

			bool prepare(
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
				std::uint32_t seed = 1u
			);

			struct CPUDeferrableWork
			{
				std::vector<BladedGrassPatchShader::Vertex> Vertices;
				model::geo::Sphere BoundingSphere;
			};
			std::optional<CPUDeferrableWork> prepareCPUDeferrable(
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
				std::uint32_t seed = 1u
			);

			bool prepareFromDeferrable(
				const CPUDeferrableWork& result,
				std::shared_ptr<BladedGrassPatchShader> shader,
				util::PipelineState& pso,
				std::shared_ptr<view::Texture> grassTexture,
				const glm::vec4& specularColor,
				float windStrength
			);

			bool release();
			void update(float dt);
			std::uint32_t numBlades() const;
			void render(std::shared_ptr<BladedGrassPatchShader> shader, float percentageToDraw = 1.f);

			// Inherited via WithBoundingSphere
			virtual model::geo::Sphere getBoundingSphere() override;

		private:
			GLuint vao_;
			GLuint vb_;
			std::uint32_t numVertices_;
			std::shared_ptr<view::Texture> texture_;
			glm::vec4 specularColor_;
			float timeElapsed_;
			float windStrength_;
			model::geo::Sphere boundingSphere_;
		};
	}
}