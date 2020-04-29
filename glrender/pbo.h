#ifndef _PBO_H_
#define _PBO_H_

#define GLEW_STATIC
#include <GL/glew.h>

typedef void (*update_pixels_func)(unsigned char* src, int width, int height, unsigned char* dst);

class PboHelper
{
public:
	PboHelper(GLuint w, GLuint h) : 
		data_size(w*h*4), width(w), height(h) //w*h*4 ,*4����Ϊ pixel_format = GL_BGRA
	{
		buf_pbo_2_memery = new GLubyte[data_size];
		memset(buf_pbo_2_memery, 0, data_size);

		pboIdx = 0;		// ���ڴ�Ĭ��FBO��ȡ��PBO
		nextpboIdx = 0;	// ���ڴ�PBO�������� ���Ƶ�Ĭ��FBO
		pixel_format = GL_BGRA;
	}
	~PboHelper() 
	{
		glDeleteBuffers(2, PBOIds);  // ע���ͷ�PBO
		if (buf_pbo_2_memery) {
			delete[] buf_pbo_2_memery;
			buf_pbo_2_memery = nullptr;
		}
		
	}

	//PboHelper(PboHelper const&) {} // ����ģʽ��ʵ��
	//void operator=(PboHelper const&) {} // ����ģʽ��ʵ��

public:
	

	void initPixelTexture()
	{
		glGenTextures(1, &pixelTextId);
		glBindTexture(GL_TEXTURE_2D, pixelTextId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, pixel_format, GL_UNSIGNED_BYTE, (GLvoid*)buf_pbo_2_memery);
		glBindTexture(GL_TEXTURE_2D, 0);

		return;
	}

public:
	// ׼��PBO
	void preparePBO()
	{
		glGenBuffers(2, PBOIds);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, PBOIds[0]);
		//  GL_STREAM_DRAW���ڴ������ݵ�texture object GL_STREAM_READ ���ڶ�ȡFBO������
		glBufferData(GL_PIXEL_PACK_BUFFER, data_size, NULL, GL_STREAM_READ);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, PBOIds[1]);
		glBufferData(GL_PIXEL_PACK_BUFFER, data_size, NULL, GL_STREAM_READ);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

		return;
	}
	// pack ======
	void async_read_pixels()
	{
		glReadBuffer(GL_FRONT); // ���ö�ȡ��FBO
		pboIdx = (pboIdx + 1) % 2;
		nextpboIdx = (pboIdx + 1) % 2;
		// ��ʼFBO��PBO���Ʋ��� pack����
		glBindBuffer(GL_PIXEL_PACK_BUFFER, PBOIds[pboIdx]);
		// OpenGLִ���첽��DMA���� �������������Ż� ��ʱCPU����ִ����������
		glReadPixels(0, 0, width, height, pixel_format, GL_UNSIGNED_BYTE, 0);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

		return;
	}

	//PBOӳ�䵽�û��ڴ�ռ䣬�û����ݺ���ָ���޸ĸ�ӳ��ռ�
	void update_pbo_pixels(update_pixels_func fn)
	{
		glBindBuffer(GL_PIXEL_PACK_BUFFER, PBOIds[nextpboIdx]);
		glBufferData(GL_PIXEL_PACK_BUFFER, data_size, 0, GL_STREAM_DRAW);
		// ��PBOӳ�䵽�û��ڴ�ռ� Ȼ���޸�PBO������
		GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
		if (ptr)
		{
			fn(ptr, width, height, buf_pbo_2_memery);
			glUnmapBuffer(GL_PIXEL_PACK_BUFFER); // �ͷ�ӳ����û��ڴ�ռ�
		}
		// ע�� ��Ҫ��дΪglBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

		return ;
	}

	//��ʹ��PBO�ķ�ʽ��ֱ�Ӷ�ȡ���û��ڴ�
	void sync_read_pixels_and_update(update_pixels_func fn)
	{
		glReadBuffer(GL_FRONT); // ���ö�ȡ��FBO
		glReadPixels(0, 0, width, height, pixel_format, GL_UNSIGNED_BYTE, buf_pbo_2_memery);
		fn(buf_pbo_2_memery, width, height, buf_pbo_2_memery);
		
		return;
	}

	void pbo_draw(Shader& shader)
	{
		// ʹ����ɫ��ʱ �������ַ�ʽ��Ч ʹ��һ���������
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pixelTextId);
		glBindVertexArray(QuadHelper::getInstance().getVAO());
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, pixel_format, GL_UNSIGNED_BYTE, (GLvoid*)buf_pbo_2_memery);
		shader.updateUniform1i("cubeText", 1);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

		return;
	}

	//unpack ========




private:
	//ͼƬ�Ŀ���
	GLuint width;
	GLuint height;

	//pack
	GLuint data_size;
	GLuint PBOIds[2];
	GLuint pixelTextId;
	GLuint pboIdx;
	GLuint nextpboIdx;
	GLenum pixel_format;
	GLubyte* buf_pbo_2_memery;// ��������
};

#endif // !_PBO_H_

