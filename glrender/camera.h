#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <iostream>
#include <fstream>
#include <iomanip>      // std::setprecision

#include "common.h"
#include "math.h"

// 定义移动方向
enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};
// 定义预设常量
const GLfloat YAW = -90.0f;
const GLfloat PITCH = 0.0f;
const GLfloat SPEED = 3.0f;
const GLfloat MOUSE_SENSITIVTY = 0.05f;
const GLfloat MOUSE_ZOOM = 45.0f;
const float  MAX_PITCH_ANGLE = 89.0f; // 防止万向锁

class Camera
{
public:
	Camera(glm::vec3 pos = glm::vec3(0.0, 0.0, 2.0),
		glm::vec3 up = glm::vec3(0.0, 1.0, 0.0),
		GLfloat yaw  = YAW, GLfloat pitch = PITCH) 
		:position(pos), forward(0.0, 0.0, -1.0), worldUp(up),
		moveSpeed(SPEED), mouse_zoom(MOUSE_ZOOM), mouse_sensitivity(MOUSE_SENSITIVTY),
		yawAngle(yaw), pitchAngle(pitch)
	{
		this->updateCameraVectors();
	}
public:
	// 获取视变换矩阵
	glm::mat4 getViewMatrix()
	{
		return glm::lookAt(this->position, this->position + this->forward, this->viewUp);
	}
	// 处理键盘按键后方向移动
	void handleKeyPress(Camera_Movement direction, GLfloat deltaTime)
	{
		GLfloat velocity = this->moveSpeed * deltaTime;
		switch (direction)
		{
		case FORWARD:
			this->position += this->forward * velocity;
			break;
		case BACKWARD:
			this->position -= this->forward * velocity;
			break;
		case LEFT:
			this->position -= this->side * velocity;
			break;
		case RIGHT:
			this->position += this->side * velocity;
			break;
		default:
			break;
		}
	}
	// 处理鼠标移动
	void handleMouseMove(GLfloat xoffset, GLfloat yoffset)
	{
		
		xoffset *= this->mouse_sensitivity; // 用鼠标灵敏度调节角度变换
		yoffset *= this->mouse_sensitivity;

		this->yawAngle += xoffset;
		this->pitchAngle += yoffset;

		this->normalizeAngle();
		this->updateCameraVectors();
	}
	// 处理鼠标滚轮缩放 保持在[1.0, MOUSE_ZOOM]之间
	void handleMouseScroll(GLfloat yoffset)
	{
		if (this->mouse_zoom >= 1.0f && this->mouse_zoom <= MOUSE_ZOOM)
			this->mouse_zoom -= this->mouse_sensitivity * yoffset;
		if (this->mouse_zoom <= 1.0f)
			this->mouse_zoom = 1.0f;
		if (this->mouse_zoom >= 45.0f)
			this->mouse_zoom = 45.0f;
	}
	// 使pitch yaw角度保持在合理范围内
	void normalizeAngle()
	{
		//pitchAngle，y方向的偏移
		if (this->pitchAngle > MAX_PITCH_ANGLE)
			this->pitchAngle = MAX_PITCH_ANGLE;
		if (this->pitchAngle < -MAX_PITCH_ANGLE)
			this->pitchAngle = -MAX_PITCH_ANGLE;
		//yawAngle，x方向的偏移
		if (this->yawAngle < 0.0f)
			this->yawAngle += 360.0f;
	}
	// 计算forward side向量
	void updateCameraVectors()
	{
		//yawAngle：绕y轴旋转，即x方向的偏移，pitchAngle：绕x轴旋转，即y方向的偏移
		glm::vec3 forward;
		forward.x = cos(glm::radians(this->pitchAngle)) * cos(glm::radians(this->yawAngle));
		forward.y = sin(glm::radians(this->pitchAngle));
		forward.z = cos(glm::radians(this->pitchAngle)) * sin(glm::radians(this->yawAngle));
		this->forward = glm::normalize(forward);

		this->side = glm::normalize(glm::cross(this->forward, this->worldUp));
		this->viewUp = glm::normalize(glm::cross(this->side, this->forward));
	}
public:
	glm::vec3 forward, worldUp, side, viewUp, position; // 相机属性
	GLfloat yawAngle, pitchAngle; // 欧拉角
	GLfloat moveSpeed, mouse_sensitivity, mouse_zoom; // 相机选项
};

#endif