#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include <iostream>
#include "common.h"
#include "texture.h"

class FramebufferHelper
{
public:
	/*
	* 附加纹理到Color, depth ,stencil Attachment
	*/
	static bool prepareColorDeptStencilFBO(GLsizei width, GLsizei height, GLuint& colorTextId, GLuint& depthStencilTextId, GLuint& fboId)
	{
		glGenFramebuffers(1, &fboId);
		glBindFramebuffer(GL_FRAMEBUFFER, fboId);
		// 附加 color attachment
		colorTextId = TextureHelper::makeAttachmentTexture(0, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE); // 创建FBO中的纹理
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTextId, 0);
		// 附加 depth stencil texture attachment
		depthStencilTextId = TextureHelper::makeAttachmentTexture(0, GL_DEPTH24_STENCIL8, width, height, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depthStencilTextId, 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			return false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return true;
	}

	/*
	* 附加纹理到Color Attachment 同时附加RBO到depth stencil Attachment
	*/
	static bool prepareColorRenderFBO(GLsizei width, GLsizei height, GLuint& colorTextId, GLuint& rboId, GLuint& fboId)
	{
		glGenFramebuffers(1, &fboId);
		glBindFramebuffer(GL_FRAMEBUFFER, fboId);
		// 附加 color attachment
		colorTextId = TextureHelper::makeAttachmentTexture(0, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE); // 创建FBO中的纹理
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTextId, 0);
		
		// 附加 depth stencil RBO attachment
		glGenRenderbuffers(1, &rboId);
		glBindRenderbuffer(GL_RENDERBUFFER, rboId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // 预分配内存
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboId);//将深度模板缓冲区使用RBO代替
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			return false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return true;
	}

	/*
	* 附加纹理到Color Attachment 同时附加RBO到depth stencil Attachment
	*/
	static bool prepareColorRenderMSFBO(GLsizei width, GLsizei height, const int samplesCnt, GLuint& colorTextId, GLuint& fboId)
	{
		glGenFramebuffers(1, &fboId);
		glBindFramebuffer(GL_FRAMEBUFFER, fboId);
		// 附加 color attachment
		colorTextId = TextureHelper::makeMAAttachmentTexture(samplesCnt, GL_RGB); // 创建FBO中的多采样纹理
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colorTextId, 0);
		// 附加 depth stencil RBO attachment
		GLuint rboId;
		glGenRenderbuffers(1, &rboId);
		glBindRenderbuffer(GL_RENDERBUFFER, rboId);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, samplesCnt, GL_DEPTH24_STENCIL8,
			width, height); // 预分配内存
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboId);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			return false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return true;
	}

	// 创建一个只包含color attachement的FBO
	static bool prepareIntermediateFBO(GLsizei width, GLsizei height, GLuint& colorTextId, GLuint& fboId)
	{
		glGenFramebuffers(1, &fboId);
		glBindFramebuffer(GL_FRAMEBUFFER, fboId);
		// 附加 color attachment
		colorTextId = TextureHelper::makeAttachmentTexture(0, GL_RGB, width,
			height, GL_RGB, GL_UNSIGNED_BYTE); // 创建FBO中的纹理
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTextId, 0);
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			return false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return true;
	}
};

#endif // !_FRAMEBUFFER_H_

