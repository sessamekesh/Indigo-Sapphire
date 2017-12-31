#pragma once

#include <glad/glad.h>
#include <vector>
#include <model/imagedata.h>

namespace view
{
	class Texture
	{
	public:
		enum TEXTURE_TYPE
		{
			RGBA
		};

	public:
		Texture();
		~Texture();
		Texture(const Texture&) = delete;

		bool init(
			TEXTURE_TYPE type,
			const model::ImageData& imageData
		);
		bool release();

		GLuint texture() const;

	protected:
		bool isLoaded_;
		GLuint texture_;
	};
}