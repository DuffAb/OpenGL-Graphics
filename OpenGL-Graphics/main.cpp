// OpenGL-Graphics.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <vector>
#include "common.h"
#include "glfwcallback.h"

// 包含着色器加载库
#include "shader.h"
// 包含纹理加载辅助类
#include "texture.h"
// 包含数学辅助
#include "math.h"


// Window dimensions
const GLuint WIDTH = 960;
const GLuint HEIGHT = 540;


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

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Demo of mixing 2D texture(press A and S to adjust)", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// 创建的窗口的context指定为当前context
	glfwMakeContextCurrent(window);
	
	// 注册窗口键盘事件回调函数
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// 让glew获取所有拓展函数
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
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
	// 指定顶点属性数据 顶点位置 颜色 纹理
	GLfloat vertices[] = {
		0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,  // 0
		0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // 1
		0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,  // 2
		0.0f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // 3
	};

	// 索引数据
	GLshort indices[] = {
		0, 1, 2,  // 第一个三角形
		0, 2, 3   // 第二个三角形
	};

	// 指定坐标轴三角形箭头属性数据 顶点 颜色
	GLfloat axisTriangleData[] = {
		0.945f,    0.03125f,  0.0f,   1.0f, 0.0f, 0.0f, // +x轴三角形
		1.0f,      0.0f,      0.0f,   1.0f, 0.0f, 0.0f,
		0.945f,    -0.03125f, 0.0f,   1.0f, 0.0f, 0.0f,
		-0.03125f, 0.945f,    0.0f,   0.0f, 1.0f, 0.0f,// +y轴三角形
		0.0f,      1.0f,      0.0f,   0.0f, 1.0f, 0.0f,
		0.03125f,  0.945f,    0.0f,   0.0f, 1.0f, 0.0f,
		-0.03125f, 0.0f,      0.945f, 0.0f, 0.0f, 1.0f,// +z轴三角形
		0.0f,      0.0f,      1.0f,   0.0f, 0.0f, 1.0f,
		0.03125f,  0.0f,      0.945f, 0.0f, 0.0f, 1.0f,
	};
	// 指定坐标轴直线属性数据 顶点 颜色
	GLfloat axisLineData[] = {
		-1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 1.0f
	};

	// 创建缓存对象
	GLuint VAOId, VBOId, EBOId;
	// 1.创建并绑定VAO对象
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glGenVertexArrays(1, &VAOId);
	glBindVertexArray(VAOId);
	
	// 2.创建并绑定VBO 对象
	glGenBuffers(1, &VBOId);
	glBindBuffer(GL_ARRAY_BUFFER, VBOId);
	// 分配空间，传送顶点数据
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 3.创建并绑定EBO对象
	glGenBuffers(1, &EBOId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOId);
	// 分配空间，传送索引数据
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	
	// 4.指定解析方式，并启用顶点属性
	// 顶点位置属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	// 顶点颜色属性
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,	8 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	// 顶点纹理坐标
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,	8 * sizeof(GL_FLOAT), (GLvoid*)(6 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(2);

	// 解除绑定，防止后续操作干扰到了当前VAO和VBO
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // 注意不要解除EBO绑定
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
	glBindVertexArray(0);			  // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)

	// 准备坐标轴数据
	GLuint axisVAOIds[2], axisVBOIds[2];
	glGenVertexArrays(2, axisVAOIds);
	glBindVertexArray(axisVAOIds[0]);
	glGenBuffers(2, axisVBOIds);
	// 准备坐标轴箭头
	glBindBuffer(GL_ARRAY_BUFFER, axisVBOIds[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axisTriangleData), axisTriangleData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,	6 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,	6 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	// 准备坐标轴直线
	glBindVertexArray(axisVAOIds[1]);
	glBindBuffer(GL_ARRAY_BUFFER, axisVBOIds[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(axisLineData), axisLineData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,	6 * sizeof(GL_FLOAT), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,	6 * sizeof(GL_FLOAT), (GLvoid*)(3 * sizeof(GL_FLOAT)));
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//第二部分：准备着色器程序
	Shader shader("shader/modelTransformation/translation/rectangle.vertex", "shader/modelTransformation/translation/rectangle.frag");
	Shader axisShader("shader/modelTransformation/translation/axis.vertex", "shader/modelTransformation/translation/axis.frag");

	// Uncommenting this call will result in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	//填充绘制
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	//用线来绘制

	// Section3 准备纹理对象
	GLuint textureId = TextureHelper::load2DTexture("resources/textures/cat.png");

	// 开始游戏主循环
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents(); // 处理例如鼠标 键盘等事件

		// 清除颜色缓冲区 重置为指定颜色
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// 这里填写场景绘制代码
		glBindVertexArray(VAOId);
		shader.use();

		glm::mat4 projection;	// 投影变换矩阵
		glm::mat4 view;			// 视变换矩阵
		glm::mat4 model;		// 模型变换矩阵
		shader.updateUniformMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
		shader.updateUniformMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
		// 启用多个纹理单元 绑定纹理对象
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId);
		shader.updateUniform1i("tex", 0);// 设置纹理单元为0号

		// 绘制第一个矩形，第一象限，保持原位置
		shader.updateUniformMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

		// 绘制第二个矩形，第二象限，x轴平移-0.5f
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-0.5f, 0.0f, 0.0f));
		shader.updateUniformMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

		// 绘制第三个矩形，第三象限，x轴平移-0.8f，y轴平移-0.8f
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(-0.8f, -0.8f, 0.0f));
		shader.updateUniformMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

		// 绘制第四个矩形，第四象限，y轴平移-0.5f
		model = glm::mat4();
		model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
		shader.updateUniformMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

		// 绘制坐标轴
		glBindVertexArray(axisVAOIds[0]);
		axisShader.use();
		glDrawArrays(GL_TRIANGLES, 0, 9);
		glBindVertexArray(axisVAOIds[1]);
		glDrawArrays(GL_LINES, 0, 6);

		glBindVertexArray(0);
		glUseProgram(0);

		glfwSwapBuffers(window); // 交换缓存
	}

	// 释放资源
	glDeleteVertexArrays(1, &VAOId);
	glDeleteBuffers(1, &VBOId);

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}
