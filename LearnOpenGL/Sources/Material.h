#pragma once

#include <glm/glm.hpp>

class Material
{
public:
	glm::vec3 albedo;
	float shininess;
	float kd;
	float ks;

	Material() : albedo(1.0, 0.0, 0.0), shininess(1.0), kd(0.8), ks(0.10) {};
	~Material();
};

