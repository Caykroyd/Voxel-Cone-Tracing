#pragma once

#include <glm/glm.hpp>
#include "Shader/ShaderProgram.h"
#include "Shader/ShaderProperty.h"

class Material : public ShaderProperty
{
public:
	glm::vec3 albedo;
	float shininess;
	float kd;
	float ks;

	void SendToShader(ShaderProgram& shader) override {
		shader.set("material.albedo", albedo);
		shader.set("material.shininess", shininess);
		shader.set("material.kd", kd);
		shader.set("material.ks", ks);
	}

	Material() : albedo(1.0, 0.0, 0.0), shininess(1.0f), kd(0.8f), ks(0.10f) {};
	~Material();
};

