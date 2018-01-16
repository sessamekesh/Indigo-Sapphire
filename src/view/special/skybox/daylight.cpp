#include <view/special/skybox/daylight.h>
#include <resources.h>

namespace view
{
	namespace special
	{
		namespace skybox
		{
			// http://antongerdelan.net/opengl/cubemaps.html
			float points[] = {
				-10.f,  10.f, -10.f,
				-10.f, -10.f, -10.f,
				10.f, -10.f, -10.f,
				10.f, -10.f, -10.f,
				10.f,  10.f, -10.f,
				-10.f,  10.f, -10.f,

				-10.f, -10.f,  10.f,
				-10.f, -10.f, -10.f,
				-10.f,  10.f, -10.f,
				-10.f,  10.f, -10.f,
				-10.f,  10.f,  10.f,
				-10.f, -10.f,  10.f,

				10.f, -10.f, -10.f,
				10.f, -10.f,  10.f,
				10.f,  10.f,  10.f,
				10.f,  10.f,  10.f,
				10.f,  10.f, -10.f,
				10.f, -10.f, -10.f,

				-10.f, -10.f,  10.f,
				-10.f,  10.f,  10.f,
				10.f,  10.f,  10.f,
				10.f,  10.f,  10.f,
				10.f, -10.f,  10.f,
				-10.f, -10.f,  10.f,

				-10.f,  10.f, -10.f,
				10.f,  10.f, -10.f,
				10.f,  10.f,  10.f,
				10.f,  10.f,  10.f,
				-10.f,  10.f,  10.f,
				-10.f,  10.f, -10.f,

				-10.f, -10.f, -10.f,
				-10.f, -10.f,  10.f,
				10.f, -10.f, -10.f,
				10.f, -10.f, -10.f,
				-10.f, -10.f,  10.f,
				10.f, -10.f,  10.f
			};

			DaylightSkybox::DaylightSkybox(
				const glm::vec3& pos,
				const glm::quat& rotation,
				const glm::vec3& scl
			)
				: WithWorldTransform(pos, rotation, scl)
				, isReady_(false)
				, vertexBuffer_(0u)
				, vao_(0u)
				, texture_(nullptr)
			{}

			DaylightSkybox::~DaylightSkybox()
			{
				if (isReady_)
				{
					release();
				}
			}

			bool DaylightSkybox::prepare(std::shared_ptr<view::special::SkyboxShader> shader, util::PipelineState& pso)
			{
				if (isReady_)
				{
					release();
				}

				glGenVertexArrays(1, &vao_);
				glBindVertexArray(vao_);

				glGenBuffers(1, &vertexBuffer_);
				glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
				glBufferStorage(
					GL_ARRAY_BUFFER,
					108 * sizeof(float),
					&points[0],
					0x00
				);

				auto frontImg = model::readPNG(ASSET_PATH("texture/daylight-skybox/front.png"));
				auto backImg = model::readPNG(ASSET_PATH("texture/daylight-skybox/back.png"));
				auto leftImg = model::readPNG(ASSET_PATH("texture/daylight-skybox/left.png"));
				auto rightImg = model::readPNG(ASSET_PATH("texture/daylight-skybox/right.png"));
				auto topImg = model::readPNG(ASSET_PATH("texture/daylight-skybox/top.png"));
				auto bottomImg = model::readPNG(ASSET_PATH("texture/daylight-skybox/bottom.png"));

				if (!frontImg || !backImg || !leftImg || !rightImg || !topImg || !bottomImg)
				{
					return false;
				}

				texture_ = std::make_shared<view::Cubemap>();
				if (!texture_->init(*rightImg, *leftImg, *topImg, *bottomImg, *frontImg, *backImg))
				{
					release();
					return false;
				}
				shader->setVertexAttribPointers(pso);

				glBindVertexArray(NULL);

				isReady_ = true;
				return true;
			}

			bool DaylightSkybox::release()
			{
				if (vertexBuffer_ != 0u)
				{
					glDeleteBuffers(1, &vertexBuffer_);
					vertexBuffer_ = 0u;
				}

				if (texture_)
				{
					texture_->release();
					texture_ = nullptr;
				}

				isReady_ = false;
				return true;
			}

			void DaylightSkybox::render(std::shared_ptr<view::special::SkyboxShader> shader, util::PipelineState& pso)
			{
				glBindVertexArray(vao_);
				shader->setWorldMatrix(worldTransform());
				shader->setCubemap(texture_);
				glDrawArrays(GL_TRIANGLES, 0, 108);
			}
			void DaylightSkybox::render(std::shared_ptr<view::solidshader::SolidShader> ss)
			{
				glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
				//shader->setWorldMatrix(worldTransform());
				ss->setWorldMatrix(glm::mat4(1.f));
				view::solidshader::setVertexAttribPointers();
				//shader->setCubemap(texture_);
				glDrawArrays(GL_TRIANGLES, 0, 108);
			}
		}
	}
}