// OpenGL-Graphics.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <vector>
#include "common.h"
#include "glfwcallback.h"

// 包含着色器加载库
#include "shader.h"


// Window dimensions
const GLuint WIDTH = 960;
const GLuint HEIGHT = 540;

int main()
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	// Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_SAMPLES, 4);				// 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);	// We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//We don't want the old OpenGL
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	// Create a GLFWwindow object that we can use for GLFW's functions
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "glShaderUniform", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	
	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
	glewExperimental = GL_TRUE;
	// Initialize GLEW to setup the OpenGL Function pointers
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return -1;
	}

	// Define the viewport dimensions
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Set up vertex data (and buffer(s)) and attribute pointers. 指定顶点属性数据，顶点位置
	GLfloat vertices[] = {
		//顶点位置 
		-0.5f,  0.0f, 0.0f,// 0
		 0.5f,  0.0f, 0.0f,// 1
		 0.0f,  0.5f, 0.0f,// 2
		 0.0f, -0.5f, 0.0f,// 3
	};

	// 索引数据
	GLshort indices[] = {
		0, 1, 2,  // 第一个三角形
		0, 3, 1   // 第二个三角形
	};

	// 第一部分：创建缓存对象
	GLuint VAOId, VBOId, EBOId;
	// 1.创建并绑定VAO对象
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glGenVertexArrays(1, &VAOId);
	glBindVertexArray(VAOId);
	
	// 2.创建并绑定VBO对象
	glGenBuffers(1, &VBOId);
	glBindBuffer(GL_ARRAY_BUFFER, VBOId);

	// 3.创建并绑定EBO对象
	glGenBuffers(1, &EBOId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOId);

	// 4.分配空间，传送顶点数据
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	// 5.分配空间，传送索引数据
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	// 6.指定解析方式，并启用顶点属性
	// 顶点位置属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
		
	// 解除绑定，防止后续操作干扰到了当前VAO和VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
	glBindVertexArray(0);			  // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)


	//第二部分：准备着色器程序
	Shader shader("shader/triangle.vertex", "shader/triangle.frag");

	// Uncommenting this call will result in wireframe polygons.
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	//填充绘制
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	//用线来绘制

	// 开始游戏主循环
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();// 处理例如鼠标 键盘等事件

		// Render
		// 清除颜色缓冲区 重置为指定颜色
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// 这里填写场景绘制代码
		glBindVertexArray(VAOId);
		shader.use();

		// Update the uniform color
		GLfloat timeValue = glfwGetTime();
		GLfloat greenValue = (sin(timeValue) / 2) + 0.5; //sin(timeValue) 返回值在 [-1 , 1]
		shader.updateUniform4f("uniformColor", 0.0f, greenValue, 0.0f, 1.0f);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

		glBindVertexArray(0);
		glUseProgram(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// 释放资源
	glDeleteVertexArrays(1, &VAOId);
	glDeleteBuffers(1, &VBOId);

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}
