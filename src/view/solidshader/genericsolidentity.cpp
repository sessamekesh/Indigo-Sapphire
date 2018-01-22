#include <view/solidshader/genericsolidentity.h>

namespace view
{
	namespace solidshader
	{
		GenericSolidEntity::GenericSolidEntity(
			const glm::vec3& pos,
			const glm::quat& rot,
			const glm::vec3& scl
		)
			: model::WithWorldTransform(pos, rot, scl)
		{}

		GenericSolidEntity::~GenericSolidEntity()
		{
			release();
		}

		void GenericSolidEntity::addMesh(std::shared_ptr<view::GenericMesh> m, const glm::vec4& color)
		{
			meshes_.push_back({ m, { {}, color } });
		}

		bool GenericSolidEntity::prepare(std::shared_ptr<view::solidshader::SolidShader> shader, util::PipelineState& pso)
		{
			for (auto&& m : meshes_)
			{
				if (m.second.glr.vao == 0u || m.second.glr.vb == 0u || m.second.glr.ib == 0u)
				{
					releaseInternal(m.second.glr.vao, m.second.glr.vb, m.second.glr.ib);
					auto verts = view::solidshader::SolidShader::translateVertices(m.first->vertices, m.second.color);
					prepareInternal(
						verts, m.first->indices, shader, pso,
						m.second.glr.vao, m.second.glr.vb, m.second.glr.ib, m.second.glr.numIndices
					);
				}
			}

			return true;
		}

		bool GenericSolidEntity::release()
		{
			for (auto&& m : meshes_)
			{
				releaseInternal(m.second.glr.vao, m.second.glr.vb, m.second.glr.ib);
			}
			meshes_.clear();

			return true;
		}

		void GenericSolidEntity::render(std::shared_ptr<view::solidshader::SolidShader> shader)
		{
			shader->setWorldMatrix(worldTransform());
			for (auto&& m : meshes_)
			{
				glBindVertexArray(m.second.glr.vao);
				glDrawElements(GL_TRIANGLES, m.second.glr.numIndices, GL_UNSIGNED_INT, nullptr);
			}
		}
	}
}