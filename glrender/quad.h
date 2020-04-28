#ifndef _QUAD_H_
#define _QUAD_H_
#define GLEW_STATIC
#include <GL/glew.h>

class QuadHelper
{
public:
	static QuadHelper& getInstance()
	{
		static QuadHelper instance;
		return instance;
	}
	~QuadHelper() 
	{
		glDeleteVertexArrays(1, &quadVAOId);
		glDeleteBuffers(1, &quadVBOId);
	}

private:
	QuadHelper()
	{

	}
	QuadHelper(QuadHelper const&) {} // 单例模式不实现
	void operator=(QuadHelper const&) {} // 单例模式不实现

public:
	// 建立矩形的VBO数据
	void prepareQuadVBO()
	{
		// 顶点属性数据 位置 纹理
		GLfloat quadVertices[] = {
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		glGenVertexArrays(1, &quadVAOId);
		glGenBuffers(1, &quadVBOId);
		glBindVertexArray(quadVAOId);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBOId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glBindVertexArray(0);
	}

	GLuint getVAO()
	{
		return quadVAOId;
	}
private:
	GLuint quadVAOId, quadVBOId;
};

#endif // !_QUAD_H_

