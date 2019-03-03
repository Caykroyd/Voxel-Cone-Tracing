// ----------------------------------------------
// Computer Graphics 
// Base code for practical assignment
// Introduction to interactive 3D graphics 
// application development with modern OpenGL.
//
// Copyright (C) 2018 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------

#define _USE_MATH_DEFINES

#include <glad/glad.h>
#include <cstdlib>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>
#include <memory>

#include <glm/glm.hpp>
#include <glm/ext.hpp>

#define SOLUTION

using namespace std;

// Window parameters
static GLFWwindow * window = nullptr;

static std::vector<float> vertexPositions; // All vertex positions packed in one array [x0, y0, z0, x1, y1, z1, ...]
static std::vector<unsigned int> triangleIndices; // All triangle indices packed in one array[v00, v01, v02, v10, v11, v12, ...] with vij the index of j-th vertex of the i-th triangle

// GPU objects
static GLuint program; // A GPU program contains at least a vertex shader and a fragment shader
static GLuint posVbo; // Identifier of the vertex buffer storing the list of vertex positions as an array in GPU memory
static GLuint ibo; // Identifier of the index buffer storing the connectivity of the mesh as an array in GPU memory 
static GLuint vao; // Identifier of a single wrapper linking vbos and ibos to form an complete GPU mesh, ready to draw

// CPU?
static std::vector<float> vertexColors;  

// Basic camera model
class Camera {
public:
	inline float getFov () const { return m_fov; }
	inline void setFoV (float f) { m_fov = f; }
	inline float getAspectRatio () const { return m_aspectRatio; }
	inline void setAspectRatio (float a) { m_aspectRatio = a; }
	inline float getNear () const { return m_near; }
	inline void setNear (float n) { m_near = n; }
	inline float getFar () const { return m_far; }
	inline void setFar (float n) { m_far = n; }
	
	// Returns the view matrix: to express the world w.r.t. the camera, 
	// we use the inverse of the camera own transform, to all scene's entities.
	// For now, the camera is fixed at [0, 0, 5] so that we can draw geometry at the origin
	// and actually see it.  
	inline glm::mat4 computeViewMatrix () const {
		return inverse (glm::translate (glm::mat4 (1.0), glm::vec3 (0.0, 0.0, 5.0)));
	}
	
	// Returns the projection matrix stemming from the camera parameter. CAREFUL: right now, a translation is added (basic view transform)
	inline glm::mat4 computeProjectionMatrix () const {
		return glm::perspective (glm::radians (m_fov), m_aspectRatio, m_near, m_far); 
	}

private:
	float m_fov = 45.f; // Field of view, in degrees
	float m_aspectRatio = 1.f; // Ratio between the width and the height of the image
	float m_near = 0.1f; // Distance before which geometry is excluded fromt he rasterization process
	float m_far = 10.f; // Distance after which the geometry is excluded fromt he rasterization process
};

static Camera camera;

// Executed each time the window is resized. Adjust the aspect ratio and the rendering viewport to the current window. 
void windowSizeCallback (GLFWwindow* window, int width, int height) {
	camera.setAspectRatio (static_cast<float>(width) / static_cast<float>(height));
	glViewport (0, 0, (GLint)width, (GLint)height); // Dimension of the rendering region in the window
}

// Executed each time a key is entered.
void keyCallback (GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS && key == GLFW_KEY_UP) {
		// Exercise III.c
	} else if (action == GLFW_PRESS && key == GLFW_KEY_DOWN) {
		// Exercise III.c
	} else if (action == GLFW_PRESS && key == GLFW_KEY_F1) {
		GLint mode[2];
		glGetIntegerv (GL_POLYGON_MODE, mode);
		glPolygonMode (GL_FRONT_AND_BACK, mode[1] == GL_FILL ? GL_LINE : GL_FILL);
	} else if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
		glfwSetWindowShouldClose (window, true); // Closes the application if the escape key is pressed
	}
}

void initGLFW () {
	// Initialize GLFW, the library responsible for window management
	if (!glfwInit ()) {
		std::cerr << "ERROR: Failed to init GLFW" << std::endl;
		std::exit (EXIT_FAILURE);
	}

	// Before creating the window, set some option flags
	glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint (GLFW_RESIZABLE, GL_TRUE);

	// Create the window
	window = glfwCreateWindow (1024, 768, "Base GL - Computer Graphics - Practical Assignment - OpenGL Intro.", nullptr, nullptr);
	if (!window) {
		std::cerr << "ERROR: Failed to open window" << std::endl;
		glfwTerminate ();
		std::exit (EXIT_FAILURE);
	}

	// Load the OpenGL context in the GLFW window using GLAD OpenGL wrangler
	glfwMakeContextCurrent (window);
	glfwSetWindowSizeCallback (window, windowSizeCallback);
	glfwSetKeyCallback (window, keyCallback);
}

