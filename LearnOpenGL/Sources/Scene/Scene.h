#pragma once

#include <glm/glm.hpp>

#include <list>

#include "../Camera/Camera.h"
#include "Light/Light.h"
#include "Mesh/Mesh.h"
#include "Extendable/IUpdatable.h"

enum Primitives { P_SPHERE };

class Scene {
private:
	std::list<std::shared_ptr<IUpdatable>> updatables;
protected:
	std::shared_ptr<Mesh> load_mesh(const std::string & filename);
	std::shared_ptr<Mesh> load_primitive(Primitives shape, glm::vec3 translation, glm::vec3 rotation, float scale);
public:
	Scene() {};

	// Camera control variables
	float meshScale = 1.0; // To Update based on the mesh size, so that navigation runs at scale
	bool isRotating = false;
	bool isPanning = false;
	bool isZooming = false;
	double baseX = 0.0, baseY = 0.0;
	glm::vec3 baseTrans = glm::vec3(0.0);
	glm::vec3 baseRot = glm::vec3(0.0);

	virtual void init() {};
	void draw(std::shared_ptr<ShaderProgram> shader);
	void update();
	void terminate();

	Light light;
	std::shared_ptr<Camera> camera = nullptr;
	std::vector<std::shared_ptr<Mesh>> mesh = {}; // later turn this into an array


	void Register(std::shared_ptr<IUpdatable> obj) {
		updatables.push_back(obj);
	}
};