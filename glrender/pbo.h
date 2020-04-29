#ifndef _PBO_H_
#define _PBO_H_

#define GLEW_STATIC
#include <GL/glew.h>

typedef void (*update_pixels_func)(unsigned char* src, int width, int height, unsigned char* dst);

class PboHelper
{
public:
	PboHelper(GLuint w, GLuint h) : 
		data_size(w*h*4), width(w), height(h) //w*h*4 ,*4是因为 pixel_format = GL_BGRA
	{
		buf_pbo_2_memery = new GLubyte[data_size];
		memset(buf_pbo_2_memery, 0, data_size);

		pboIdx = 0;		// 用于从默认FBO读取到PBO
		nextpboIdx = 0;	// 用于从PBO更新像素 绘制到默认FBO
		pixel_format = GL_BGRA;
	}
	~PboHelper() 
	{
		glDeleteBuffers(2, PBOIds);  // 注意释放PBO
		if (buf_pbo_2_memery) {
			delete[] buf_pbo_2_memery;
			buf_pbo_2_memery = nullptr;
		}
		
	}

	//PboHelper(PboHelper const&) {} // 单例模式不实现
	//void operator=(PboHelper const&) {} // 单例模式不实现

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
	// 准备PBO
	void preparePBO()
	{
		glGenBuffers(2, PBOIds);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, PBOIds[0]);
		//  GL_STREAM_DRAW用于传递数据到texture object GL_STREAM_READ 用于读取FBO中像素
		glBufferData(GL_PIXEL_PACK_BUFFER, data_size, NULL, GL_STREAM_READ);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, PBOIds[1]);
		glBufferData(GL_PIXEL_PACK_BUFFER, data_size, NULL, GL_STREAM_READ);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

		return;
	}
	// pack ======
	void async_read_pixels()
	{
		glReadBuffer(GL_FRONT); // 设置读取的FBO
		pboIdx = (pboIdx + 1) % 2;
		nextpboIdx = (pboIdx + 1) % 2;
		// 开始FBO到PBO复制操作 pack操作
		glBindBuffer(GL_PIXEL_PACK_BUFFER, PBOIds[pboIdx]);
		// OpenGL执行异步的DMA传输 这个命令会立即放回 此时CPU可以执行其他任务
		glReadPixels(0, 0, width, height, pixel_format, GL_UNSIGNED_BYTE, 0);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

		return;
	}

	//PBO映射到用户内存空间，用户传递函数指针修改该映射空间
	void update_pbo_pixels(update_pixels_func fn)
	{
		glBindBuffer(GL_PIXEL_PACK_BUFFER, PBOIds[nextpboIdx]);
		glBufferData(GL_PIXEL_PACK_BUFFER, data_size, 0, GL_STREAM_DRAW);
		// 从PBO映射到用户内存空间 然后修改PBO中数据
		GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
		if (ptr)
		{
			fn(ptr, width, height, buf_pbo_2_memery);
			glUnmapBuffer(GL_PIXEL_PACK_BUFFER); // 释放映射的用户内存空间
		}
		// 注意 不要误写为glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
		glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);

		return ;
	}

	//不使用PBO的方式，直接读取到用户内存
	void sync_read_pixels_and_update(update_pixels_func fn)
	{
		glReadBuffer(GL_FRONT); // 设置读取的FBO
		glReadPixels(0, 0, width, height, pixel_format, GL_UNSIGNED_BYTE, buf_pbo_2_memery);
		fn(buf_pbo_2_memery, width, height, buf_pbo_2_memery);
		
		return;
	}

	void pbo_draw(Shader& shader)
	{
		// 使用着色器时 下面这种方式无效 使用一个纹理替代
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
	//图片的宽，高
	GLuint width;
	GLuint height;

	//pack
	GLuint data_size;
	GLuint PBOIds[2];
	GLuint pixelTextId;
	GLuint pboIdx;
	GLuint nextpboIdx;
	GLenum pixel_format;
	GLubyte* buf_pbo_2_memery;// 像素数据
};

#endif // !_PBO_H_

