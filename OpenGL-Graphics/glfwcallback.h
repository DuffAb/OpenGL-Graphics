#ifndef _GLFW_CALL_BACK_H_
#define _GLFW_CALL_BACK_H_
#include <iostream>
#include "common.h"

bool keys[1024];
GLfloat mixValue = 0.4f; // 纹理混合参数

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	// 当用户按下ESC键,我们设置window窗口的WindowShouldClose属性为true
	// 关闭应用程序
	std::cout << key << std::endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void key_callback_mix(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE); // 关闭窗口
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

#endif // !_GLFW_CALL_BACK_H_




