#include <glad/glad.h>
#include <view/special/metaball/metaballgroup.h>
#include <glm/gtc/constants.hpp>

namespace view
{
	namespace special
	{
		namespace metaball
		{
			MetaballGroup::MetaballGroup(
				std::shared_ptr<model::specialgeo::metaball::MetaballGroup> model,
				const glm::vec3& mtlDiffuse,
				const glm::vec3& mtlSpecular,
				float shininess
			)
				: model_(model)
				, vertexBuffer_(0u)
				, indexBuffer_(0u)
				, vao_(0u)
				, numIndices_(0u)
				, isReady_(false)
				, mtlDiffuse_(mtlDiffuse)
				, mtlSpecular_(mtlSpecular)
				, shininess_(shininess)
			{}

			MetaballGroup::~MetaballGroup()
			{
				if (isReady_)
				{
					release();
					isReady_ = false;
				}
			}

			bool MetaballGroup::preparePhong(
				const glm::vec3& cameraPos,
				const glm::vec3& lookAtDir,
				const glm::vec3& cameraUp
			)
			{
				if (!isReady_)
				{
					glGenVertexArrays(1, &vao_);
					glBindVertexArray(vao_);

					glGenBuffers(1, &vertexBuffer_);
					glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
					auto&& verts = getPhongVertices(cameraPos, lookAtDir, cameraUp);
					glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(view::special::metaball::PhongVertex), &verts[0], GL_DYNAMIC_DRAW);
					view::special::metaball::setPhongVertexAttribPointers();

					glGenBuffers(1, &indexBuffer_);
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer_);
					auto&& indices = getIndices();
					glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(std::uint32_t), &indices[0], 0x00);

					glBindVertexArray(NULL);

					numIndices_ = indices.size();
					isReady_ = true;
				}
				else
				{
					glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer_);
					auto&& verts = getPhongVertices(cameraPos, lookAtDir, cameraUp);
					if (verts.size() == 0u)
					{
						return false;
					}
					void* vbMap = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
					memcpy(vbMap, &verts[0], verts.size() * sizeof(view::special::metaball::PhongVertex));
					glUnmapBuffer(GL_ARRAY_BUFFER);
				}

				return true;
			}

			bool MetaballGroup::release()
			{
				if (isReady_)
				{
					if (indexBuffer_ != 0u)
					{
						glDeleteBuffers(1, &indexBuffer_);
						indexBuffer_ = 0u;
					}

					if (vertexBuffer_ != 0u)
					{
						glDeleteBuffers(1, &vertexBuffer_);
						vertexBuffer_ = 0u;
					}

					if (vao_ != 0u)
					{
						glDeleteVertexArrays(1, &vao_);
						vao_ = 0u;
					}
					isReady_ = false;
				}
				return true;
			}

			void MetaballGroup::renderPhong(std::shared_ptr<view::special::metaball::PhongShader> shader)
			{
				shader->setWorldMatrix(glm::mat4(1.f));
				shader->setMaterial(
					mtlDiffuse_,
					mtlSpecular_,
					shininess_
				);
				shader->setCharges(model_->getChargeData());
				
				glBindVertexArray(vao_);
				glDrawElements(GL_TRIANGLES, numIndices_, GL_UNSIGNED_INT, NULL);
			}

			std::vector<view::special::metaball::PhongVertex> MetaballGroup::getPhongVertices(
				const glm::vec3& cameraPos,
				const glm::vec3& lookAtDir,
				const glm::vec3& cameraUp
			)
			{
				auto rectOpt = model_->getOrthoBindingRect(cameraPos, lookAtDir, cameraUp);
				if (!rectOpt)
				{
					// TODO SESS: Error message here...
					return {};
				}

				auto rect = *rectOpt;

				glm::vec3 v1Pos = rect.pos() + glm::vec3(-rect.width(), rect.height(), 0.f);
				glm::vec3 v2Pos = rect.pos() + glm::vec3(rect.width(), rect.height(), 0.f);
				glm::vec3 v3Pos = rect.pos() + glm::vec3(rect.width(), -rect.height(), 0.f);
				glm::vec3 v4Pos = rect.pos() + glm::vec3(-rect.width(), -rect.height(), 0.f);

				std::vector<view::special::metaball::PhongVertex> verts;
				verts.reserve(4u);
				verts.push_back({ v1Pos });
				verts.push_back({ v2Pos });
				verts.push_back({ v3Pos });
				verts.push_back({ v4Pos });

				return verts;
			}

			std::vector<std::uint32_t> MetaballGroup::getIndices()
			{
				return std::vector<std::uint32_t>({
					0, 1, 2,
					0, 2, 3
				});
			}
		}
	}
}