#define _USE_MATH_DEFINES

#include "Window.h"

#include <iostream>
#include <exception>
#include <time.h>

#include "Scene/Scene_1.h"


void Window::glfw_init() {

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		std::exit(EXIT_FAILURE);
	}

	// sets all callback functions for events
	glfwSetFramebufferSizeCallback(window, &Window::framebuffer_size_callback); // for when the user attempts to resize the window: we in this case set it's size accordingly
	glfwSetCursorPosCallback(window, this->cursor_pos_callback);
	glfwSetKeyCallback(window, this->key_callback);
	glfwSetMouseButtonCallback(window, this->mouse_button_callback); // make callbacks static

	glfwMakeContextCurrent(window);
}

void Window::glad_init(glm::vec4 color = glm::vec4(0.2f, 0.3f, 0.3f, 1.0f)) {

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	glViewport(0, 0, 800, 600);
	glClearColor(color.x, color.y, color.z, color.w); // sets the color of a cleared screen

	glEnable(GL_DEBUG_OUTPUT); // Modern error callback functionnality
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // For recovering the line where the error occurs, set a debugger breakpoint in DebugMessageCallback
	glDebugMessageCallback(debugMessageCallback, 0); // Specifies the function to call when an error message is generated.
	//Cullfacing was culling the wrong face, that's why we got weird behaviour from lighting (we were seeing the inside of the sphere)
	glCullFace(GL_BACK);     // Specifies the faces to cull (here the ones pointing away from the camera)
	//glEnable(GL_CULL_FACE); // Enables face culling (based on the orientation defined by the CW/CCW enumeration).
	glDepthFunc(GL_LESS); // Specify the depth test for the z-buffer
	glEnable(GL_DEPTH_TEST); // Enable the z-buffer test in the rasterization

}

void Window::terminate_resources() {
	current_scene->terminate();
	shader.reset();
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::terminate()
{
	terminate_resources();
	glfwTerminate();
}

void Window::load_shaders() {
	try {
		shader = ShaderProgram::genBasicShaderProgram(SHADER_PATH + "VertexShader.glsl",
			SHADER_PATH + "FragmentShader.glsl");
	}
	catch (std::exception & e) {
		Game::GetInstance().Exit("Error loading shader program [" + SHADER_PATH + "VertexShader.glsl" +
			"] and [" + SHADER_PATH + "FragmentShader.glsl" + "]. \n" + e.what(), true);
	}
}

void Window::update()
{
	process_input(window);

	current_scene->update();

	Game::GetInstance().Update(glfwGetTime());

	draw();

	glfwSwapBuffers(window); // double buffers
	glfwPollEvents(); // check if any events have been triggered for glfw, like inputs.
}

void Window::init()
{
	Game& game = Game::GetInstance();
	
	// Bind the error callback so errors are logged
	game.SetExitCallback(std::bind(&Window::terminate, this));

	glfw_init();
	glad_init();
	
	load_shaders();
	current_scene = std::make_shared<Scene_1>();
	current_scene->init();
	
	Game::GetInstance().SetTime(glfwGetTime());
}


void Window::draw() {
	current_scene->draw(shader);
}

Window& Window::GetInstance()
{
	static Window app;

	return app;
}

void Window::process_input(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void Window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	//Window::GetInstance().current_scene->camera->setAspectRatio(static_cast<float>(width) / static_cast<float>(height));
	glViewport(0, 0, width, height);
}

void Window::cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{/*
	Window& a = Window::GetInstance();
	std::shared_ptr<Scene> s = a.current_scene;

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	float normalizer = static_cast<float> ((width + height) / 2);
	float dx = static_cast<float> ((s->baseX - xpos) / normalizer);
	float dy = static_cast<float> ((ypos - s->baseY) / normalizer);
	if (s->isRotating) {
		glm::vec3 dRot(-dy * M_PI, dx * M_PI, 0.0);
		//s->camera->rotation = s->baseRot + dRot;
	}
	else if (s->isPanning) {
		//s->camera->position = s->baseTrans + s->meshScale * glm::vec3(dx, dy, 0.0);
	}
	else if (s->isZooming) {
		//s->camera->position = s->baseTrans + s->meshScale * glm::vec3(0.0, 0.0, dy);
	}*/
}

void Window::mouse_button_callback(GLFWwindow * window, int button, int action, int mods)
{ // use an event registering system! use a list of pairs (button, action) that maps into a set of callback functions
/*	Window& a = Window::GetInstance();
	std::shared_ptr<Scene> s = a.current_scene;

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (!s->isRotating) {
			s->isRotating = true;
			glfwGetCursorPos(window, &s->baseX, &s->baseY);
			//s->baseRot = s->camera->rotation;
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		s->isRotating = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		if (!s->isPanning) {
			s->isPanning = true;
			glfwGetCursorPos(window, &s->baseX, &s->baseY);
			//s->baseTrans = s->camera->position;
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		s->isPanning = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
		if (!s->isZooming) {
			s->isZooming = true;
			glfwGetCursorPos(window, &s->baseX, &s->baseY);
			//s->baseTrans = s->camera->position;
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
		s->isZooming = false;
	}*/
}

void Window::key_callback(GLFWwindow * windowPtr, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS && key == GLFW_KEY_H) {
		//printHelp();
	}
	else if (action == GLFW_PRESS && key == GLFW_KEY_F1) {
		GLint mode[2];
		glGetIntegerv(GL_POLYGON_MODE, mode);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glPolygonMode(GL_FRONT_AND_BACK, mode[0] == GL_FILL ? GL_LINE : GL_FILL); // fixed: original code was using mode[1]
	}
	else if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
		glfwSetWindowShouldClose(windowPtr, true); // Closes the application if the escape key is pressed
	}
}

