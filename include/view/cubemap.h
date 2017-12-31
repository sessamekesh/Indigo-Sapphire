#pragma once

#include <glad/glad.h>
#include <vector>
#include <model/imagedata.h>

namespace view
{
	class Cubemap
	{
	public:
		Cubemap();
		~Cubemap();
		Cubemap(const Cubemap&) = delete;

		bool init(
			const model::ImageData& left,
			const model::ImageData& right,
			const model::ImageData& top,
			const model::ImageData& bottom,
			const model::ImageData& front,
			const model::ImageData& back
		);
		bool release();

		GLuint texture() const;

	protected:
		bool isLoaded_;
		GLuint texture_;
	};
}