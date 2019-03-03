#pragma once
#include <glm/glm.hpp>

struct Light
{
public:
	Light(glm::vec3 l_position = glm::vec3(0.0), glm::vec3 l_color = glm::vec3(1.0), float l_intensity = 1.0) : position(l_position), color(l_color), intensity(l_intensity) {};
	glm::vec3 position;
	glm::vec3 color;
	float intensity;
};

