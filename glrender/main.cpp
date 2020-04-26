// OpenGL-Graphics.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include "common.h"
#include "glfwcallback.h"

// 包含着色器加载库
#include "shader.h"
// 包含相机控制辅助类
#include "camera.h"
// 包含纹理加载辅助类
#include "texture.h"
// 包含数学辅助
#include "math.h"
// 加载模型的类
#include "model.h"
// 准备framebuffer的类
#include "framebuffer.h"
// 天空包围盒类
#include "skybox.h"
// 包含字体管理类
#include "font.h"
// 帧率计算辅助类
#include "fps.h"
//
#include "cube.h"

int main()
{
	// Init GLFW
	if (!glfwInit())	// 初始化glfw库
	{
		std::cout << "Error::GLFW could not initialize GLFW!" << std::endl;
		return -1;
	}
	// 开启OpenGL 3.3 core profile
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	//glfwWindowHint(GLFW_SAMPLES, 4);				// 设置采样点个数4个，注意这里设置GLFW选项，不要写成了GL_SAMPLES
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//We don't want the old OpenGL
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// 创建窗口
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Demo of Frame Rate Counter", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 200, 100);
	// 创建的窗口的context指定为当前context
	glfwMakeContextCurrent(window);
	
	Event event(window);
	// 注册窗口键盘事件回调函数
	event.setEvent(glfwKey);
	// 注册鼠标事件回调函数
	event.setEvent(glfwCursorPos);
	// 注册鼠标滚轮事件回调函数
	event.setEvent(glfwScroll);
	// 注册窗口大小事件回调函数
	event.setEvent(glfwFramebufferSize);
	
	// 鼠标捕获 停留在程序内
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	// 初始化GLEW 获取OpenGL函数
	glewExperimental = GL_TRUE;// 让glew获取所有拓展函数
	GLenum status = glewInit();
	if (status != GLEW_OK)
	{
		std::cout << "Error::GLEW glew version:" << glewGetString(GLEW_VERSION) << " error string:" << glewGetErrorString(status) << std::endl;
		glfwTerminate();
		return -1;
	}

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	// 设置视口参数
	glViewport(0, 0, width, height);

	// Section1 准备顶点数据和FBO
	FontHelper::getInstance().prepareTextVBO();
	CubeHelper::getInstance().prepareCubeVBO();

	// Section2 加载字形
	FontHelper::getInstance().loadFont("arial", "resources/fonts/arial.ttf");
	FontHelper::getInstance().loadASCIIChar("arial", 38);

	// Section3 加载纹理

	CubeHelper::getInstance().prepareCubeTexture("resources/textures/wood.png");

	// Section3 准备着色器程序
	Shader shader("shader/textRendering/FPSCounter/scene.vertex", "shader/textRendering/FPSCounter/scene.frag");
	Shader textShader("shader/textRendering/FPSCounter/text.vertex", "shader/textRendering/FPSCounter/text.frag");

	glEnable(GL_DEPTH_TEST);	// 开启深度测试
	glEnable(GL_CULL_FACE);		// 开启面剔除
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	// 注意绘制文字时开启透明特性

	FpsHelper fpsCounter(glfwGetTime());
	std::wstring fpsStr;

	// 开始游戏主循环
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents();	// 处理例如鼠标 键盘等事件
		do_movement();		// 根据用户操作情况 更新相机属性

		// 设置colorBuffer颜色
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		// 清除colorBuffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(camera.mouse_zoom, (GLfloat)(width) / height, 0.01f, 100.0f); // 投影矩阵
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 model;

		// 先绘制场景
		shader.use();
		// 设置光源属性
		shader.updateUniform3f("light.ambient", 0.2f, 0.2f, 0.2f);
		shader.updateUniform3f("light.diffuse", 0.5f, 0.5f, 0.5f);
		shader.updateUniform3f("light.specular", 1.0f, 1.0f, 1.0f);
		shader.updateUniform3f("light.position", lampPos.x, lampPos.y, lampPos.z);
		// 设置观察者位置
		shader.updateUniform3f("viewPos", camera.position.x, camera.position.y, camera.position.z);
		// 设置变换矩阵
		shader.updateUniformMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
		shader.updateUniformMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
		shader.updateUniformMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
		CubeHelper::getInstance().draw(shader);
		// 绘制FPS文字
		// 绘制文字时使用正交投影
		textShader.use();
		projection = glm::ortho(0.0f, (GLfloat)(width), 0.0f, (GLfloat)height);
		view = glm::mat4();
		textShader.updateUniformMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
		textShader.updateUniformMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
		model = glm::mat4();
		textShader.updateUniformMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
		GLfloat xOffset = 0, yOffset = 0;
		xOffset = sin(glfwGetTime()) / 4.0 * width;
		yOffset = cos(glfwGetTime()) / 4.0 * height; // 偏移随机位置
		GLfloat xPos = width / 3.0f + xOffset;
		GLfloat yPos = height / 2.0f + yOffset;
		fpsCounter.frames++;
		double now = glfwGetTime();
		if (now - fpsCounter.lastFrameTime >= 1.0)
		{
			fpsStr = fpsCounter.getFramePerSecondsAsString();
			fpsCounter.resetCounter(now);
		}
		if (!fpsStr.empty())
		{
			FontHelper::getInstance().renderText(textShader, fpsStr,
				xPos,
				yPos,
				1.2f, glm::vec3(1.0f, 1.0f, 0.0f));
		}

		glBindVertexArray(0);
		glUseProgram(0);
		glfwSwapBuffers(window); // 交换缓存
	}

	// 释放资源
	glfwTerminate();
	return 0;
}