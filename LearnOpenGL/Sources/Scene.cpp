#include "Scene.h"

#include <glm/glm.hpp>

#include "MeshLoader.h"
#include "Window.h"
#include "FirstPersonController.h"
#include "PerspectiveCamera.h"

#include <memory>
#include <iostream>


void Scene::draw(std::shared_ptr<ShaderProgram> shader)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Erase the color and z buffers.
	shader->use(); // Activate the program to be used for upcoming primitive

	ShaderProgram& s = *shader;
	
	camera->SendToShader(s);

	light.SendToShader(s);

	for (size_t i = 0; i < mesh.size(); i++) {

		Material material_red = Material();
		material_red.SendToShader(s);

		mesh[i]->SendToShader(s);

		mesh[i]->render();
	}
	shader->stop();
}

void Scene::update()
{
	std::list<std::shared_ptr<IUpdatable>> destroy_queue = std::list<std::shared_ptr<IUpdatable>>();

	for (auto& obj : updatables) {

		if (obj->should_destroy)
		{
			obj->destroy();
			destroy_queue.push_back(obj);
			continue;
		}

		if (obj->should_init)
		{
			obj->init();
		}

		obj->update();

	}

	// Destroy all marked objects
	for (auto obj : destroy_queue) {
		updatables.remove(obj);
	}

}

void Scene::terminate() {
	camera.reset();
	for (size_t i = 0; i < mesh.size(); i++)
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

