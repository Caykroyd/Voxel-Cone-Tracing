#ifndef APP_H
#define APP_H

#define _USE_MATH_DEFINES

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <list>
#include <memory>
#include <string>

#include "Utility/Error.h"
#include "Shader/ShaderProgram.h"
#include "Scene/Scene.h"
#include "Game.h"

class Window {

private:

	const std::string SHADER_PATH = std::string("Resources/Shaders/");

	std::shared_ptr<ShaderProgram> shader = nullptr;
	
	void process_input(GLFWwindow* window);

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void cursor_pos_callback(GLFWwindow * window, double xpos, double ypos);
	static void mouse_button_callback(GLFWwindow * window, int button, int action, int mods);
	static void key_callback(GLFWwindow * windowPtr, int key, int scancode, int action, int mods);

	Window() {};

	void glfw_init();
	void glad_init(glm::vec4 color);

	void load_shaders();

	void draw();

	void terminate_resources();
	
public:
	Window(Window const&) = delete;
	void operator=(Window const&) = delete;
	static Window& GetInstance();

	void init();

	void update();

	inline bool ShouldClose() const { return glfwWindowShouldClose(window); };

	void terminate();

	std::shared_ptr<Scene> current_scene = nullptr;

	GLFWwindow* window = nullptr;

	float window_width, window_height;

};

#endif