#pragma once

#include <glad\glad.h>

namespace view
{
	template <typename BufferStruct>
	class UniformBufferObject
	{
	public:
		UniformBufferObject()
			: isDirty_(true)
			, ubo_(NULL)
			, data({})
		{}

		UniformBufferObject(const UniformBufferObject&) = delete;
		~UniformBufferObject()
		{
			release();
		}

		bool release()
		{
			if (ubo_ != NULL)
			{
				glDeleteBuffers(1, &ubo_);
				ubo_ = NULL;
			}

			return true;
		}

		bool prepare()
		{
			release();

			// TODO SESS: Change... all this. It's totally wrong.
			// https://www.packtpub.com/books/content/opengl-40-using-uniform-blocks-and-uniform-buffer-objects
			glGenBuffers(1, &ubo_);
			glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
			auto mapping = new BufferStruct;
			glBufferData(GL_UNIFORM_BUFFER, sizeof(BufferStruct), mapping, GL_DYNAMIC_DRAW);
			delete mapping;

			return true;
		}

		void markDirty()
		{
			isDirty_ = true;
		}

		void sync()
		{
			if (isDirty_)
			{
				glBindBuffer(GL_UNIFORM_BUFFER, ubo_);
				auto mapping = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
				memcpy_s(mapping, sizeof(BufferStruct), &data, sizeof(BufferStruct));
				glUnmapBuffer(GL_UNIFORM_BUFFER);
				// glBufferData(GL_UNIFORM_BUFFER, sizeof(BufferStruct), mapping, GL_DYNAMIC_DRAW);
			}
			isDirty_ = false;
		}

	public:
		BufferStruct data;
		GLuint ubo() const
		{
			return ubo_;
		}

	private:
		bool isDirty_;
		GLuint ubo_;
	};
}
