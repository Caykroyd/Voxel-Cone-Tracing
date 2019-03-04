/*
MIT License

Copyright(c) 2016 Fredrik Präntare

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#pragma once

#include <iostream> // TODO: Remove.

#define GLEW_STATIC
#define GLM_ENABLE_EXPERIMENTAL
#include <glm\glm.hpp>
#include <GLFW/glfw3.h>
#include <glm/gtx/rotate_vector.hpp>

#include "Camera.h"
#include "Window.h"

#include <memory>

/// <summary> A first person controller that can be attached to a camera. </summary>
class FirstPersonController : public IUpdatable {
public:
	const float CAMERA_SPEED = 1.4f;
	const float CAMERA_ROTATION_SPEED = 0.003f;
	const float CAMERA_POSITION_INTERPOLATION_SPEED = 8.0f;
	const float CAMERA_ROTATION_INTERPOLATION_SPEED = 8.0f;

	std::shared_ptr<Camera> renderingCamera = nullptr;
	std::shared_ptr<Camera> targetCamera = nullptr; // Dummy camera used for interpolation.

	FirstPersonController(std::shared_ptr<Camera> camera) {
		targetCamera = std::make_shared<Camera>(*camera);
		renderingCamera = camera;
	}

	FirstPersonController() { }

	void init() {

	}

	void update() {

		if (firstUpdate) {
			targetCamera->rotation = renderingCamera->rotation;
			targetCamera->position = renderingCamera->position;

			int xwidth, yheight;
			glfwGetWindowSize(Window::getInstance().window, &xwidth, &yheight);
			glfwSetCursorPos(Window::getInstance().window, xwidth / 2, yheight / 2);
			//camera->setAspectRatio(static_cast<float>(width) / static_cast<float>(height));

			firstUpdate = false;
		}

		int xwidth, yheight;
		double xpos, ypos;
		double xmid, ymid;

		GLFWwindow * window = Window::getInstance().window;

		glfwGetWindowSize(window, &xwidth, &yheight);
		glfwGetCursorPos(window, &xpos, &ypos);

		xmid = xwidth / 2.0;
		ymid = yheight / 2.0;

		// ----------
		// Rotation.
		// ----------
		float xDelta = float(xmid - xpos);
		float yDelta = float(ymid - ypos);

		float xRot = static_cast<float>(CAMERA_ROTATION_SPEED * xDelta);
		float yRot = static_cast<float>(CAMERA_ROTATION_SPEED * yDelta);

		// X rotation.

		targetCamera->rotation = glm::rotateY(targetCamera->rotation, xRot);

		// Y rotation.
		glm::vec3 newDirection = glm::rotate(targetCamera->rotation, yRot, targetCamera->right());
		float a = glm::dot(newDirection, glm::vec3(0, 1, 0));
		if (abs(a) < 0.99)
			targetCamera->rotation = newDirection;


		// ----------
		// Position.
		// ----------
		// Move forward.
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			targetCamera->position += targetCamera->forward() * (float)Window::delta_time * CAMERA_SPEED;
		}
		// Move backward.
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			targetCamera->position -= targetCamera->forward() * (float)Window::delta_time * CAMERA_SPEED;
		}
		// Strafe right.
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			targetCamera->position += targetCamera->right() * (float)Window::delta_time * CAMERA_SPEED;
		}
		// Strafe left.
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			targetCamera->position -= targetCamera->right() * (float)Window::delta_time * CAMERA_SPEED;
		}

		// Interpolate between target and current camera.
		auto camera = renderingCamera;
		camera->rotation = mix(camera->rotation, targetCamera->rotation, glm::clamp(Window::delta_time * CAMERA_ROTATION_INTERPOLATION_SPEED, 0.0, 1.0));
		camera->position = mix(camera->position, targetCamera->position, glm::clamp(Window::delta_time * CAMERA_POSITION_INTERPOLATION_SPEED, 0.0, 1.0));

		// Reset mouse position for next update iteration.
		glfwSetCursorPos(window, xwidth / 2, yheight / 2);

		// Update view (camera) matrix.
		camera->updateViewMatrix();
	}
private:
	bool firstUpdate = true;
};
