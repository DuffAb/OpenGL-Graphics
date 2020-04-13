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

	// Section1 准备顶点数据
	// 指定顶点属性数据 顶点位置 纹理 法向量
	GLfloat vertices[] = {
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,	// A
		 0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,	// B
		 0.5f,  0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,	// C
		 0.5f,  0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,	// C
		-0.5f,  0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,	// D
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,	// A


		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,	// E
		-0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f, // H
		 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,	// G
		 0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,	// G
		 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,	// F
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,	// E

		-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,	// D
		-0.5f,  0.5f, -0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, // H
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,	// E
		-0.5f, -0.5f, -0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,	// E
		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,	// A
		-0.5f,  0.5f,  0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,	// D

		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // F
		0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // G
		0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, // C
		0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, // C
		0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // B
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, // F

		 0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,	// G
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,   // H
		-0.5f, 0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,	// D
		-0.5f, 0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,	// D
		 0.5f, 0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,	// C
		 0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,	// G

		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,  // A
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,  // E
		 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,  // F
		 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,  // F
		 0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,  // B
		-0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,  // A
	};

	glm::vec3 cubePositions[] = {
		glm::vec3( 0.0f,  0.0f,  0.0f),
		glm::vec3( 2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3( 2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3( 1.3f, -2.0f, -2.5f),
		glm::vec3( 1.5f,  2.0f, -2.5f),
		glm::vec3( 1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f, 0.2f, 2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f, 2.0f, -12.0f),
		glm::vec3(0.0f, 0.0f, -3.0f)
	};

	// 创建缓存对象
	GLuint VAOId, VBOId;
	// 1.创建并绑定VAO对象
	glGenVertexArrays(1, &VAOId);
	glBindVertexArray(VAOId);
	
	// 2.创建并绑定VBO对象
	glGenBuffers(1, &VBOId);
	glBindBuffer(GL_ARRAY_BUFFER, VBOId);
	// 分配空间，传送顶点数据
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 3.指定解析方式，并启用顶点属性
	// 顶点位置属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// 顶点颜色属性
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,	8 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	// 顶点纹理坐标
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE,	8 * sizeof(GL_FLOAT), (GLvoid*)(5 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);
	// 解除绑定，防止后续操作干扰到了当前VAO和VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// 创建光源的VAO
	GLuint lampVAOId;
	glGenVertexArrays(1, &lampVAOId);
	glBindVertexArray(lampVAOId);
	glBindBuffer(GL_ARRAY_BUFFER, VBOId); // 重用上面的数据，无需重复发送顶点数据，仍然需要指定解析方式
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0); // 只需要顶点位置即可
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	// 第二部分：准备着色器程序
	Shader shader("shader/lighting/multipleLight/cube.vertex", "shader/lighting/multipleLight/cube.frag");
	Shader lampShader("shader/lighting/multipleLight/lamp.vertex", "shader/lighting/multipleLight/lamp.frag");

	// Uncommenting this call will result in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	//填充绘制
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	//用线来绘制

	GLint diffuseMap = TextureHelper::load2DTexture("resources/textures/container_diffuse.png");
	GLint specularMap = TextureHelper::load2DTexture("resources/textures/container_specular.png");
	shader.use();
	shader.updateUniform1i("material.diffuseMap", 0);
	shader.updateUniform1i("material.specularMap", 1);
	
	//开启深度测试
	glEnable(GL_DEPTH_TEST);
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
		glBindVertexArray(VAOId);
		shader.use();

		// 投影变换矩阵
		glm::mat4 projection = glm::perspective(camera.mouse_zoom, (GLfloat)(width) / width, 1.0f, 100.0f);
		// 视变换矩阵
		glm::mat4 view = camera.getViewMatrix();
		
		setupLights(shader, pointLightPositions, sizeof(pointLightPositions) / sizeof(pointLightPositions[0]));
		
		
		// 设置材料光照属性
		// 启用diffuseMap
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		// 启用specularMap
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);
		shader.updateUniform1f("material.shininess", 32.0f);//镜面高光系数
		// 设置观察者位置
		shader.updateUniform3f("viewPos", camera.position.x, camera.position.y, camera.position.z);
		// 设置变换矩阵
		shader.updateUniformMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
		shader.updateUniformMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
		// 绘制多个立方体
		glm::mat4 model;// 模型变换矩阵
		for (int i = 0; i < sizeof(cubePositions) / sizeof(cubePositions[0]); ++i)
		{
			model = glm::mat4();
			model = glm::translate(model, cubePositions[i]);
			GLfloat angle = 20.0f * i;
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
			shader.updateUniformMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		
		// 利用立方体模拟点光源
		glBindVertexArray(lampVAOId);
		lampShader.use();
		lampShader.updateUniformMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
		lampShader.updateUniformMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
		for (int i = 0; i < sizeof(pointLightPositions) / sizeof(pointLightPositions[0]); ++i)
		{
			model = glm::mat4();
			model = glm::translate(model, lampPos);
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
			lampShader.updateUniformMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glBindVertexArray(0);
		glUseProgram(0);
		glfwSwapBuffers(window); // 交换缓存
	}

	// 释放资源
	glDeleteVertexArrays(1, &VAOId);
	glDeleteBuffers(1, &VBOId);
	glDeleteVertexArrays(1, &lampVAOId);
	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}

