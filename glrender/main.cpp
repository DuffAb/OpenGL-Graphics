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
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Demo of loading model with AssImp", nullptr, nullptr);
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

	//Section1 加载模型数据 为了方便更换模型 我们从文件读取模型文件路径
	Model objModel;
	if (!objModel.loadModel("resources/models/nanosuit/nanosuit.obj"))
	{
		glfwTerminate();
		std::system("pause");
		return -1;
	}

	// Section2 准备着色器程序
	Shader shader("shader/modelLoading/AssImpWithLight/model.vertex", "shader/modelLoading/AssImpWithLight/model.frag");
				
	//开启深度测试
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	// 开始游戏主循环
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents();	// 处理例如鼠标 键盘等事件
		do_movement();		// 根据用户操作情况 更新相机属性

		// 清除颜色缓冲区 重置为指定颜色
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		// 设置光源属性 点光源
		shader.updateUniform3f("light.ambient", 0.2f, 0.2f, 0.2f);
		shader.updateUniform3f("light.diffuse", 0.5f, 0.5f, 0.5f);
		shader.updateUniform3f("light.specular", 1.0f, 1.0f, 1.0f);
		shader.updateUniform3f("light.position", lampPos.x, lampPos.y, lampPos.z);
		// 设置衰减系数
		shader.updateUniform1f("light.constant", 1.0f);
		shader.updateUniform1f("light.linear", 0.09f);
		shader.updateUniform1f("light.quadratic", 0.032f);
		// 设置观察者位置
		shader.updateUniform3f("viewPos", camera.position.x, camera.position.y, camera.position.z);
		// 投影矩阵
		glm::mat4 projection = glm::perspective(camera.mouse_zoom, (GLfloat)(width) / height, 1.0f, 100.0f);
		// 视变换矩阵
		glm::mat4 view = camera.getViewMatrix();
		shader.updateUniformMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
		shader.updateUniformMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -1.55f, 0.0f)); // 适当下调位置
		model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f)); // 适当缩小模型
		shader.updateUniformMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
		// 这里填写场景绘制代码
		objModel.draw(shader); // 绘制物体

		glBindVertexArray(0);
		glUseProgram(0);
		glfwSwapBuffers(window); // 交换缓存
	}

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}