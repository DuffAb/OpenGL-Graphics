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
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Demo of EnviromentMapping(refraction)", nullptr, nullptr);
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

	//Section1 顶点属性数据
	// 指定立方体顶点属性数据 顶点位置 法向量
	GLfloat cubeVertices[] = {
		// 正面
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,	// A
		 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,	// B
		 0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f,	// C
		 0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f,	// C
		-0.5f,  0.5f, 0.5f, 0.0f, 0.0f, 1.0f,	// D
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,	// A

		// 背面
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,	// E
		-0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f, // H
		 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f,	// G
		 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f,	// G
		 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,	// F
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,	// E

		// 左侧面
		-0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,	// D
		-0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f, // H
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, // E
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,	// E
		-0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f,	// A
		-0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f,	// D

		// 右侧面
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,	// F
		0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f,	// G
		0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,	// C
		0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f,	// C
		0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f,	// B
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,	// F

		// 顶面
		 0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,	// G
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,   // H
		-0.5f, 0.5f,  0.5f, 0.0f, 1.0f, 0.0f,	// D
		-0.5f, 0.5f,  0.5f, 0.0f, 1.0f, 0.0f,	// D
		 0.5f, 0.5f,  0.5f, 0.0f, 1.0f, 0.0f,	// C
		 0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,	// G

		// 底面
		-0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f,	// A
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, // E
		 0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,	// F
		 0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,	// F
		 0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f,	// B
		-0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f,	// A
	};
	GLuint cubeVAOId, cubeVBOId;
	glGenVertexArrays(1, &cubeVAOId);
	glGenBuffers(1, &cubeVBOId);
	glBindVertexArray(cubeVAOId);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBOId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	// 顶点位置数据
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// 顶点法向量数据
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	// Section2 创建Skybox
	std::vector<const char*> faces;
	faces.push_back("resources/skyboxes/sky/sky_rt.jpg");
	faces.push_back("resources/skyboxes/sky/sky_lf.jpg");
	faces.push_back("resources/skyboxes/sky/sky_up.jpg");
	faces.push_back("resources/skyboxes/sky/sky_dn.jpg");
	faces.push_back("resources/skyboxes/sky/sky_bk.jpg");
	faces.push_back("resources/skyboxes/sky/sky_ft.jpg");

	/*faces.push_back("resources/skyboxes/urbansp/urbansp_rt.tga");
	faces.push_back("resources/skyboxes/urbansp/urbansp_lf.tga");
	faces.push_back("resources/skyboxes/urbansp/urbansp_up.tga");
	faces.push_back("resources/skyboxes/urbansp/urbansp_dn.tga");
	faces.push_back("resources/skyboxes/urbansp/urbansp_bk.tga");
	faces.push_back("resources/skyboxes/urbansp/urbansp_ft.tga");*/
	SkyBox skybox;
	skybox.init(faces);

	// Section3 准备着色器程序
	Shader shader("shader/environmentMapping/refraction/scene.vertex", "shader/environmentMapping/refraction/scene.frag");
	Shader skyBoxShader("shader/environmentMapping/refraction/skybox.vertex", "shader/environmentMapping/refraction/skybox.frag");

	glEnable(GL_DEPTH_TEST);	// 开启深度测试
	glEnable(GL_CULL_FACE);		// 开启面剔除
	glDepthFunc(GL_LESS);

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
		glm::mat4 view = camera.getViewMatrix(); // 视变换矩阵 移除translate部分
		glm::mat4 model;

		// 先绘制场景
		shader.use();
		shader.updateUniformMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
		shader.updateUniformMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
		shader.updateUniformMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
		glBindVertexArray(cubeVAOId);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.getTextId());
		shader.updateUniform1i("envText", 0);
		shader.updateUniform3f("cameraPos", camera.position.x, camera.position.y, camera.position.z); // 注意设置观察者位置
		glDrawArrays(GL_TRIANGLES, 0, 36);

		// 然后绘制包围盒
		skyBoxShader.use();
		view = glm::mat4(glm::mat3(camera.getViewMatrix())); // 视变换矩阵 移除translate部分
		skyBoxShader.updateUniformMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
		skyBoxShader.updateUniformMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.getTextId()); // 注意绑定到CUBE_MAP
		skyBoxShader.updateUniform1i("skybox", 0);
		skybox.draw(skyBoxShader);

		glUseProgram(0);
		glDepthFunc(GL_LESS);
		glfwSwapBuffers(window); // 交换缓存
	}

	// 释放资源
	glDeleteVertexArrays(1, &cubeVAOId);
	glDeleteBuffers(1, &cubeVBOId);
	glfwTerminate();
	return 0;
}