void setupLights(Shader& shader, glm::vec3* PointLightPositions, int pointLightCnt)
{
	shader.use();
	// 设置方向光源
	glm::vec3 lampDir(0.5f, 0.8f, 0.0f);
	shader.updateUniform3f("dirLight.ambient", 0.0f, 0.1f, 0.4f);
	shader.updateUniform3f("dirLight.diffuse", 0.0f, 0.2f, 0.8f);
	shader.updateUniform3f("dirLight.specular", 0.0f, 1.0f, 1.0f);
	shader.updateUniform3f("dirLight.direction", lampDir.x, lampDir.y, lampDir.z);

	// 设置点光源
	for (int i = 0; i < pointLightCnt; ++i)
	{
		// 设置光源属性 点光源
		std::stringstream indexStr;
		indexStr << i;
		std::string lightName = "pointLights[" + indexStr.str() + "]";  // 使用数组索引pointLights[i]
		shader.updateUniform3f((lightName + ".ambient").c_str(), 0.0f, 0.1f, 0.4f);
		shader.updateUniform3f((lightName + ".diffuse").c_str(), 0.0f, 0.1f, 0.6f);
		shader.updateUniform3f((lightName + ".specular").c_str(), 0.0f, 1.0f, 1.0f);
		glm::vec3 lampPos = PointLightPositions[i];
		shader.updateUniform3f((lightName + ".position").c_str(), lampPos.x, lampPos.y, lampPos.z);
		// 设置衰减系数
		shader.updateUniform1f((lightName + ".constant").c_str(), 1.0f);	// 衰减常数
		shader.updateUniform1f((lightName + ".linear").c_str(), 0.09f);		// 衰减一次系数
		shader.updateUniform1f((lightName + ".quadratic").c_str(), 0.032f);	// 衰减二次系数
	}

	// 设置FlashLight光源
	shader.updateUniform3f("spotLight.ambient", 0.0f, 0.1f, 0.4f);
	shader.updateUniform3f("spotLight.diffuse", 0.0f, 0.2f, 0.8f);
	shader.updateUniform3f("spotLight.specular", 0.0f, 1.0f, 1.0f);
	shader.updateUniform3f("spotLight.position", camera.position.x, camera.position.y, camera.position.z);
	shader.updateUniform3f("spotLight.direction", camera.forward.x, camera.forward.y, camera.forward.z);
	shader.updateUniform1f("spotLight.cutoff", cos(glm::radians(12.5f)));
	shader.updateUniform1f("spotLight.outerCutoff", cos(glm::radians(17.5f)));
	// 设置衰减系数
	shader.updateUniform1f("spotLight.constant", 1.0f);
	shader.updateUniform1f("spotLight.linear", 0.09f);
	shader.updateUniform1f("spotLight.quadratic", 0.032f);
}

// xoz平面内圆形坐标
glm::vec3 getEyePosCircle()
{
	GLfloat radius = 6.0f;
	GLfloat xPos = radius * cos(glfwGetTime());
	GLfloat zPos = radius * sin(glfwGetTime());
	return glm::vec3(xPos, 0.0f, zPos);
}
// 球形坐标 这里计算theta phi角度仅做示例演示
// 可以根据需要设定
glm::vec3 getEyePosSphere()
{
	GLfloat radius = 6.0f;
	GLfloat theta = glfwGetTime(), phi = glfwGetTime() / 2.0f;
	GLfloat xPos = radius * sin(theta) * cos(phi);
	GLfloat yPos = radius * sin(theta) * sin(phi);
	GLfloat zPos = radius * cos(theta);
	return glm::vec3(xPos, yPos, zPos);
}
