#ifndef _GLFW_CALL_BACK_H_
#define _GLFW_CALL_BACK_H_
#include <iostream>
#include "common.h"
#include "camera.h"

bool keys[1024];		// ���������¼
GLfloat mixValue = 0.4f;// �����ϲ���
bool pboUsed = false;	// PBO����

// Window dimensions
const GLuint WIDTH = 800;
const GLuint HEIGHT = 600;

// ���������������
GLfloat lastX = WIDTH / 2.0f, lastY = HEIGHT / 2.0f;
bool firstMouseMove = true;
GLfloat deltaTime = 0.0f; // ��ǰ֡����һ֡��ʱ���
GLfloat lastFrame = 0.0f; // ��һ֡ʱ��
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

glm::vec3 lampPos(1.2f, 1.0f, 2.0f);

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// ���û�����ESC��,��������window���ڵ�WindowShouldClose����Ϊtrue
	// �ر�Ӧ�ó���
	// std::cout << key << std::endl;
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // �رմ���
	}
	else if (key == GLFW_KEY_O && action == GLFW_PRESS)
	{
		glEnable(GL_MULTISAMPLE);
	}
	else if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		glDisable(GL_MULTISAMPLE);
	}
	else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
	{
		pboUsed = !pboUsed;
	}
}

// ���̻ص�����ԭ������
void key_callback_mix(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // �رմ���
	}
	else if (key == GLFW_KEY_S && action == GLFW_PRESS)
	{
		mixValue += 0.05f;
		if (mixValue > 1.0f)
			mixValue = 1.0f;
	}
	else if (key == GLFW_KEY_A && action == GLFW_PRESS)
	{
		mixValue -= 0.05f;
		if (mixValue < 0.0f)
			mixValue = 0.0f;
	}
}

void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouseMove) // �״�����ƶ�
	{
		lastX = xpos;
		lastY = ypos;
		firstMouseMove = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;
	//GLfloat yoffset = ypos - lastY; //��������ƶ������淴��

	lastX = xpos;
	lastY = ypos;

	camera.handleMouseMove(xoffset, yoffset);
}

// ����������ദ�������ֿ���
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.handleMouseScroll(yoffset);
}

// ����������ദ����̿���
void do_movement()
{

	if (keys[GLFW_KEY_W])
		camera.handleKeyPress(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.handleKeyPress(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.handleKeyPress(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.handleKeyPress(RIGHT, deltaTime);
}
enum EventType
{
	glfwKey,
	glfwCursorPos,
	glfwScroll,
	glfwFramebufferSize,
};

class Event
{
public:
	Event(GLFWwindow* w) : window(w){}
	~Event(){}

public:
	void setEvent(EventType e)
	{
		switch (e)
		{
		case glfwKey:
			glfwSetKeyCallback(window, key_callback);
			break;
		case glfwCursorPos:
			glfwSetCursorPosCallback(window, mouse_move_callback);
			break;
		case glfwScroll:
			glfwSetScrollCallback(window, mouse_scroll_callback);
			break;
		case glfwFramebufferSize:
			glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
			break;
		default:
			break;
		}
	}

private:
	GLFWwindow* window;

};


#endif // !_GLFW_CALL_BACK_H_




