#pragma once

#include <view/GenericVertex.h>
#include <memory>
#include <map>
#include <view/shader.h>

namespace view
{
	template <typename ShaderType, typename VertexType>
	class Entity
	{
		static_assert(std::is_base_of<view::Shader, ShaderType>::value, "ShaderType must be a base class of view::Shader");
	public:

		bool releaseInternal(GLuint& o_VAO, GLuint& o_VB, GLuint& o_IB)
		{
			if (o_IB)
			{
				glDeleteBuffers(1, &o_IB);
				o_IB = 0u;
			}

			if (o_VB)
			{
				glDeleteBuffers(1, &o_VB);
				o_VB = 0u;
			}

			if (o_VAO)
			{
				glDeleteVertexArrays(1, &o_VAO);
				o_VAO = 0u;
			}

			return true;
		}

		bool releaseInternal(GLuint& o_VAO, GLuint& o_VB)
		{
			if (o_VB)
			{
				glDeleteBuffers(1, &o_VB);
				o_VB = NULL;
			}

			if (o_VAO)
			{
				glDeleteVertexArrays(1, &o_VAO);
				o_VAO = 0ull; // Did I just blow your mind?
			}

			return true;
		}

		bool prepareInternal(const std::vector<VertexType> vertices, const std::vector<std::uint32_t> indices, std::shared_ptr<ShaderType> shader, util::PipelineState& pso, GLuint& o_VAO, GLuint& o_VB, GLuint& o_IB, std::uint32_t& o_numIndices)
		{
			glGenVertexArrays(1, &o_VAO);
			glBindVertexArray(o_VAO);
			
			glGenBuffers(1, &o_VB);
			glBindBuffer(GL_ARRAY_BUFFER, o_VB);
			glBufferStorage(
				GL_ARRAY_BUFFER,
				vertices.size() * sizeof(VertexType),
				&vertices[0],
				0x00
			);
			glGenBuffers(1, &o_IB);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, o_IB);
			glBufferStorage(
				GL_ELEMENT_ARRAY_BUFFER,
				indices.size() * sizeof(std::uint32_t),
				&indices[0],
				0x00
			);
			o_numIndices = indices.size();
			shader->setVertexAttribPointers(pso);

			return true;
		}

		bool prepareInternal(const std::vector<VertexType> vertices, std::shared_ptr<ShaderType> shader, util::PipelineState& pso, GLuint& o_VAO, GLuint& o_VB, std::uint32_t& o_numPoints)
		{
			glGenVertexArrays(1, &o_VAO);
			glBindVertexArray(o_VAO);

			glGenBuffers(1, &o_VB);
			glBindBuffer(GL_ARRAY_BUFFER, o_VB);
			glBufferStorage(
				GL_ARRAY_BUFFER,
				vertices.size() * sizeof(VertexType),
				&vertices[0],
				0x00
			);
			o_numPoints = vertices.size();
			shader->setVertexAttribPointers(pso);

			return true;
		}
	};
}