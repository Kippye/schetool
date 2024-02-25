#pragma once

#include <algorithm>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <window.h>
#include <input.h>

class Window;

class Ray
{
	public:
		glm::vec3 startPos, endPos, direction;
		float length;
		void traceRay();

		Ray(){}
		Ray(glm::vec3 _startPos, glm::vec3 _direction, float distance);
};

class Camera
{
	private:
		Window* m_windowManager;
	public:
		float cameraNearPos = 2.0f, cameraFarPos = 100.0f;
		float nearZ = 0.1f, farZ = 100.1f;
		static const inline float CAMERA_SPEED = 14.0f;
		glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 cameraRight = glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 movementFront = glm::vec3(0.0f, 0.0f, -1.0f);
		glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 lastMovement = glm::vec3(0.0f);
		glm::mat4 view, projection;
		float yaw = -90.0f, pitch = 0.0f;
		float FOV = 45.0f;
		float zoomSpeed = 1.5f;
		float currentCameraSpeed = CAMERA_SPEED;
		const float cameraSensitivity = 0.1f;
		float yVelocity = 0.0f;

	public:
		Camera();
		void init(Window*);
		void moveCamera(glm::vec3 movement);
		void updateView();
		// Function that calculates the world ray from eye coordinates and distance.
		Ray to_world_ray(glm::vec4 eyeCoords, float distance);
		// Function converting pixel coordinates to NDCs.
		glm::vec2 to_NDC(glm::vec2 pos);
		// Function calculating eye coordinates from clip coordinates.
		glm::vec4 to_eye(glm::vec4 clipCoords);
		Ray screen_to_world_ray(glm::vec2 pos);
		// Function calculating screen, world coordinates.
		glm::vec4 screen_to_world(glm::vec2 pos);
};
