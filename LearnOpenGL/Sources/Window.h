#ifndef APP_H
#define APP_H

#define _USE_MATH_DEFINES

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/quaternion.hpp>

#include <list>

#include "Error.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "Mesh.h"
#include "MeshLoader.h"
#include "Light.h"
#include "Material.h"
#include "Scene.h"

class Window {

private:

	const std::string SHADER_PATH = std::string("Resources/Shaders/");

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void cursor_pos_callback(GLFWwindow * window, double xpos, double ypos);
	static void mouse_button_callback(GLFWwindow * window, int button, int action, int mods);
	static void key_callback(GLFWwindow * windowPtr, int key, int scancode, int action, int mods);

	void glfw_init();
	void glad_init(glm::vec4 color);
	
	void terminate_resources();
	
	void load_shaders();

	void set_light(const Light l) const;

	void set_material(const Material m) const;

	void set_scene(Scene& s) const;
	
	void draw();
	   
	std::shared_ptr<ShaderProgram> shader = nullptr;

	void process_input(GLFWwindow* window);

	std::string toString(glm::vec3 v);

	Window() {};

	static double _time;

public:
	Window(Window const&) = delete;
	void operator=(Window const&) = delete;
	static Window& getInstance();

	void update();

	bool shouldClose();

	void init();

	void terminate();

	static void exit_error(const std::string & message);

	std::shared_ptr<Scene> current_scene = nullptr;

	GLFWwindow* window = nullptr;

	float window_width, window_height;

	static double delta_time;

};

#endif