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
// 包含简单的obj读取文件
#include "simpleobjloader.h"
// 包含纹理加载辅助类
#include "texture.h"
// 包含数学辅助
#include "math.h"
// 包含相机控制辅助类
#include "camera.h"

// 准备光源
void setupLights(Shader& shader, glm::vec3* PointLightPositions, int pointLightCnt);

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
	glfwWindowHint(GLFW_SAMPLES, 4);				// 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//We don't want the old OpenGL
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// 创建窗口
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Demo of multiple lighting source", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
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
		std::cout << "Error::GLEW glew version:" << glewGetString(GLEW_VERSION)
			<< " error string:" << glewGetErrorString(status) << std::endl;
		glfwTerminate();
		return -1;
	}

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	// 设置视口参数
	glViewport(0, 0, width, height);

	// Section1 从obj文件加载数据
	std::vector<Vertex> vertData;
	if (!ObjLoader::loadFromFile("resources/models/cube/cube.obj", vertData))
	{
		std::cerr << "Could not load obj model, exit now.";
		std::system("pause");
		exit(-1);
	}

	// Section2 准备纹理
	GLint textureId = TextureHelper::loadDDS("resources/models/cube/cube.dds");
	
	// Section3 建立Mesh对象
	Mesh mesh(vertData, textureId);

	// Section4 准备着色器程序
	Shader shader("shader/modelLoading/simpleObjLoader/cube.vertex", "shader/modelLoading/simpleObjLoader/cube.frag");
		
	//开启深度测试
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	// 开始游戏主循环
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents(); // 处理例如鼠标 键盘等事件
		do_movement(); // 根据用户操作情况 更新相机属性

		// 清除颜色缓冲区 重置为指定颜色
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// 这里填写场景绘制代码
		shader.use();

		// 投影变换矩阵
		glm::mat4 projection = glm::perspective(camera.mouse_zoom, (GLfloat)(width) / width, 1.0f, 100.0f);
		// 视变换矩阵
		glm::mat4 view = camera.getViewMatrix();
		
		// 设置变换矩阵
		shader.updateUniformMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
		shader.updateUniformMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
		// 绘制多个立方体
		glm::mat4 model;// 模型变换矩阵
		model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
		shader.updateUniformMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
		
		// 这里填写场景绘制代码
		mesh.draw(shader); // 绘制物体

		glBindVertexArray(0);
		glUseProgram(0);
		glfwSwapBuffers(window); // 交换缓存
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}