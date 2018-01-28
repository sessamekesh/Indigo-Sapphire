#include <view/texture.h>

namespace view
{
	Texture::Texture()
		: isLoaded_(false)
		, texture_(0u)
	{}

	Texture::~Texture()
	{
		if (isLoaded_)
		{
			release();
		}
	}

	bool Texture::init(
		TEXTURE_TYPE type,
		const model::ImageData& imageData)
	{
		// No empty images!
		if (imageData.width == 0u || imageData.height == 0u)
		{
			return false;
		}

		switch (type)
		{
		case Texture::RGBA:
			if (imageData.width * imageData.height * 4u != imageData.pixels.size())
			{
				return false;
			}
			break;
		default:
			return false;
		}

		if (isLoaded_)
		{
			release();
		}

		glGenTextures(1, &texture_);
		glBindTexture(GL_TEXTURE_2D, texture_);

		int gltype = GL_RGBA;
		switch (type)
		{
		case Texture::RGBA:
			gltype = GL_RGBA;
			break;
		default:
			return false;
		}

		glTexImage2D(
			GL_TEXTURE_2D,
			0, gltype,
			imageData.width, imageData.height,
			0u,
			gltype,
			GL_UNSIGNED_BYTE,
			&imageData.pixels[0]
		);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, NULL);

		isLoaded_ = true;
		return true;
	}

	bool Texture::release()
	{
		isLoaded_ = false;
		glDeleteTextures(1, &texture_);
		texture_ = 0u;
		return true;
	}

	GLuint Texture::texture() const
	{
		return texture_;
	}
}