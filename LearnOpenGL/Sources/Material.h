#pragma once

#include <glm/glm.hpp>
#include "ShaderProgram.h"
#include "ShaderProperty.h"

class Material : public ShaderProperty
{
public:
	glm::vec3 albedo;
	float shininess;
	float kd;
	float ks;

	void SendToShader(ShaderProgram& shader) const override {
		shader.set("material.albedo", albedo);
		shader.set("material.shininess", shininess);
		shader.set("material.kd", kd);
		shader.set("material.ks", ks);
	}

	Material() : albedo(1.0, 0.0, 0.0), shininess(1.0), kd(0.8), ks(0.10) {};
	~Material();
};

