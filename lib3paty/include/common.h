#ifndef COMMON_H_
#define COMMON_H_

#define GLEW_STATIC
#include "GL/glew.h"

// GLFW
#include "GLFW/glfw3.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef _DEBUG
#pragma comment(lib, "glew32sd.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glfw3.lib")
#pragma comment(lib, "SOIL.lib")
#pragma comment(lib, "assimp-vc140-mt.lib")
#else
#pragma comment(lib, "glew32sd.lib")
#endif

#include "Shader.h"
#include "Camera.h"

namespace xgvr
{
	// Window dimensions
	const GLuint WIDTH = 1440;
	const GLuint HEIGHT = 810;
	
	// Camera
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	GLfloat yaw = -90.0f;	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
	GLfloat pitch = 0.0f;
	GLfloat lastX = WIDTH / 2.0;
	GLfloat lastY = HEIGHT / 2.0;
	GLfloat fov = 45.0f;
	bool keys[1024];

	// Camera
	Camera  camera(glm::vec3(0.0f, 0.0f, 3.0f));

	GLfloat deltaTime = 0.0f;   // 当前帧遇上一帧的时间差
	GLfloat lastFrame = 0.0f;   // 上一帧的时间

	// Light attributes
	glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

	class xgvrShader
	{
	public:
		xgvrShader(const GLchar* vertexShaderSource, const GLchar* fragmentShaderSource)
		{
			shaderProgram = 0;
			createUserShader(vertexShaderSource, fragmentShaderSource);
		}
		void createUserShader(const GLchar* vertexShaderSource, const GLchar* fragmentShaderSource)
		{
			// Vertex shader
			GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
			glCompileShader(vertexShader);
			// Check for compile time errors
			GLint success;
			GLchar infoLog[512];
			glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
			}
			// Fragment shader
			GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
			glCompileShader(fragmentShader);
			// Check for compile time errors
			glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
			}
			// Link shaders
			shaderProgram = glCreateProgram();
			glAttachShader(shaderProgram, vertexShader);
			glAttachShader(shaderProgram, fragmentShader);
			glLinkProgram(shaderProgram);
			// Check for linking errors
			glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
				std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
			}
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);
		}

		GLint GetUniformLocation(const GLchar* name)
		{
			return glGetUniformLocation(shaderProgram, name);
		}

		void useShader()
		{
			glUseProgram(shaderProgram);
		}
	protected:
	private:
		GLuint shaderProgram;
	};

	GLint createTexture(GLsizei width, GLsizei height, const void *pixels)
	{
		GLuint textureId;
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId); // All upcoming GL_TEXTURE_2D operations now have effect on our texture object
		// Set our texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// Set texture filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
		return textureId;
	}
	
	//查询硬件支持的顶点属性的上限
	GLint getVertexAttributeCount()
	{
		GLint nrAttributes;
		glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
		return nrAttributes;
	}
	
	GLint getIntegerv(GLuint type)
	{
		GLint num;
		glGetIntegerv(type, &num);
		return num;
	}

	

	//To know the max width and height of a 1D or 2D texture that your GPU supports
	//获取GPU支持的1D或2D纹理的最大宽度和高度
	GLint getMaxTexture2DSize()
	{
		GLint value;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &value);   //Returns 1 value
		//MaxTexture2DWidth = MaxTexture2DHeight = value;
		return value;
	}

	//To know the max width, height, depth of a 3D texture that your GPU supports
	//获取GPU支持的3D纹理的最大宽度，高度和深度
	GLint getMaxTexture3DSize()
	{
		GLint value;
		glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &value);
		//MaxTexture3DWidth = MaxTexture3DHeight = MaxTexture3DDepth = value;
		return value;
	}

	//To know the max width and height of a a cubemap texture that your GPU supports
	//获取GPU支持的立方体贴图纹理的最大宽度和高度
	GLint getMaxTextureCubemapSize()
	{
		GLint value;
		glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &value);
		//MaxTextureCubemapWidth = MaxTextureCubemapHeight = value;
		return value;
	}

	//The function return a value such as 16 or 32 or above. That is the number of image samplers that your GPU supports in the fragment shader.
	//该函数返回诸如16或32或更高的值。这是GPU在片段着色器中支持的图像采样器数量。
	GLint getMaxTextureImageUnits()
	{
		GLint MaxTextureImageUnits;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &MaxTextureImageUnits);
		return MaxTextureImageUnits;
	}

	//The function return a value such as 16 or 32 or above. That is the number of image samplers that your GPU supports in the vertex shader.
	//该函数返回诸如16或32或更高的值，这是GPU在顶点着色器中支持的图像采样器数量，某些GPU可能会返回 0。
	GLint getMaxVertexTextureImageUnits()
	{
		GLint MaxVertexTextureImageUnits;
		glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &MaxVertexTextureImageUnits);
		return MaxVertexTextureImageUnits;
	}

	//The function return a value such as 16 or 32 or above. That is the number of image samplers that your GPU supports in the geometry shader.
	//该函数返回诸如16或32或更高的值，这是GPU在几何着色器中支持的图像采样器数量。
	GLint getMaxGSGeometryTextureImageUnits()
	{
		GLint MaxGSGeometryTextureImageUnits;
		glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &MaxGSGeometryTextureImageUnits);
		return MaxGSGeometryTextureImageUnits;
	}

	//VS + GS + FS
	GLint getMaxCombinedTextureImageUnits()
	{
		GLint MaxCombinedTextureImageUnits;
		glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &MaxCombinedTextureImageUnits);
		return MaxCombinedTextureImageUnits;
	}

	//Return the number of texture coordinates available which usually is 8
	//可用的纹理坐标数，通常为8
	GLint getMaxTextureCoords()
	{
		GLint MaxTextureCoords;
		glGetIntegerv(GL_MAX_TEXTURE_COORDS, &MaxTextureCoords);
		return MaxTextureCoords;
	}

	//查询允许的颜色附件数
	GLint getMaxColorAttchments()
	{
		GLint MaxColorAttchments;
		glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &MaxColorAttchments);
		return MaxColorAttchments;
	}

	void framebuffer_size_callback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	// Is called whenever a key is pressed/released via GLFW
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
	{
		// 当用户按下ESC键,我们设置window窗口的WindowShouldClose属性为true
		// 关闭应用程序
		std::cout << key << std::endl;
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);

		if (key >= 0 && key < 1024)
		{
			if (action == GLFW_PRESS)
				keys[key] = true;
			else if (action == GLFW_RELEASE)
				keys[key] = false;
		}
	}

	void do_movement()
	{
		// Camera controls
		GLfloat cameraSpeed = 5.0f * deltaTime;
		if (keys[GLFW_KEY_W])
			cameraPos += cameraSpeed * cameraFront;
		if (keys[GLFW_KEY_S])
			cameraPos -= cameraSpeed * cameraFront;
		if (keys[GLFW_KEY_A])
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (keys[GLFW_KEY_D])
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}

	void do_movement_camera()
	{
		// Camera controls
		if (keys[GLFW_KEY_W])
			camera.ProcessKeyboard(FORWARD, deltaTime);
		if (keys[GLFW_KEY_S])
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (keys[GLFW_KEY_A])
			camera.ProcessKeyboard(LEFT, deltaTime);
		if (keys[GLFW_KEY_D])
			camera.ProcessKeyboard(RIGHT, deltaTime);
	}

	bool firstMouse = true;
	void mouse_callback(GLFWwindow* window, double xpos, double ypos)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		GLfloat xoffset = xpos - lastX;
		GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to left
		lastX = xpos;
		lastY = ypos;

		GLfloat sensitivity = 0.05;	// Change this value to your liking
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(front);
	}

	//void mouse_callback_camera(GLFWwindow* window, double xpos, double ypos)
	//{
	//	if (firstMouse)
	//	{
	//		lastX = xpos;
	//		lastY = ypos;
	//		firstMouse = false;
	//	}

	//	GLfloat xoffset = xpos - lastX;
	//	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	//	lastX = xpos;
	//	lastY = ypos;

	//	camera.ProcessMouseMovement(xoffset, yoffset);
	//}

	void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{
		if (fov >= 1.0f && fov <= 45.0f)
			fov -= yoffset * 0.05;
		if (fov <= 1.0f)
			fov = 1.0f;
		if (fov >= 45.0f)
			fov = 45.0f;
	}

	/*void scroll_callback_camera(GLFWwindow* window, double xoffset, double yoffset)
	{
		camera.ProcessMouseScroll(yoffset);
	}*/
}
#endif /*COMMON_H_*/
