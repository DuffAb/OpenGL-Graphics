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
// 立方体辅助类
#include "cube.h"
// 矩形面片辅助类
#include "quad.h"
// 计时器辅助类
#include "timer.h"
// 视频辅助类
#include "video.h"


GLuint PBOIds[2];
void preparePBO();
// 图片参数
const int    IMAGE_WIDTH = WIDTH;
const int    IMAGE_HEIGHT = HEIGHT;
const int DATA_SIZE = IMAGE_WIDTH * IMAGE_HEIGHT * 4;
const int    TEXT_WIDTH = 8;
const int    TEXT_HEIGHT = 30;
const GLenum PIXEL_FORMAT = GL_RGBA;
void updatePixels(GLubyte* dst, int size);
GLubyte* imageData = 0; // 图片数据
void initImageData();
void releaseImageData();

//视频渲染对象
VideoObject videoObject;

int pboMode = 2;
Timer timer, t1, t2;
float copyTime, updateTime;
GLuint textureId;
void initTexture();

void renderInfo(Shader& shader);
void renderScene(Shader& shader);
void printTransferRate();
std::map<wchar_t, FontCharacter> unicodeCharacters;

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
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Demo of render video", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetWindowPos(window, 300, 100);
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

	// Section1 准备顶点数据
	FontHelper::getInstance().prepareTextVBO();	//准备渲染字体的顶点数据空间
	QuadHelper::getInstance().prepareQuadVBO(); //准备渲染矩形面片的顶点数据
	preparePBO();
	initImageData();
	initTexture();
	std::srand(std::time(0));
	// Section2 加载字体
	FontHelper::getInstance().loadFont("arial", "resources/fonts/arial.ttf");
	FontHelper::getInstance().loadASCIIChar("arial", 38);

	// Section3 加载视频
	const std::string fileName = "resources/videos/movie.wmv";
	if (!videoObject.openVideo(fileName, IMAGE_WIDTH, IMAGE_HEIGHT))
	{
		std::cerr << " failed to open video file: " << fileName << std::endl;
		char waitKey;
		std::cin >> waitKey;
		return -1;
	}
	// Section3 准备着色器程序
	Shader shader("shader/videoRendering/videoRendering/scene.vertex", "shader/videoRendering/videoRendering/scene.frag");
	Shader textShader("shader/videoRendering/videoRendering/text.vertex", "shader/videoRendering/videoRendering/text.frag");

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// 开始游戏主循环
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = (GLfloat)glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		glfwPollEvents(); // 处理例如鼠标 键盘等事件
		do_movement(); // 根据用户操作情况 更新相机属性

		// 清除颜色缓冲区 重置为指定颜色
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(camera.mouse_zoom, (GLfloat)(width) / height, 1.0f, 100.0f); // 投影矩阵
		glm::mat4 view = camera.getViewMatrix(); // 视变换矩阵
		glm::mat4 model;
		// 这里填写场景绘制代码
		// 先绘制纹理图片
		shader.use();
		shader.updateUniformMatrix4fv("projection",	1, GL_FALSE, glm::value_ptr(projection));
		shader.updateUniformMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
		model = glm::mat4();
		shader.updateUniformMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
		renderScene(shader);
		// 在绘制信息文字
 		textShader.use();
 		projection = glm::ortho(0.0f, (GLfloat)(width), 0.0f, (GLfloat)height);
 		view = glm::mat4();
		textShader.updateUniformMatrix4fv("projection",	1, GL_FALSE, glm::value_ptr(projection));
		textShader.updateUniformMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
 		model = glm::mat4();
		textShader.updateUniformMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
 		renderInfo(textShader);

		printTransferRate();

		glBindVertexArray(0);
		glUseProgram(0);
		glfwSwapBuffers(window); // 交换缓存
	}
	// 释放资源
	
	
	glDeleteBuffers(2, PBOIds);  // 注意释放PBO
	releaseImageData();

	glfwTerminate();
	return 0;
}

// 准备PBO
void preparePBO()
{
	glGenBuffers(2, PBOIds);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBOIds[0]);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBOIds[1]);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, NULL, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

// 写入视频到纹理中去
void updatePixels(GLubyte* dst, int size)
{
	if (!dst)
		return;
	if (!videoObject.getNextFrame(dst, size))
	{
		std::cerr << "Error failed to get next frame" << std::endl;
	}
}


