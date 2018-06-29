#pragma once
//camera class
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

extern bool keyPressState[1024];

class Camera
{
	const float speed = 5.0;
	bool tracking;
	glm::vec3 eyePos;
	glm::vec3 forward;
	void rotateHorizontal(float angle)
	{
		forward = glm::vec3(glm::rotate(glm::mat4(), glm::radians(angle), glm::vec3(0, 1, 0))*glm::vec4(forward.x, forward.y, forward.z, 0.0));
		forward = glm::normalize(forward);
	}
	void rotateVertical(float angle)
	{
		float pitch = glm::degrees(glm::asin(forward.y));
		if (abs(pitch + angle) < 89.0)
		{
			forward = glm::vec3(glm::rotate(glm::mat4(), glm::radians(angle), glm::cross(forward, glm::vec3(0, 1, 0)))*glm::vec4(forward.x, forward.y, forward.z, 0));
		}
	}
	glm::vec2 mousePos;
public:
	Camera()
	{
		tracking = false;
		reset();
	}

	void reset()
	{
		tracking = false;
		eyePos = glm::vec3(0, 400, -400);
		forward = glm::vec3(0, -1, 1);
	}

	glm::mat4 getCameraViewMatrix()
	{
		return glm::lookAt(eyePos, eyePos + forward, glm::vec3(0, 1, 0));
	}
	void strafe(float dis)
	{
		glm::vec3 right = glm::cross(forward, glm::vec3(0, 1, 0));
		eyePos += glm::normalize(right)*dis*speed;
	}
	void walk(float dis)
	{
		eyePos += forward*dis*speed;
	}
	void startTrackMouse(int x, int y)
	{
		tracking = true;
		mousePos = glm::vec2(x, y);
	}
	void stopTrackMouse()
	{
		tracking = false;
	}
	void mouseMove(int x, int y)
	{
		if (tracking)
		{
			int dx = x - mousePos.x;
			int dy = y - mousePos.y;
			rotateHorizontal(-dx / 10.0);
			rotateVertical(-dy / 10.0);
			mousePos = glm::vec2(x, y);
		}
	}

	void update()
	{
		float speedRatio = 1.0;
		if (keyPressState[340])
		{
			speedRatio = 50.0;
		}
		if (keyPressState['A'])
		{
			strafe(-0.01*speed*speedRatio);
		}
		if (keyPressState['D'])
		{
			strafe(0.01*speed*speedRatio);
		}
		if (keyPressState['S'])
		{
			walk(-0.01*speed*speedRatio);
		}
		if (keyPressState['W'])
		{
			walk(0.01*speed*speedRatio);
		}
	}

};

