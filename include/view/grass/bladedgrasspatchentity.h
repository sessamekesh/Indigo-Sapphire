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
				std::shared_ptr<view::Texture> grassTexture,
				std::shared_ptr<util::SurfaceMaskBase> surfaceMask,
				const glm::vec2& minXZ, const glm::vec2& maxXZ,
				float minDistance, float maxDistance,
				float minBaseWidth, float maxBaseWidth,
				float minHeight, float maxHeight,
				float minRotation, float maxRotation,
				float minTwistRate, float maxTwistRate,
				float minTaperRate, float maxTaperRate,
				const glm::vec4& specularColor,
				float windStrength,
				std::uint32_t seed = 1u
			);

			bool release();
			void update(float dt);
			void render(std::shared_ptr<BladedGrassPatchShader> shader);

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