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

#include "pbo.h"


GLuint cubeTextId;
void drawRightSidePixel(Shader& shader);
void drawLeftSidePixel(Shader& shader);
// 图片参数
const int    SINGLE_SCREEN_WIDTH = WIDTH / 2;
const int    SINGLE_SCREEN_HEIGHT = HEIGHT;
const int	 DATA_SIZE = SINGLE_SCREEN_WIDTH * SINGLE_SCREEN_HEIGHT * 4; //*4是因为 GL_BGRA
const int    TEXT_WIDTH = 8;
const int    TEXT_HEIGHT = 30;
void updatePixels(unsigned char* src, int width, int height, unsigned char* dst);

Timer timer, t1;
float readTime, processTime;

void renderInfo(Shader& shader);
bool isFirstRendered = false;
void renderInitScene(Shader& shader);
void renderScene(Shader& shader);

void printTransferRate();
PboHelper bh(SINGLE_SCREEN_WIDTH, SINGLE_SCREEN_HEIGHT);
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
	QuadHelper::getInstance().prepareQuadVBO();//准备四方形面片顶点数据
	FontHelper::getInstance().prepareTextVBO();//准备字体渲染顶点数据空间
	bh.preparePBO();

	// Section2 加载字体
	FontHelper::getInstance().loadFont("arial", "resources/fonts/arial.ttf");
	FontHelper::getInstance().loadASCIIChar("arial", 38);

	// Section3 加载纹理
	cubeTextId = TextureHelper::load2DTexture("resources/textures/wood.png");
	bh.initPixelTexture();

	// Section4 准备着色器程序
	Shader shader("shader/PBO/PBO-pack/scene.vertex", "shader/PBO/PBO-pack/scene.frag");
	Shader textShader("shader/PBO/PBO-pack/text.vertex", "shader/PBO/PBO-pack/text.frag");

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
		glfwPollEvents();	// 处理例如鼠标 键盘等事件
		do_movement();		// 根据用户操作情况 更新相机属性

		// 清除颜色缓冲区 重置为指定颜色
		glClearColor(0.18f, 0.04f, 0.14f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection;
		glm::mat4 view;
		glm::mat4 model;
		// 这里填写场景绘制代码
		// 先绘制纹理图片
		shader.use();
		shader.updateUniformMatrix4fv("projection",	1, GL_FALSE, glm::value_ptr(projection));
		shader.updateUniformMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
		model = glm::mat4();
		shader.updateUniformMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
		if (!isFirstRendered)
		{
			renderInitScene(shader);
			isFirstRendered = true;
		}
		else
		{
			renderScene(shader);
		}
		// 在右侧绘制信息文字 否则左侧文字会复制到右侧
		textShader.use();
		projection = glm::ortho(0.0f, (GLfloat)(SINGLE_SCREEN_WIDTH), 0.0f, (GLfloat)SINGLE_SCREEN_HEIGHT);
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
	glfwTerminate();
	return 0;
}

// 改变像素的亮度
void updatePixels(unsigned char* src, int width, int height, unsigned char* dst)
{
	if (!src || !dst)
		return;
	static int shift = 0;
	shift = ++shift % 200;

	int value;
	for (int i = 0; i < height; ++i)
	{
		for (int j = 0; j < width; ++j)
		{
			value = *src + shift;
			if (value > 255) *dst = (unsigned char)255;
			else            *dst = (unsigned char)value;
			++src;
			++dst;

			value = *src + shift;
			if (value > 255) *dst = (unsigned char)255;
			else            *dst = (unsigned char)value;
			++src;
			++dst;

			value = *src + shift;
			if (value > 255) *dst = (unsigned char)255;
			else            *dst = (unsigned char)value;
			++src;
			++dst;

			++src;    // 跳过alpha
			++dst;
		}
	}
}

// 绘制一个初始场景 保证当FBO里使用glReadPixels时有内容
void renderInitScene(Shader& shader)
{
	// 绘制左边正方形
	drawLeftSidePixel(shader);
	// 绘制右边正方形
	drawRightSidePixel(shader);
}
void drawLeftSidePixel(Shader& shader)
{
	glViewport(0, 0, SINGLE_SCREEN_WIDTH, SINGLE_SCREEN_HEIGHT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cubeTextId);
	glBindVertexArray(QuadHelper::getInstance().getVAO());
	shader.updateUniform1i("cubeText", 0);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
void drawRightSidePixel(Shader& shader)
{
	glViewport(SINGLE_SCREEN_WIDTH, 0, SINGLE_SCREEN_WIDTH, SINGLE_SCREEN_HEIGHT);
	bh.pbo_draw(shader);
	
}
void renderScene(Shader& shader)
{	
	if (pboUsed)
	{
		t1.start();
		// 开始FBO到PBO复制操作 pack操作
		bh.async_read_pixels();
		
		// 计算读取数据所需时间
		t1.stop();
		readTime = t1.getElapsedTimeInMilliSec();

		// 开始修改nextIndex指向的PBO的数据
		t1.start();
		bh.update_pbo_pixels(updatePixels);
		// 计算更新PBO数据所需时间
		t1.stop();
		processTime = t1.getElapsedTimeInMilliSec();
	}
	else
	{
		// 不使用PBO的方式 读取到client memory并修改
		t1.start();
		bh.sync_read_pixels_and_update(updatePixels);
		t1.stop();
		readTime = t1.getElapsedTimeInMilliSec();
	}
	glDrawBuffer(GL_BACK);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawLeftSidePixel(shader);
	drawRightSidePixel(shader);
}

void renderInfo(Shader& shader)
{
	float color[4] = { 1, 1, 1, 1 };

	std::wstringstream ss;
	ss << "PBO: ";
	if (pboUsed)
		ss << "on" << std::ends;
	else
		ss << "off" << std::ends;

	FontHelper::getInstance().renderText(shader, ss.str(), 1, HEIGHT - TEXT_HEIGHT, 0.4f, glm::vec3(0.0f, 0.0f, 1.0f));
	ss.str(L"");

	ss << std::fixed << std::setprecision(3);
	ss << "Read Time: " << readTime << " ms" << std::ends;
	FontHelper::getInstance().renderText(shader, ss.str(), 1, HEIGHT - (2 * TEXT_HEIGHT), 0.4f, glm::vec3(0.0f, 0.0f, 1.0f));
	ss.str(L"");

	ss << "Process Time: " << processTime << " ms" << std::ends;
	FontHelper::getInstance().renderText(shader, ss.str(), 1, HEIGHT - (3 * TEXT_HEIGHT), 0.4f, glm::vec3(0.0f, 0.0f, 1.0f));
	ss.str(L"");

	ss << "Press SPACE key to toggle PBO on/off." << std::ends;
	FontHelper::getInstance().renderText(shader, ss.str(), 1, 1, 0.6f, glm::vec3(0.0f, 0.0f, 1.0f));

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