#define _USE_MATH_DEFINES
#include <cmath>

#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cstdlib>
#include <cstdio>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>

#include <memory>
#include <algorithm>
#include <exception>

#include "Error.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "Mesh.h"
#include "MeshLoader.h"
#include "Light.h"
#include "Material.h"

static const std::string SHADER_PATH("Resources/Shaders/");

static GLFWwindow* window = nullptr;
static std::shared_ptr<Camera> camera = nullptr;
static std::vector<std::shared_ptr<Mesh>> mesh = {}; // later turn this into an array

static std::shared_ptr<ShaderProgram> shader = nullptr;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	camera->setAspectRatio (static_cast<float>(width) / static_cast<float>(height));
	glViewport(0, 0, width, height);
}

void process_input(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

// Camera control variables
static float meshScale = 1.0; // To update based on the mesh size, so that navigation runs at scale
static bool isRotating(false);
static bool isPanning(false);
static bool isZooming(false);
static double baseX(0.0), baseY(0.0);
static glm::vec3 baseTrans(0.0);
static glm::vec3 baseRot(0.0);

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	float normalizer = static_cast<float> ((width + height) / 2);
	float dx = static_cast<float> ((baseX - xpos) / normalizer);
	float dy = static_cast<float> ((ypos - baseY) / normalizer);
	if (isRotating) {
		glm::vec3 dRot(-dy * M_PI, dx * M_PI, 0.0);
		camera->setRotation(baseRot + dRot);
	}
	else if (isPanning) {
		camera->setTranslation(baseTrans + meshScale * glm::vec3(dx, dy, 0.0));
	}
	else if (isZooming) {
		camera->setTranslation(baseTrans + meshScale * glm::vec3(0.0, 0.0, dy));
	}
}

