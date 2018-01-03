#pragma once

#include <glad/glad.h>

namespace view
{
	class Framebuffer
	{
	public:
		Framebuffer();
		~Framebuffer();
		Framebuffer(const Framebuffer&) = delete;

		bool init(unsigned int textureWidth, unsigned int textureHeight);
		bool release();

		void bind();
		
		static void bindDefaultFramebuffer();

		GLuint colorBufferTexture() const;

	private:
		GLuint fbo_;
		GLuint colorBuffer_;
		GLuint depthBuffer_;
		bool isReady_;
		unsigned int width_, height_;
	};
}