void initOpenGL () {
	glfwMakeContextCurrent (window);
	if (!gladLoadGLLoader ((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "ERROR: Failed to initialize OpenGL context" << std::endl;
		glfwTerminate();
		std::exit (EXIT_FAILURE);
	}
	glCullFace (GL_BACK);     // Specifies the faces to cull (here the ones pointing away from the camera)
	glEnable (GL_CULL_FACE); // Enables face culling (based on the orientation defined by the CW/CCW enumeration).
	glDepthFunc (GL_LESS); // Specify the depth test for the z-buffer
	glEnable (GL_DEPTH_TEST); // Enable the z-buffer test in the rasterization
	glClearColor (0.0f, 0.0f, 0.0f, 1.0f); // specify the background color, used any time the framebuffer is cleared
}

// Loads the content of an ASCII file in a standard C++ string
std::string file2String (const std::string & filename) {
	std::ifstream t (filename.c_str ());
	std::stringstream buffer;
	buffer << t.rdbuf ();
	return buffer.str ();
}

// Loads and compile a shader, before attaching it to a program
void loadShader (GLuint program, GLenum type, const std::string & shaderFilename) {
	GLuint shader = glCreateShader (type); // Create the shader, e.g., a vertex shader to be applied to every single vertex of a mesh
	std::string shaderSourceString = file2String (shaderFilename); // Loads the shader source from a file to a C++ string
	const GLchar * shaderSource = (const GLchar *)shaderSourceString.c_str (); // Interface the C++ string through a C pointer
	glShaderSource (shader, 1, &shaderSource, NULL); // Load the vertex shader source code
	glCompileShader (shader);  // THe GPU driver compile the shader
	glAttachShader (program, shader); // Set the vertex shader as the one ot be used with the program/pipeline
	glDeleteShader (shader);
}

void initGPUProgram () {
	program = glCreateProgram (); // Create a GPU program i.e., a graphics pipeline
	loadShader (program, GL_VERTEX_SHADER, "intro_shaders/VertexShader.glsl");
	loadShader (program, GL_FRAGMENT_SHADER, "intro_shaders/FragmentShader.glsl");
	glLinkProgram (program); // The main GPU program is ready to be handle streams of polygons
}

// Change this code to crear a different geometry
void initCPUGeometry () {
	vertexPositions = { // The array of vertex positions 
		0.f, 0.f, 0.f,   1.f, 0.f, 0.f,   0.f, 1.f, 0.f 
	}; 
	triangleIndices = { 0, 1, 2 }; 
	vertexColors = {    // The array of vertex colors 
		1.f, 0.f, 0.f,   0.f, 1.f, 0.f,   0.f, 0.f, 1.f  
	};
}

void initGPUGeometry () {
	glCreateBuffers (1, &posVbo); // Generate a GPU buffer to store the positions of the vertices
	size_t vertexBufferSize = sizeof (float) * vertexPositions.size (); // Gather the size of the buffer from the CPU-side vector
	glNamedBufferStorage (posVbo, vertexBufferSize, NULL, GL_DYNAMIC_STORAGE_BIT); // Creta a data store on the GPU
	glNamedBufferSubData (posVbo, 0, vertexBufferSize, vertexPositions.data ()); // Fill the data store from a CPU array
	glCreateBuffers (1, &ibo); // Same for the index buffer, that stores the list of indices of the triangles forming the mesh
	size_t indexBufferSize = sizeof (unsigned int) * triangleIndices.size ();
	glNamedBufferStorage (ibo, indexBufferSize, NULL, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferSubData (ibo, 0, indexBufferSize, triangleIndices.data ());
	glCreateVertexArrays (1, &vao); // Create a single hangle that joins together attributes (vertex positions, normals) and connectivity (triangles indices)
	glBindVertexArray (vao);
	glEnableVertexAttribArray (0);
	glBindBuffer (GL_ARRAY_BUFFER, posVbo);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (GLfloat), 0);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBindVertexArray (0); // Desactive the VAO just created. Will be activated at rendering time. 
}

void initCamera () {
	int width, height;
	glfwGetWindowSize (window, &width, &height);
	camera.setAspectRatio (static_cast<float>(width) / static_cast<float>(height));
}

void init () {
	initGLFW ();
	initOpenGL ();
	initCPUGeometry ();
	initGPUProgram ();
	initGPUGeometry ();
	initCamera ();
}

void clear () {
	glDeleteProgram (program);
	glDeleteVertexArrays (1, &vao);
	glDeleteBuffers (1, &posVbo);
	glDeleteBuffers (1, &ibo);
	glfwDestroyWindow (window);
	glfwTerminate ();
}

// The main rendering call
void render () {
	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Erase the color and z buffers.
	glm::mat4 projectionMatrix = camera.computeProjectionMatrix ();
	glm::mat4 viewMatrix = camera.computeViewMatrix ();
	glUseProgram (program); // Activate the program to be used for upcoming primitive
	glUniformMatrix4fv (glGetUniformLocation (program, "projectionMat"), 1, GL_FALSE, glm::value_ptr (projectionMatrix)); // Pass it to the GPU program
	glUniformMatrix4fv (glGetUniformLocation (program, "modelViewMat"), 1, GL_FALSE, glm::value_ptr (viewMatrix)); 
	glBindVertexArray (vao); // Activate the VAO storing geometry data
	glDrawElements (GL_TRIANGLES, triangleIndices.size (), GL_UNSIGNED_INT, 0); // Call for rendering: stream the current GPU geometry through the current GPU program
}

// Update any accessible variable based on the current time
void update (float currentTime) {
	// Animate any entity of the program here
	static const float initialTime = currentTime;
	float dt = currentTime - initialTime;
	// <---- Update here what needs to be animated over time ---->
}

int main (int argc, char ** argv) {
	init (); // Your initialization code (user interface, OpenGL states, scene with geometry, material, lights, etc)
	while (!glfwWindowShouldClose (window)) {
		update (static_cast<float> (glfwGetTime ()));
		render ();
		glfwSwapBuffers (window);
		glfwPollEvents();
	}
	clear ();
	return EXIT_SUCCESS;
}

