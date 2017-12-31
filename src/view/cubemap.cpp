#include <view/cubemap.h>

namespace view
{
	Cubemap::Cubemap()
		: isLoaded_(false)
		, texture_(0u)
	{}

	Cubemap::~Cubemap()
	{
		if (isLoaded_)
		{
			release();
		}
	}

	bool Cubemap::init(
		const model::ImageData& left,
		const model::ImageData& right,
		const model::ImageData& top,
		const model::ImageData& bottom,
		const model::ImageData& front,
		const model::ImageData& back
	) {
		if (left.width == 0u || left.height == 0u
			|| right.width == 0u || right.height == 0u
			|| top.width == 0u || top.height == 0u
			|| bottom.width == 0u || bottom.height == 0u
			|| front.width == 0u || front.height == 0u
			|| back.width == 0u || back.height == 0u)
		{
			return false;
		}

		if (isLoaded_)
		{
			release();
		}

		glActiveTexture(GL_TEXTURE0);
		glGenTextures(1, &texture_);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture_);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA,
			front.width, front.height,
			0, GL_RGBA, GL_UNSIGNED_BYTE,
			&front.pixels[0]
		);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA,
			back.width, back.height,
			0, GL_RGBA, GL_UNSIGNED_BYTE,
			&back.pixels[0]
		);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA,
			top.width, top.height,
			0, GL_RGBA, GL_UNSIGNED_BYTE,
			&top.pixels[0]
		);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA,
			bottom.width, bottom.height,
			0, GL_RGBA, GL_UNSIGNED_BYTE,
			&bottom.pixels[0]
		);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA,
			right.width, right.height,
			0, GL_RGBA, GL_UNSIGNED_BYTE,
			&right.pixels[0]
		);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA,
			left.width, left.height,
			0, GL_RGBA, GL_UNSIGNED_BYTE,
			&left.pixels[0]
		);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(GL_TEXTURE_CUBE_MAP, NULL);
		isLoaded_ = true;
		return true;
	}

	bool Cubemap::release()
	{
		if (texture_ != 0u)
		{
			glDeleteTextures(1, &texture_);
			texture_ = 0u;
			isLoaded_ = false;
		}

		return true;
	}

	GLuint Cubemap::texture() const
	{
		return texture_;
	}
}