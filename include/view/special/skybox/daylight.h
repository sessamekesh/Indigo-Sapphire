#pragma once

#include <model/withworldtransform.h>
#include <view/cubemap.h>
#include <view/special/skyboxshader.h>

#include <view/solidshader/solidshader.h>

namespace view
{
	namespace special
	{
		namespace skybox
		{
			class DaylightSkybox : public model::WithWorldTransform
			{
			public:
				DaylightSkybox(
					const glm::quat& rotation,
					const glm::vec3& scl
				);
				~DaylightSkybox();
				DaylightSkybox(const DaylightSkybox&) = delete;

				bool prepare(std::shared_ptr<view::special::SkyboxShader> shader, util::PipelineState& pso);
				bool release();

				void render(std::shared_ptr<view::special::SkyboxShader> shader, util::PipelineState& pso);

				void render(std::shared_ptr < view::solidshader::SolidShader> ss);

			protected:
				bool isReady_;
				GLuint vao_;
				GLuint vertexBuffer_;
				std::shared_ptr<view::Cubemap> texture_;
			};
		}
	}
}