#ifndef _FONT_H_
#define _FONT_H_

// 引入GLEW库 定义静态链接
#define GLEW_STATIC
#include <GL/glew.h>
#include <ft2build.h>
#include <GLM/glm.hpp>
#include <map>
#include <string>
#include <iostream>

#include FT_FREETYPE_H

struct FontCharacter
{
	GLuint textureId;	// 存储字形纹理
	glm::ivec2 size;	// 字形大小
	glm::ivec2 bearing;	// bearingx bearingy
	GLuint advance;		// 距离下个字形的水平距离
};

class FontHelper
{
private:
	typedef std::map<std::string, FT_Face> FontFaceMapType;

public:
	static FontHelper& getInstance()
	{
		static FontHelper instance;
		return instance;
	}
	~FontHelper()
	{
		for (FontFaceMapType::const_iterator it = _faceMap.begin(); it != _faceMap.end(); ++it)
		{
			if (it->second)
			{
				FT_Done_Face(it->second);
			}
		}
		FT_Done_FreeType(_ft);

		glDeleteVertexArrays(1, &textVAOId);
		glDeleteBuffers(1, &textVBOId);
	}

private:
	FontHelper()
	{
		if (FT_Init_FreeType(&_ft))
		{
			std::cerr << "ERROR::FontHelper: Could not init FreeType Library" << std::endl;
		}
	}
	FontHelper(FontHelper const&) {} // 单例模式不实现
	void operator=(FontHelper const&) {} // 单例模式不实现

public:
	void prepareTextVBO()
	{
		glGenVertexArrays(1, &textVAOId);
		glGenBuffers(1, &textVBOId);
		glBindVertexArray(textVAOId);
		glBindBuffer(GL_ARRAY_BUFFER, textVBOId);
		// 绘制文字的矩形顶点属性数据 位置 纹理 是动态计算出来的
		// 这里预分配空间
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		// xy 表示位置 zw表示纹理坐标
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	}

	void renderText(Shader& shader, std::wstring text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color)
	{
		shader.use();
		shader.updateUniform3f("textColor", color.x, color.y, color.z);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(textVAOId);

		// 遍历绘制字符串中字符
		std::wstring::const_iterator c;
		for (c = text.begin(); c != text.end(); c++)
		{
			if (characters.count(*c) <= 0)
			{
				continue;
			}
			FontCharacter ch = characters[*c];

			GLfloat xpos = x + ch.bearing.x * scale;
			GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

			GLfloat w = ch.size.x * scale;
			GLfloat h = ch.size.y * scale;
			// 计算这个字形对应的矩形位置数据
			GLfloat vertices[6][4] = {
				{ xpos,     ypos + h, 0.0, 0.0 },
				{ xpos,     ypos,     0.0, 1.0 },
				{ xpos + w, ypos,     1.0, 1.0 },

				{ xpos,     ypos + h, 0.0, 0.0 },
				{ xpos + w, ypos,     1.0, 1.0 },
				{ xpos + w, ypos + h, 1.0, 0.0 }
			};
			// 绑定这个字符对应的纹理
			glBindTexture(GL_TEXTURE_2D, ch.textureId);
			// 动态设置VBO中数据 因为这个表示文字的矩形位置数据有变动 需要动态调整
			glBindBuffer(GL_ARRAY_BUFFER, textVBOId);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// 因为advance 以1/64像素表示距离单位 所以这里移位后表示一个像素距离
			x += (ch.advance >> 6) * scale;
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void loadFont(const std::string& fontName, const std::string& fontPath)
	{
		if (NULL != getFontFace(fontName))
		{
			return;
		}
		FT_Face face;
		if (FT_New_Face(_ft, fontPath.c_str(), 0, &face))
		{
			std::cerr << "ERROR::FontResourceManager: Failed to load font with name = " << fontPath << std::endl;
			return;
		}
		_faceMap[fontName] = face;
		FT_Select_Charmap(face, FT_ENCODING_UNICODE); // unicode编码
	}
	bool loadASCIIChar(const std::string& fontName, const int fontSize)
	{
		// 因为我们使用1个字节的颜色分量来存储纹理 所以解除默认的4字节对齐限制
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		FT_Face face = getFontFace(fontName);
		if (NULL == face)
		{
			std::cerr << "ERROR::FontResourceManager: Failed to get font with name = " << fontName << std::endl;
			return false;
		}
		FT_Set_Pixel_Sizes(face, 0, fontSize);
		for (GLubyte c = 0; c < 255; c++)
		{
			if (!isprint(c))
			{
				continue;
			}
			// 加载字符的字形 
			// FT_LOAD_RENDER  选项告知freeType创建一个 8-bit grayscale bitmap image
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cerr << "ERROR::FontResourceManager: Failed to load Glyph with char=" << c << std::endl;
				continue;
			}
			// 为字体对应的字形创建一个纹理 保存以备后续处理
			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			FontCharacter character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			characters.insert(std::pair<GLchar, FontCharacter>(c, character));
		}
		glBindTexture(GL_TEXTURE_2D, 0);
		return true;
	}
	bool loadUnicodeText(const std::string& fontName, const int fontSize, const std::wstring& text)
	{
		// 因为我们使用1个字节的颜色分量来存储纹理 所以解除默认的4字节对齐限制
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		FT_Face face = getFontFace(fontName);
		if (NULL == face)
		{
			std::cerr << "ERROR::FontResourceManager: Failed to get font with name = " << fontName << std::endl;
			return false;
		}
		FT_Set_Pixel_Sizes(face, 0, fontSize);
		for (std::wstring::const_iterator it = text.begin(); it != text.end(); ++it)
		{
			wchar_t wchar = *it;
			// 加载字符的字形 
			// FT_LOAD_RENDER  选项告知freeType创建一个 8-bit grayscale bitmap image
			if (FT_Load_Char(face, wchar, FT_LOAD_RENDER))
			{
				std::cerr << "ERROR::FontResourceManager: Failed to load Glyph with char=" << wchar << std::endl;
				continue;
			}
			// 为字体对应的字形创建一个纹理 保存以备后续处理
			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			FontCharacter character = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			characters.insert(std::pair<wchar_t, FontCharacter>(wchar, character));
		}
		glBindTexture(GL_TEXTURE_2D, 0);
		return true;
	}
private:
	FT_Face getFontFace(const std::string& fontName)
	{
		if (_faceMap.count(fontName) > 0)
		{
			return _faceMap[fontName];
		}
		return NULL;
	}
private:
	FT_Library _ft;
	FontFaceMapType _faceMap;

	GLuint textVAOId, textVBOId;
	std::map<wchar_t, FontCharacter> characters;
};


#endif // _FONT_H_

