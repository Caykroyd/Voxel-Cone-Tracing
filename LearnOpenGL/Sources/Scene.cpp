#define _USE_MATH_DEFINES
#include "Scene.h"

#include <glm/glm.hpp>

#include "MeshLoader.h"
#include "App.h"
#include "FirstPersonController.h"
#include "PerspectiveCamera.h"

#include <memory>
#include <iostream>
#include "App.h" // circular dependency


void Scene::init() {
	
	// Camera
	camera = std::make_shared<PerspectiveCamera>();
	std::shared_ptr<FirstPersonController> controller = std::make_shared<FirstPersonController>(camera);
	Window::Register(controller);

	light = Light(glm::vec3(10.0, 10.0, 10.0), glm::vec3(1.0, 1.0, 1.0), 1.f);

	//load_primitive(P_SPHERE, { 0,0,-4 }, { 0,M_PI_2,0 }, 0.5);
	load_mesh("Resources/Models/sphere.off");
	load_primitive(P_SPHERE, { 2,0,0 }, { 0,M_PI_2,0 }, 1.0);
	//load_primitive(P_SPHERE, { -2,0,0 }, { 0,M_PI_2,0 }, 1.0);
	load_primitive(P_SPHERE, { 0,2,0 }, { 0,M_PI_2,0 }, 1.0);
	load_primitive(P_SPHERE, { 0,-2,0 }, { 0,M_PI_2,0 }, 1.0);

	// Adjust the camera to the actual mesh
	float meshSize = 1.0f;
	glm::vec3 center;
	mesh[0]->computeBoundingSphere(center, meshSize);
	meshSize *= 2;
	glm::vec3 k = center + mesh[0]->getTranslation();
	std::cout << k.x << " " << k.y << " " << k.z << std::endl;

	camera->position = center + mesh[0]->getTranslation() + glm::vec3(0, 0.0, 6.0 * meshSize);
	//camera->setNear(meshSize / 100.f);
	//camera->setFar(60.f * meshSize);
}

void Scene::terminate() {
	camera.reset();
	for (int i = 0; i < mesh.size(); i++)
		mesh[i].reset();
}

std::shared_ptr<Mesh> Scene::load_mesh(const std::string& filename) {

	// Mesh
	std::shared_ptr<Mesh> meshPtr = std::make_shared<Mesh>();
	try {
		MeshLoader::loadOFF(filename, meshPtr);
	}
	catch (std::exception & e) {
		Window::exit_error("Error loading mesh from " + filename + ":" + e.what());
	}
	meshPtr->init();

	mesh.push_back(meshPtr);

	return meshPtr;
}


std::shared_ptr<Mesh> Scene::load_primitive(Primitives shape, glm::vec3 translation, glm::vec3 rotation, float scale) {

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