void renderScene(Shader& shader)
{
	static int index = 0;				// 用于从PBO拷贝像素到纹理对象
	int nextIndex = 0;                  // 指向下一个PBO 用于更新PBO中像素
	glActiveTexture(GL_TEXTURE0);
	if (pboMode > 0)
	{
		if (pboMode == 1)
		{
			// 只有一个时 使用0号PBO
			index = nextIndex = 0;
		}
		else if (pboMode == 2)
		{
			index = (index + 1) % 2;
			nextIndex = (index + 1) % 2;
		}

		// 开始PBO到texture object的数据复制 unpack操作
		t1.start();

		// 绑定纹理 和PBO
		glBindTexture(GL_TEXTURE_2D, textureId);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBOIds[index]);

		// 从PBO复制到texture object 使用偏移量 而不是指针
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, PIXEL_FORMAT, GL_UNSIGNED_BYTE, 0);

		// 计算复制数据所需时间
		t1.stop();
		copyTime = t1.getElapsedTimeInMilliSec();


		// 开始修改nextIndex指向的PBO的数据
		t1.start();

		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, PBOIds[nextIndex]);
		glBufferData(GL_PIXEL_UNPACK_BUFFER, DATA_SIZE, 0, GL_STREAM_DRAW);
		// 从PBO映射到用户内存空间 然后修改PBO中数据
		GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
		if (ptr)
		{
			// 更新映射后的内存数据
			updatePixels(ptr, DATA_SIZE);
			glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER); // 释放映射的用户内存空间
		}

		// 计算修改PBO数据所需时间
		t1.stop();
		updateTime = t1.getElapsedTimeInMilliSec();

		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	}
	else
	{
		// 不使用PBO的方式 从用户内存复制到texture object
		t1.start();

		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, PIXEL_FORMAT, GL_UNSIGNED_BYTE, (GLvoid*)imageData);

		t1.stop();
		copyTime = t1.getElapsedTimeInMilliSec();

		// 修改内存数据
		t1.start();
		updatePixels(imageData, DATA_SIZE);
		t1.stop();
		updateTime = t1.getElapsedTimeInMilliSec();
	}
	shader.updateUniform1i("randomText", 0);

	// 绘制多个矩形显示纹理
	glBindVertexArray(QuadHelper::getInstance().getVAO());
	glm::vec2  positions[] = {
		glm::vec2(-8, 8),
		glm::vec2(0.0, 8),
		glm::vec2(8, 8),

		glm::vec2(-8, 2),
		glm::vec2(0.0, 2),
		glm::vec2(8, 2),

		glm::vec2(-8, 0.0f),
		glm::vec2(0.0, 0.0f),
		glm::vec2(8, 0.0f),

		glm::vec2(-8, -2),
		glm::vec2(0.0, -2),
		glm::vec2(8, -2),

		glm::vec2(-8, -8),
		glm::vec2(0.0, -8),
		glm::vec2(8, -8),
	};
	const double FAR_Z_POS = -30.0f;
	static double zPos[15] = { FAR_Z_POS, FAR_Z_POS, FAR_Z_POS,
		FAR_Z_POS, FAR_Z_POS, FAR_Z_POS ,
		FAR_Z_POS, FAR_Z_POS, FAR_Z_POS };
	for (size_t i = 0; i < sizeof(positions) / sizeof(positions[0]); ++i)
	{
		glm::mat4 model = glm::mat4();
		zPos[i] += (std::rand() % 10) * 0.1f;
		if (zPos[i] - 0.1f >= 0.0001f)
		{
			zPos[i] = FAR_Z_POS;
		}
		glm::vec3 pos = glm::vec3(positions[i].x, positions[i].y, zPos[i]);
		model = glm::translate(model, pos);
		model = glm::scale(model, glm::vec3(1.4));
		shader.updateUniformMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	glBindVertexArray(0);
}

void renderInfo(Shader& shader)
{
	float color[4] = { 1, 1, 1, 1 };

	std::wstringstream ss;
	ss << "PBO: ";
	if (pboMode == 0)
		ss << "off" << std::ends;
	else if (pboMode == 1)
		ss << "1 PBO" << std::ends;
	else if (pboMode == 2)
		ss << "2 PBOs" << std::ends;
	FontHelper::getInstance().renderText(shader, ss.str(), 1, WIDTH - TEXT_HEIGHT, 0.4f, glm::vec3(0.0f, 0.0f, 1.0f));
	ss.str(L"");

	ss << std::fixed << std::setprecision(3);
	ss << "Updating Time: " << updateTime << " ms" << std::ends;
	FontHelper::getInstance().renderText(shader, ss.str(), 1, HEIGHT - (2 * TEXT_HEIGHT), 0.4f, glm::vec3(0.0f, 0.0f, 1.0f));
	ss.str(L"");

	ss << "Copying Time: " << copyTime << " ms" << std::ends;
	FontHelper::getInstance().renderText(shader, ss.str(), 1, HEIGHT - (3 * TEXT_HEIGHT), 0.4f, glm::vec3(0.0f, 0.0f, 1.0f));
	ss.str(L"");

	ss << "Press SPACE key to toggle PBO on/off." << std::ends;
	FontHelper::getInstance().renderText(shader, ss.str(), 1, 1, 0.8f, glm::vec3(0.0f, 0.0f, 1.0f));

	ss << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
}

void printTransferRate()
{
	const double INV_MEGA = 1.0 / (1024 * 1024);
	static Timer timer;
	static int count = 0;
	static std::stringstream ss;
	double elapsedTime;

	// 循环直到1s时间到
	elapsedTime = timer.getElapsedTime();
	if (elapsedTime < 1.0)
	{
		++count;
	}
	else
	{
		std::cout << std::fixed << std::setprecision(1);
		std::cout << "Transfer Rate: " << (count / elapsedTime) * DATA_SIZE * INV_MEGA << " MB/s. (" << count / elapsedTime << " FPS)\n";
		std::cout << std::resetiosflags(std::ios_base::fixed | std::ios_base::floatfield);
		count = 0;                      // 重置计数器
		timer.start();                  // 重新开始计时器
	}
}

void initImageData()
{
	imageData = new GLubyte[DATA_SIZE];
	memset(imageData, 0, DATA_SIZE);
}

void initTexture()
{
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, IMAGE_WIDTH, IMAGE_HEIGHT, 0, PIXEL_FORMAT, GL_UNSIGNED_BYTE, (GLvoid*)imageData);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void releaseImageData()
{
	delete[] imageData;
}