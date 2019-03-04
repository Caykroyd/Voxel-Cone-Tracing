#ifndef SCENE_1_H
#define SCENE_1_H

#define _USE_MATH_DEFINES

#include "Scene/Scene.h"
#include "Camera/PerspectiveCamera.h"
#include "Camera/FirstPersonController.h"

class Scene_1 : public Scene {
		
public:
	void init() override {

		// Camera
		camera = std::make_shared<PerspectiveCamera>();
		std::shared_ptr<FirstPersonController> controller = std::make_shared<FirstPersonController>(camera);
		Register(controller);


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

};

#endif