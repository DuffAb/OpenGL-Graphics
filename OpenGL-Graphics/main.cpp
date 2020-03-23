// OpenGL-Graphics.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <vector>
#include "common.h"
#include "glfwcallback.h"

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
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "glTriangleVertex", nullptr, nullptr);
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
		//左边大三角形
		-0.7f, -0.5f, 0.0f, // Left  
		 0.3f, -0.5f, 0.0f, // Right 
		-0.2f,  0.5f, 0.0f, // Top   
		 //右边小三角形 
		 0.5f, -0.5f, 0.0f, // Left
		 1.0f, -0.5f, 0.0f, // Right
		0.75f,  0.0f, 0.0f  // Top  
	};

	// 第一部分：创建缓存对象
	GLuint VAOId, VBOId;
	// 1.创建并绑定VAO对象
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glGenVertexArrays(1, &VAOId);
	glBindVertexArray(VAOId);
	
	// 2.创建并绑定VBO对象
	glGenBuffers(1, &VBOId);
	glBindBuffer(GL_ARRAY_BUFFER, VBOId);

	// 3.分配空间，传送数据
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// 4.指定解析方式，并启用顶点属性
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// 解除绑定，防止后续操作干扰到了当前VAO和VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
	glBindVertexArray(0);			  // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)


	//第二部分：准备着色器程序
	// 1.准备着色器源代码
	const GLchar* vertexShaderSource = "#version 330\n"
		"layout(location = 0) in vec3 position;\n"
		"void main()\n"
		"{\n"
			"gl_Position = vec4(position, 1.0);\n"
		"}\0";
	const GLchar* fragShaderSource = "#version 330\n"
		"out vec4 color;\n"
		"void main()\n"
		"{\n color = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n}";
	
	// 2. 创建Shader object
	// 顶点着色器
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderId, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShaderId);
	GLint compileStatus = 0;
	glGetShaderiv(vertexShaderId, GL_COMPILE_STATUS, &compileStatus); // 检查编译状态
	if (compileStatus == GL_FALSE) // 获取错误报告
	{
		GLint maxLength = 0;
		glGetShaderiv(vertexShaderId, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> errLog(maxLength);
		glGetShaderInfoLog(vertexShaderId, maxLength, &maxLength, &errLog[0]);
		std::cout << "Error::shader vertex shader compile failed," << &errLog[0] << std::endl;
	}
	// 片元着色器
	GLuint fragShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragShaderId, 1, &fragShaderSource, NULL);
	glCompileShader(fragShaderId);
	glGetShaderiv(fragShaderId, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(fragShaderId, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> errLog(maxLength);
		glGetShaderInfoLog(fragShaderId, maxLength, &maxLength, &errLog[0]);
		std::cout << "Error::shader fragment shader compile failed," << &errLog[0] << std::endl;
	}
	// 3.链接形成 shader program object
	GLuint shaderProgramId = glCreateProgram();
	glAttachShader(shaderProgramId, vertexShaderId);
	glAttachShader(shaderProgramId, fragShaderId);
	glLinkProgram(shaderProgramId);
	GLint linkStatus;
	glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &linkStatus);
	if (linkStatus == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(shaderProgramId, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<GLchar> errLog(maxLength);
		glGetProgramInfoLog(shaderProgramId, maxLength, &maxLength, &errLog[0]);
		std::cout << "Error::shader link failed," << &errLog[0] << std::endl;
	}
	// 链接完成后detach
	glDetachShader(shaderProgramId, vertexShaderId);
	glDetachShader(shaderProgramId, fragShaderId);
	// 不需要连接到其他程序时，释放空间
	glDeleteShader(vertexShaderId);
	glDeleteShader(fragShaderId);

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
		glUseProgram(shaderProgramId);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);
		glUseProgram(0);

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}

	// 释放资源
	glDeleteProgram(shaderProgramId);
	glDeleteVertexArrays(1, &VAOId);
	glDeleteBuffers(1, &VBOId);

	// Terminate GLFW, clearing any resources allocated by GLFW.
	glfwTerminate();
	return 0;
}
