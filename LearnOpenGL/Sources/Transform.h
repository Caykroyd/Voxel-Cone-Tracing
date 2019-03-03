#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/ext.hpp>

/// A base class to represent spatially embedded entities in the scene. 
class Transform {
public:
	Transform () : position (0.0), rotation (0.0), scale (1.0) {}
	virtual ~Transform () {}
	
	inline const glm::vec3 getTranslation () const { return position; }
	inline void setTranslation (const glm::vec3 & t) { position = t; }
	inline const glm::vec3 getRotation () const { return rotation; }
	inline void setRotation (const glm::vec3 & r) { rotation = r; }
	inline glm::vec3 getScale () const { return scale; }
	inline void setScale (float s) { scale = glm::vec3(s); }
	inline void setScale(const glm::vec3 & s) { scale = s; }

	inline const glm::vec3 up() const { return rotation * glm::vec3(0, 1.0, 0); }
	inline const glm::vec3 right() const { return rotation * glm::vec3(1.0, 0, 0); }
	inline const glm::vec3 forward() const { return rotation * glm::vec3(0, 0, -1.0); }

	inline glm::mat4 computeTransformMatrix () const {
		glm::mat4 id (1.0);
		glm::mat4 sm = glm::scale (id, scale);
		glm::mat4 rsm = glm::rotate (sm, rotation[0], glm::vec3 (1.0, 0.0, 0.0));
		rsm = glm::rotate (rsm, rotation[1], glm::vec3 (0.0, 1.0, 0.0));
		rsm = glm::rotate (rsm, rotation[2], glm::vec3 (0.0, 0.0, 1.0));
		glm::mat4 trsm = glm::translate (rsm, position);
		return trsm;
	}

	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
};

#endif // TRANSFORM_H