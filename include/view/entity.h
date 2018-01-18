#pragma once

#include <view/GenericVertex.h>
#include <memory>
#include <map>

namespace view
{
	template <typename ShaderType>
	class Entity
	{
		static_assert(std::is_base_of<view::Shader, ShaderType>::value, "ShaderType must be a base class of view::Shader");
	public:

		virtual bool prepare(std::shared_ptr<std::map<std::string, view::GenericMesh>> rawEntity, std::shared_ptr<ShaderType> shader, util::PipelineState& pso) = 0;
		virtual bool release() = 0;

		bool prepareInternal(view::GenericMesh gm, std::shared_ptr<ShaderType> shader, util::PipelineState& pso, GLuint& o_VAO, GLuint& o_VB, GLuint& o_IB, std::uint32_t& o_numIndices)
		{
			std::vector<std::uint8_t> vbData = shader->getVertexBuffer(gm.vertices);

			glCreateVertexArrays(1, &o_VAO);
			glBindVertexArray(o_VAO);
			
			glGenBuffers(1, &o_VB);
			glBindBuffer(GL_ARRAY_BUFFER, o_VB);
			glBufferStorage(
				GL_ARRAY_BUFFER,
				vbData.size(),
				&vbData[0],
				0x00
			);
			glGenBuffers(1, &o_IB);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, o_IB);
			glBufferStorage(
				GL_ELEMENT_ARRAY_BUFFER,
				gm.indices.size() * sizeof(std::uint32_t),
				&gm.indices[0],
				0x00
			);
			o_numIndices = gm.indices.size();
			shader->setVertexAttribPointers(pso);

			return true;
		}
	};
}