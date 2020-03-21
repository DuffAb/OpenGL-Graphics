#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>

class Shader
{
public:
	// Constructor generates the shader on the fly
	GLuint createShader(GLuint shaderType, const GLchar* Path)
	{
		// 1. Retrieve the vertex/fragment source code from filePath
		std::string shaderCode;
		std::ifstream vShaderFile;
		// ensures ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::badbit);
		try
		{
			// Open files
			vShaderFile.open(Path);
			std::stringstream vShaderStream;
			// Read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			// Convert stream into string
			shaderCode = vShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const GLchar* vShaderCode = shaderCode.c_str();
		// 2. Compile shaders
		GLuint shaderId;
		GLint success;
		GLchar infoLog[512];
		// Vertex Shader
		shaderId = glCreateShader(shaderType);
		glShaderSource(shaderId, 1, &vShaderCode, NULL);
		glCompileShader(shaderId);
		// Print compile errors if any
		glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		return shaderId;
	}

	void linkProgram()
	{
		// Shader Program
		this->Program = glCreateProgram();
		glAttachShader(this->Program, vertex);
		glAttachShader(this->Program, fragment);
		glLinkProgram(this->Program);
		// Print linking errors if any
		GLint success;
		GLchar infoLog[512];
		glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
		// Delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath)
	{
		vertex = createShader(GL_VERTEX_SHADER, vertexPath);
		fragment = createShader(GL_FRAGMENT_SHADER, fragmentPath);
		linkProgram();
	}

	Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath)
	{
		// 1. Retrieve the vertex/fragment source code from filePath
		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		std::ifstream gShaderFile;
		// ensures ifstream objects can throw exceptions:
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			// Open files
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			gShaderFile.open(geometryPath);
			std::stringstream vShaderStream, fShaderStream, gShaderStream;
			// Read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			gShaderStream << gShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			gShaderFile.close();
			// Convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
			geometryCode = gShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}
		const GLchar* vShaderCode = vertexCode.c_str();
		const GLchar* fShaderCode = fragmentCode.c_str();
		const GLchar* gShaderCode = geometryCode.c_str();
		// 2. Compile shaders
		GLuint vertex, fragment, geometry;
		GLint success;
		GLchar infoLog[512];
		// Vertex Shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		// Print compile errors if any
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		// Fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		// Print compile errors if any
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		// Geometry Shader
		geometry = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometry, 1, &gShaderCode, NULL);
		glCompileShader(geometry);
		// Print compile errors if any
		glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(geometry, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
		}
		// Shader Program
		this->Program = glCreateProgram();
		glAttachShader(this->Program, vertex);
		glAttachShader(this->Program, fragment);
		glAttachShader(this->Program, geometry);
		glLinkProgram(this->Program);
		// Print linking errors if any
		glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
		}
		// Delete the shaders as they're linked into our program now and no longer necessery
		glDeleteShader(vertex);
		glDeleteShader(fragment);
		glDeleteShader(geometry);

	}
	// Uses the current shader
	void Use()
	{
		glUseProgram(this->Program);
	}

	void setBool(const std::string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(this->Program, name.c_str()), (int)value);
	}
	void setInt(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(this->Program, name.c_str()), value);
	}
	void setFloat(const std::string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(this->Program, name.c_str()), value);
	}
	void setMatrix4fv(const std::string &name, const GLfloat* value)
	{
		GLint transformLoc = glGetUniformLocation(this->Program, name.c_str());
		glUniformMatrix4fv(transformLoc, //uniform的位置值
			1,			//告诉OpenGL我们将要发送多少个矩阵
			GL_FALSE,	//是否希望对我们的矩阵进行置换(Transpose)，也就是说交换我们矩阵的行和列
			value);		//真正的矩阵数据
	}

	void setVec3f(const std::string &name, GLfloat v0, GLfloat v1, GLfloat v2)
	{
		GLint location = glGetUniformLocation(this->Program, name.c_str());
		glUniform3f(location, v0, v1, v2);
	}

	void setVec3fv(const std::string &name, const GLfloat* value)
	{
		GLint location = glGetUniformLocation(this->Program, name.c_str());
		glUniform3fv(location, 1, value);
	}

	GLint getUniformLocation(const std::string &name)
	{
		return glGetUniformLocation(this->Program, name.c_str());
	}

	void getActiveUniform(GLuint uniformCount)
	{
		GLsizei namelen;
		GLint size = 0;
		GLenum type;
		GLchar name[32];
		for (GLuint i = 0; i < (GLuint)uniformCount; i++)
		{
			
			glGetActiveUniform(this->Program, i, sizeof(name), &namelen, &size, &type, name);

			/*if (size)
			{
				int l = glGetUniformLocation(this->Program, name);
				char *np = name;
				while (*np)
				{
					if (*np == '[')
						*np = 0;
					np++;
				}
				Uniform u;
				u.Name = name;
				u.Location = l;
				u.Size = size;
				switch (type)
				{
				case GL_FLOAT:      u.Type = 1; break;
				case GL_FLOAT_VEC2: u.Type = 2; break;
				case GL_FLOAT_VEC3: u.Type = 3; break;
				case GL_FLOAT_VEC4: u.Type = 4; break;
				case GL_FLOAT_MAT3: u.Type = 12; break;
				case GL_FLOAT_MAT4: u.Type = 16; break;
				default:
					continue;
				}
				UniformInfo.PushBack(u);
				if (!strcmp(name, "LightCount"))
					UsesLighting = 1;
			}
			else
				break;*/
		}
	}

private:
	GLuint Program;
	GLuint vertex;
	GLuint fragment;
};

#endif