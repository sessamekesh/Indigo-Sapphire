#include <glad/glad.h>
#include <view/solidshader/assets/computermonitor.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

namespace view
{
	namespace solidshader
	{
		namespace assets
		{
			ComputerMonitor::ComputerMonitor(
				const glm::vec3& pos,
				const glm::quat& orientation,
				const glm::vec3& scl
			)
				: pos_(pos)
				, orientation_(orientation)
				, scl_(scl)
				, worldTransform_(1.f)
				, worldTransformDirty_(true)
				, glResources_({})
				, isReady_(false)
				, log_(util::WARN, util::WARN, "[ComputerMonitor (solidshader)] ")
			{}

			ComputerMonitor::~ComputerMonitor()
			{
				if (isReady_)
				{
					release();
				}
			}

			bool ComputerMonitor::prepare(std::shared_ptr<view::solidshader::SolidShader> shader, util::PipelineState& pso)
			{
				if (!isReady_)
				{
					auto importedGeo = importFromFile(ASSET_PATH("static/computerscreen/computer_screen.fbx"), glm::vec4(0.2f, 0.2f, 0.2f, 1.f), { log_ });
					if (!importedGeo)
					{
						log_.error << "Could not load geometry for computer monitor - asset preparation failed" << util::endl;
						return false;
					}

					auto geos = importedGeo.value();

					for (auto&& geo : geos)
					{
						GLResources glr = {};

						glGenVertexArrays(1, &glr.vao);
						glBindVertexArray(glr.vao);

						glGenBuffers(1, &glr.vertexBuffer);
						glBindBuffer(GL_ARRAY_BUFFER, glr.vertexBuffer);
						glBufferStorage(
							GL_ARRAY_BUFFER,
							geo.second.verts_.size() * sizeof(view::solidshader::SolidShaderVertex),
							&geo.second.verts_[0],
							0x00
						);

						glGenBuffers(1, &glr.indexBuffer);
						glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glr.indexBuffer);
						glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, geo.second.indices_.size() * sizeof(std::uint32_t), &geo.second.indices_[0], 0x00);

						shader->setVertexAttribPointers(pso);

						glr.numIndices = geo.second.indices_.size();

						glBindVertexArray(NULL);

						glResources_[geo.first] = glr;
					}

					isReady_ = true;
				}
				
				return true;
			}

			bool ComputerMonitor::release()
			{
				for (auto&& glr : glResources_)
				{
					if (glr.second.vao != 0u)
					{
						glDeleteVertexArrays(1, &glr.second.vao);
						glr.second.vao = 0u;
					}

					if (glr.second.vertexBuffer != 0u)
					{
						glDeleteBuffers(1, &glr.second.vertexBuffer);
						glr.second.vertexBuffer = 0u;
					}

					if (glr.second.indexBuffer != 0u)
					{
						glDeleteBuffers(1, &glr.second.indexBuffer);
						glr.second.indexBuffer = 0u;
					}
				}
				glResources_.clear();
				
				isReady_ = false;

				return true;
			}

			void ComputerMonitor::render(std::shared_ptr<view::solidshader::SolidShader> shader)
			{
				shader->setWorldMatrix(worldTransform());
				for (auto&& glr : glResources_)
				{
					glBindVertexArray(glr.second.vao);
					glDrawElements(GL_TRIANGLES, glr.second.numIndices, GL_UNSIGNED_INT, NULL);
				}
			}

			glm::mat4 ComputerMonitor::worldTransform()
			{
				if (worldTransformDirty_)
				{
					worldTransform_ = glm::mat4();
					worldTransform_ *= glm::translate(pos_);
					worldTransform_ *= glm::toMat4(orientation_);
					worldTransform_ *= glm::scale(scl_);
					worldTransformDirty_ = false;
				}

				return worldTransform_;
			}
		}
	}
}