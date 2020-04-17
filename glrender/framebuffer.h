#ifndef _FRAMEBUFFER_H_
#define _FRAMEBUFFER_H_

#include <iostream>
#include "common.h"
#include "texture.h"

class FramebufferHelper
{
public:
	/*
	* ��������Color, depth ,stencil Attachment
	*/
	static bool prepareColorDeptStencilFBO(GLsizei width, GLsizei height, GLuint& colorTextId, GLuint& depthStencilTextId, GLuint& fboId)
	{
		glGenFramebuffers(1, &fboId);
		glBindFramebuffer(GL_FRAMEBUFFER, fboId);
		// ���� color attachment
		colorTextId = TextureHelper::makeAttachmentTexture(0, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE); // ����FBO�е�����
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTextId, 0);
		// ���� depth stencil texture attachment
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
	* ��������Color Attachment ͬʱ����RBO��depth stencil Attachment
	*/
	static bool prepareColorRenderFBO(GLsizei width, GLsizei height, GLuint& textId, GLuint& fboId)
	{
		glGenFramebuffers(1, &fboId);
		glBindFramebuffer(GL_FRAMEBUFFER, fboId);
		// ���� color attachment
		textId = TextureHelper::makeAttachmentTexture(0, GL_RGB, width, height, GL_RGB, GL_UNSIGNED_BYTE); // ����FBO�е�����
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textId, 0);
		// ���� depth stencil RBO attachment
		GLuint rboId;
		glGenRenderbuffers(1, &rboId);
		glBindRenderbuffer(GL_RENDERBUFFER, rboId);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height); // Ԥ�����ڴ�
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboId);//�����ģ�建����ʹ��RBO����
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			return false;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return true;
	}

};

#endif // !_FRAMEBUFFER_H_

