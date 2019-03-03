#pragma once
#include <glm/glm.hpp>

class Light
{
private:
	glm::vec3 m_position;
	glm::vec3 m_color;
	float m_intensity;
public:
	Light() : m_position(0.0), m_color(0.0), m_intensity(0.0) {};
	virtual ~Light();

	void SetPosition(glm::vec3 position) { m_position = position; };
	glm::vec3 GetPosition() { return m_position; };
};

