#include <camera.h>
#include <main.h>

class Program;
extern Program program;

Ray::Ray(glm::vec3 _startPos, glm::vec3 _direction, float _distance)
{
	startPos = _startPos;
	direction = _direction;
	length = _distance;
}

void Ray::traceRay()
{
	endPos = startPos + (direction * length);
}

Camera::Camera()
{
	// create a lookat matrix
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	projection = glm::mat4(1.0f);
}

void Camera::moveCamera(glm::vec3 movement)
{
	// apply speed modifiers if necessary
	if (program.input.shift_down)
	{
		currentCameraSpeed = CAMERA_SPEED * 2.5f;
	}
	else if (program.input.alt_down)
	{
		currentCameraSpeed = CAMERA_SPEED * 0.25f;
	}
	else
	{
		currentCameraSpeed = CAMERA_SPEED;
	}

	lastMovement = movement;
	cameraPos += movement * currentCameraSpeed;
	cameraPos.z = std::clamp(cameraPos.z, cameraNearPos, cameraFarPos);
}

void Camera::updateView()
{
	// maybe this should be in moveCamera, if i do make mouse looking as well
	pitch = std::clamp(pitch, -89.0f, 89.0f);
	// calculate new front vector
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
	direction.y = 0;
	movementFront = glm::normalize(direction);

	cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	cameraUp    = glm::normalize(glm::cross(cameraRight, cameraFront));

	// update matrices
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	// perspective projection
	// projection  				   					FOV													ASPECT RATIO									CLIP RANGE
	// projection = glm::perspective(glm::radians(FOV), (float(program.windowManager.SCREEN_WIDTH) / float(program.windowManager.SCREEN_HEIGHT)), 0.1f, 100.1f);
	// orthographic projection
	float aspectRatio = (float(program.windowManager.SCREEN_WIDTH) / float(program.windowManager.SCREEN_HEIGHT));
	float sizeMultiplier = cameraPos.z / cameraFarPos;
	float baseSize = 41.4675f;
	projection = glm::ortho(-baseSize * sizeMultiplier * aspectRatio, baseSize * sizeMultiplier * aspectRatio, -baseSize * sizeMultiplier, baseSize * sizeMultiplier, nearZ, farZ);
}

// calculates the world ray from eye coordinates and distance
Ray Camera::to_world_ray(glm::vec4 eyeCoords, float distance)
{
 	// eye coord to world ray calculation
	glm::mat4 invertView = glm::inverse(view);
	glm::vec4 rayWorld = invertView * eyeCoords;

	Ray ray = Ray(cameraPos, glm::vec3(rayWorld), distance);

	return ray;
}

// function converting pixel coordinates to NDCs
glm::vec2 Camera::to_NDC(glm::vec2 pos)
{
	float x = (2.0f * pos.x) / program.windowManager.SCREEN_WIDTH - 1.0f;
	float y = (2.0f * (program.windowManager.SCREEN_HEIGHT - pos.y)) / program.windowManager.SCREEN_HEIGHT - 1.0f;

 	return glm::vec2(x, y);
}

// function calculating eye coordinates from clip coordinates
glm::vec4 Camera::to_eye(glm::vec4 clipCoords)
{
	// multiplying the matrices together
	// TODO: here is where the ortho projection goes bad 
	glm::vec3 eyeCoords = glm::inverse(projection) * clipCoords;

	// returning a vector
	return glm::vec4(eyeCoords.x / 2, eyeCoords.y / 2, -1.0f, 0.0f);
 }

Ray Camera::screen_to_world_ray(glm::vec2 pos)
{
	glm::vec2 ndc = to_NDC(pos);
	glm::vec4 clip = glm::vec4(ndc, -1.0, 1.0);

	glm::vec4 eye = to_eye(clip);

	Ray worldRay = to_world_ray(eye, cameraPos.z);
	return worldRay;
}

// Function calculating screen -> world coordinates.
glm::vec4 Camera::screen_to_world(glm::vec2 pos)
{
	glm::vec4 worldPos;

	// perspective projection
	// Ray worldRay = screen_to_world_ray(pos);
	// worldRay.length += cameraPos.z;
	// worldRay.traceRay();
	// worldRay.endPos.z = 0.0f;
	// worldPos = glm::vec4(worldRay.endPos, 1.0f);
	// orthographic projection
	glm::vec2 ndc = to_NDC(pos);
	glm::vec4 nearClip = glm::vec4(ndc, -1.0, 1.0);
	auto nearView = glm::inverse(projection) * nearClip;
	auto nearWorld = glm::inverse(view) * nearView;
	nearWorld.z = 0.0f;
	worldPos = nearWorld;

	// Returning the ending position.
	return worldPos;
}
