#include <view/framebuffer.h>

namespace view
{
	Framebuffer::Framebuffer()
		: fbo_(0u)
		, colorBuffer_(0u)
		, depthBuffer_(0u)
		, isReady_(false)
		, width_(0u)
		, height_(0u)
	{}

	Framebuffer::~Framebuffer()
	{
		if (isReady_)
		{
			release();
		}
	}

	bool Framebuffer::init(unsigned int textureWidth, unsigned int textureHeight)
	{
		if (isReady_)
		{
			return false;
		}

		width_ = textureWidth;
		height_ = textureHeight;

		glGenFramebuffers(1, &fbo_);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);

		glGenTextures(1, &colorBuffer_);
		glBindTexture(GL_TEXTURE_2D, colorBuffer_);
		glTexImage2D(GL_TEXTURE_2D, 0u, GL_RGB, textureWidth, textureHeight, 0u, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		
		glGenRenderbuffers(1, &depthBuffer_);
		glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer_);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, textureWidth, textureHeight);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer_, 0u);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer_);

		auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			return false;
		}
		
		Framebuffer::bindDefaultFramebuffer();

		isReady_ = true;

		return true;
	}

	bool Framebuffer::release()
	{
		if (fbo_)
		{
			glDeleteFramebuffers(1, &fbo_);
			fbo_ = 0u;
		}

		if (colorBuffer_)
		{
			glDeleteTextures(1, &colorBuffer_);
			colorBuffer_ = 0u;
		}

		isReady_ = false;
		return true;
	}

	void Framebuffer::bind()
	{
		//glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
		glViewport(0, 0, width_, height_);
	}

	void Framebuffer::bindDefaultFramebuffer()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0u);
		// TODO SESS: This does not set the viewport! The viewport needs to be changed!
	}

	GLuint Framebuffer::colorBufferTexture() const
	{
		return colorBuffer_;
	}
}