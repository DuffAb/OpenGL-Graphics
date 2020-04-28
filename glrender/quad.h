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
	QuadHelper(QuadHelper const&) {} // ����ģʽ��ʵ��
	void operator=(QuadHelper const&) {} // ����ģʽ��ʵ��

public:
	// �������ε�VBO����
	void prepareQuadVBO()
	{
		// ������������ λ�� ����
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