void mouse_button_callback(GLFWwindow * window, int button, int action, int mods) { // use an event registering system! use a list of pairs (button, action) that maps into a set of callback functions
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (!isRotating) {
			isRotating = true;
			glfwGetCursorPos(window, &baseX, &baseY);
			baseRot = camera->getRotation();
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		isRotating = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		if (!isPanning) {
			isPanning = true;
			glfwGetCursorPos(window, &baseX, &baseY);
			baseTrans = camera->getTranslation();
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
		isPanning = false;
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
		if (!isZooming) {
			isZooming = true;
			glfwGetCursorPos(window, &baseX, &baseY);
			baseTrans = camera->getTranslation();
		}
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
		isZooming = false;
	}
}

void key_callback(GLFWwindow * windowPtr, int key, int scancode, int action, int mods) {
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

void glfw_init() {

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
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // for when the user attempts to resize the window: we in this case set it's size accordingly
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	glfwMakeContextCurrent(window);
}

void glad_init(glm::vec4 color = glm::vec4(0.2f, 0.3f, 0.3f, 1.0f)) {

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	glViewport(0, 0, 800, 600);
	glClearColor(color.x, color.y, color.z, color.w); // sets the color of a cleared screen
	
	glEnable(GL_DEBUG_OUTPUT); // Modern error callback functionnality
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // For recovering the line where the error occurs, set a debugger breakpoint in DebugMessageCallback
	glDebugMessageCallback (debugMessageCallback, 0); // Specifies the function to call when an error message is generated.
	glCullFace(GL_BACK);     // Specifies the faces to cull (here the ones pointing away from the camera)
	glEnable(GL_CULL_FACE); // Enables face culling (based on the orientation defined by the CW/CCW enumeration).
	glDepthFunc(GL_LESS); // Specify the depth test for the z-buffer
	glEnable(GL_DEPTH_TEST); // Enable the z-buffer test in the rasterization

}

std::string toString(glm::vec3 v) {
	return "(" + std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z) + ")";
}



void terminate_resources() {
	camera.reset();
	for(int i = 0; i < mesh.size(); i++)
		mesh[i].reset();
	shader.reset();
	glfwDestroyWindow(window);
	glfwTerminate();
}

void exit_error(const std::string& message) {
	terminate_resources();
	std::cerr << message << std::endl;
	std::getchar();
	std::exit(EXIT_FAILURE);
}

void load_shaders() {
	try {
		shader = ShaderProgram::genBasicShaderProgram(SHADER_PATH + "VertexShader.glsl",
			SHADER_PATH + "FragmentShader.glsl");
	}
	catch (std::exception & e) {
		exit_error("Error loading shader program ["			+ SHADER_PATH + "VertexShader.glsl" +
										  "] and ["			+ SHADER_PATH + "FragmentShader.glsl" +			"]. \n"+ e.what());
	}
}

std::shared_ptr<Light> light = nullptr;

void set_light() {
	glm::vec3 lightSourcePosition(10.0, 10.0, 10.0);
	glm::vec3 lightSourceColor(1.0, 1.0, 1.0);
	float lightSourceIntensity = 1.f;
	shader->set("lightSource.position", lightSourcePosition);
	shader->set("lightSource.color", lightSourceColor);
	shader->set("lightSource.intensity", lightSourceIntensity);

	light = std::make_shared<Light>();
	light->SetPosition(lightSourcePosition);
}

void set_material(Material m) {
	shader->set("material.albedo", m.albedo);
	shader->set("material.shininess", m.shininess);
	shader->set("material.kd", m.kd);
	shader->set("material.ks", m.ks);
}

void generate_triangle() {
	float vertex_positions[] = {
		-0.5f, -0.5f, 0.0f,
		0.8f, -0.8f, 0.0f,
		0.0f, 0.9f, 0.0f
	};
		
}

std::shared_ptr<Mesh> load_mesh(const std::string& filename) {
	
	// Mesh
	std::shared_ptr<Mesh> meshPtr = std::make_shared<Mesh>();
	try {
		MeshLoader::loadOFF(filename, meshPtr);
	}
	catch (std::exception & e) {
		exit_error("Error loading mesh from " + filename + ":" + e.what());
	}
	meshPtr->init();

	mesh.push_back(meshPtr);

	return meshPtr;
}

enum Primitives { P_SPHERE };

std::shared_ptr<Mesh> load_primitive(Primitives shape, glm::vec3 translation = { 0,0,0 }, glm::vec3 rotation = { 0,0,0 }, float scale = 1.0f) {

	// Mesh
	std::shared_ptr<Mesh> meshPtr = std::make_shared<Mesh>();

	switch (shape) {
	case P_SPHERE:
		Mesh::primitiveSphere(50, meshPtr); break;
	}

	meshPtr->init();

	meshPtr->setTranslation(translation);
	meshPtr->setRotation(rotation);
	meshPtr->setScale(scale);

	mesh.push_back(meshPtr);

	return meshPtr;
}

void load_scene() {
	// Camera
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	camera = std::make_shared<Camera>();
	camera->setAspectRatio(static_cast<float>(width) / static_cast<float>(height));

	load_primitive(P_SPHERE, { 0,0,-4 }, { 0,M_PI_2,0 }, 0.5);
	//load_mesh("Resources/Models/sphere.off");
	load_primitive(P_SPHERE, { 0,0,2 }, { 0,M_PI_2,0 });
	
	// Adjust the camera to the actual mesh
	float meshSize = 1.0f;
	glm::vec3 center;
	mesh[0]->computeBoundingSphere(center, meshSize);
	meshSize *= 2;
	glm::vec3 k = center + mesh[0]->getTranslation();
	std::cout << k.x << " " << k.y << " " << k.z << std::endl;
	
	camera->setTranslation(center + mesh[0]->getTranslation() + glm::vec3(0, 0.0, 6.0 * meshSize));
	camera->setNear(meshSize / 100.f);
	camera->setFar(60.f * meshSize);
}

void update() {

}
void draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Erase the color and z buffers.
	shader->use(); // Activate the program to be used for upcoming primitive
	glm::mat4 projectionMatrix = camera->computeProjectionMatrix();
	shader->set("projectionMat", projectionMatrix); // Compute the projection matrix of the camera and pass it to the GPU program

	glm::mat4 viewMatrix = camera->computeViewMatrix();
	shader->set("viewMat",viewMatrix);

	Material red = Material();

	glm::vec3 lightpos = light->GetPosition();
	glm::vec4 l = viewMatrix * glm::vec4(lightpos, 1.0);
	glm::vec3 lv = glm::vec3(l);
	shader->set("lightSource.position", lv);

	for (int i = 0; i < mesh.size(); i++) {
		glm::mat4 modelMatrix = mesh[i]->computeTransformMatrix();
		glm::mat4 normalMatrix = glm::transpose(glm::inverse(viewMatrix * modelMatrix));
		
		set_material(red);

		shader->set("modelMat", modelMatrix);
		shader->set("normalMat", normalMatrix);
		
		mesh[i]->render();
	}
	shader->stop();

}

int main() {

	glfw_init();
	glad_init();

	load_shaders();
	load_scene();

	set_light();

	while (!glfwWindowShouldClose(window)) { // check if the close button has been triggered for the window 
		process_input(window);
		
		update();
		
		draw();

		glfwSwapBuffers(window); // double buffers
		glfwPollEvents(); // check if any events have been triggered for glfw, like inputs.
	}

	glfwTerminate();
	return 0;

}