#define _USE_MATH_DEFINES

#include "Mesh.h"

#include <cmath>
#include <algorithm>
#include <functional>
#include <list>

#include <iostream>

using namespace std;

Mesh::~Mesh () {
	clear ();
}

void Mesh::computeBoundingSphere (glm::vec3 & center, float & radius) const {
	center = glm::vec3 (0.0);
	radius = 0.f;
	for (const auto & p : m_vertexPositions)
		center += p;
	center /= m_vertexPositions.size ();
	for (const auto & p : m_vertexPositions)
		radius = std::max (radius, distance (center, p));
}

void Mesh::recomputePerVertexNormals (bool angleBased) {
	m_vertexNormals.clear ();
	// Change the following code to compute a proper per-vertex normal
	m_vertexNormals.resize (m_vertexPositions.size (), glm::vec3 (0.0, 0.0, 1.0));


	const unsigned int n = m_vertexPositions.size();
	std::vector<std::list<glm::vec3>> neighbours(n);
	for (glm::vec3 triangle : m_triangleIndices) {
		neighbours[triangle.x].push_back(triangle);
		neighbours[triangle.y].push_back(triangle);
		neighbours[triangle.z].push_back(triangle);
	}

	for (unsigned int i = 0; i < m_vertexPositions.size(); ++i) {
		glm::vec3 normal(0.0);
		if (neighbours[i].size() == 0)
			cout << "WTF " << i << endl;
		for (const auto & t : neighbours[i]) {
			float k = 1 / glm::distance(m_vertexPositions[t.y], m_vertexPositions[t.x]) 
							/ glm::distance(m_vertexPositions[t.z], m_vertexPositions[t.x]);
			// by computing the cross product, I obtain the normal vector with magnitude proportional to the area of the triangle
			normal += glm::cross(m_vertexPositions[t.y]- m_vertexPositions[t.x], m_vertexPositions[t.z] - m_vertexPositions[t.x]) * k;
		}
		m_vertexNormals[i] = glm::normalize(normal);
	}
}

void Mesh::init () {
	glCreateBuffers (1, &m_posVbo); // Generate a GPU buffer to store the positions of the vertices
	size_t vertexBufferSize = sizeof (glm::vec3) * m_vertexPositions.size (); // Gather the size of the buffer from the CPU-side vector
	glNamedBufferStorage (m_posVbo, vertexBufferSize, NULL, GL_DYNAMIC_STORAGE_BIT); // Create a data store on the GPU
	glNamedBufferSubData (m_posVbo, 0, vertexBufferSize, m_vertexPositions.data ()); // Fill the data store from a CPU array
	
	glCreateBuffers (1, &m_normalVbo); // Same for normal
	glNamedBufferStorage (m_normalVbo, vertexBufferSize, NULL, GL_DYNAMIC_STORAGE_BIT); 
	glNamedBufferSubData (m_normalVbo, 0, vertexBufferSize, m_vertexNormals.data ());
	
	glCreateBuffers (1, &m_texCoordVbo); // Same for texture coordinates
	size_t texCoordBufferSize = sizeof (glm::vec2) * m_vertexTexCoords.size ();
	glNamedBufferStorage (m_texCoordVbo, texCoordBufferSize, NULL, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferSubData (m_texCoordVbo, 0, texCoordBufferSize, m_vertexTexCoords.data ());

	glCreateBuffers (1, &m_ibo); // Same for the index buffer, that stores the list of indices of the triangles forming the mesh
	size_t indexBufferSize = sizeof (glm::uvec3) * m_triangleIndices.size ();
	glNamedBufferStorage (m_ibo, indexBufferSize, NULL, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferSubData (m_ibo, 0, indexBufferSize, m_triangleIndices.data ());
	
	glCreateVertexArrays (1, &m_vao); // Create a single handle that joins together attributes (vertex positions, normals) and connectivity (triangles indices)
	glBindVertexArray (m_vao);
	glEnableVertexAttribArray (0);
	glBindBuffer (GL_ARRAY_BUFFER, m_posVbo);
	glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (GLfloat), 0);
	glEnableVertexAttribArray (1);
	glBindBuffer (GL_ARRAY_BUFFER, m_normalVbo);
	glVertexAttribPointer (1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (GLfloat), 0);
	glEnableVertexAttribArray (2);
	glBindBuffer (GL_ARRAY_BUFFER, m_texCoordVbo);
	glVertexAttribPointer (2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof (GLfloat), 0);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBindVertexArray (0); // Desactive the VAO just created. Will be activated at rendering time. 
}

void Mesh::render () {
	glBindVertexArray (m_vao); // Activate the VAO storing geometry data
	glDrawElements (GL_TRIANGLES, static_cast<GLsizei> (m_triangleIndices.size () * 3), GL_UNSIGNED_INT, 0); // Call for rendering: stream the current GPU geometry through the current GPU program
}

void Mesh::clear() {
	m_vertexPositions.clear();
	m_vertexNormals.clear();
	m_vertexTexCoords.clear();
	m_triangleIndices.clear();
	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0;
	}
	if (m_posVbo) {
		glDeleteBuffers(1, &m_posVbo);
		m_posVbo = 0;
	}
	if (m_normalVbo) {
		glDeleteBuffers(1, &m_normalVbo);
		m_normalVbo = 0;
	}
	if (m_texCoordVbo) {
		glDeleteBuffers(1, &m_texCoordVbo);
		m_texCoordVbo = 0;
	}
	if (m_ibo) {
		glDeleteBuffers(1, &m_ibo);
		m_ibo = 0;
	}
}

std::shared_ptr<Mesh> Mesh::primitiveSphere(int resolution, std::shared_ptr <Mesh> sphere) {
	sphere ->clear();

	int re_2 = int(resolution / 2);

	// vertex positions as a uniform angle parametrization
	// vertex normals are the same as positions, for the sphere. Note that they already come normalized!
	sphere->m_vertexPositions.push_back(glm::vec3(0, 0, 1));
	sphere->m_vertexNormals.push_back(glm::vec3(0, 0, 1));
	for (int step_T = 0; step_T < resolution; step_T++) {
		double theta = 2 * M_PI * step_T / (resolution-1);
		for (int step_P = 0; step_P < re_2; step_P++) {
			double phi = M_PI * step_P / re_2;
			sphere->m_vertexPositions.push_back(glm::vec3(std::sin(phi)*std::cos(theta), std::sin(phi)*std::sin(theta), std::cos(phi)));
			sphere->m_vertexNormals.push_back(glm::vec3(std::sin(phi) * std::cos(theta), std::sin(phi)*std::sin(theta), std::cos(phi)));
		}
	}
	sphere->m_vertexPositions.push_back(glm::vec3(0, 0, -1));
	sphere->m_vertexNormals.push_back(glm::vec3(0, 0, -1));
	
	int size = sphere->m_vertexPositions.size();

	int k = (resolution - 1) * re_2;
	// Computing triangle indices (except for poles)
	for (int step_T = 0; step_T < resolution; step_T++) {
		int step_T2 = (step_T + 1) % resolution;

		for (int step_P = 1; step_P < re_2; step_P++) {
			int step_P2 = step_P + 1;

			// triangle 1: normals pointing outwards 
			//sphere->m_triangleIndices.push_back(glm::uvec3(1+thisIndex, 1+(thisIndex + 1 + re_2)%k, 1+thisIndex + 1));
			sphere->m_triangleIndices.push_back(glm::uvec3(step_T*re_2+step_P, step_T*re_2 + step_P2, step_T2*re_2 + step_P2));

			// triangle 2: normals pointing outwards
			//sphere->m_triangleIndices.push_back(glm::uvec3(1+thisIndex, 1+(thisIndex + re_2)%k, 1+(thisIndex + 1 + re_2)%k));
			sphere->m_triangleIndices.push_back(glm::uvec3(step_T*re_2 + step_P, step_T2*re_2 + step_P2, step_T2*re_2 + step_P));

		}
		// computing triangles in poles
		sphere->m_triangleIndices.push_back(glm::uvec3(0, step_T * re_2 + 1, step_T2 * re_2 + 1));

		sphere->m_triangleIndices.push_back(glm::uvec3(size - 1, step_T2 * re_2 + (re_2 - 1), step_T * re_2 + (re_2-1)));
	}	

	vector<bool> belongsToTriangle = vector<bool>(sphere->m_vertexPositions.size(), false);
	for (glm::vec3 triangle : sphere->m_triangleIndices) {
		belongsToTriangle[triangle.x] = true;
		belongsToTriangle[triangle.y] = true;
		belongsToTriangle[triangle.z] = true;
	}
	for (unsigned int i = 0; i < belongsToTriangle.size(); ++i) {
		if (!belongsToTriangle[i])
			cout << i << endl;
	}

	// vertex texture coordinates as a cilindrical projection
	sphere->m_vertexTexCoords.push_back(glm::vec2(0, 0));
	for (int step_T = 0; step_T < resolution; step_T++) {
		double theta = 2 * M_PI * step_T / (resolution-1);
		for (int step_P = 0; step_P < re_2; step_P++) {
			double phi = M_PI * step_P / re_2;
			sphere->m_vertexTexCoords.push_back(glm::vec2(std::sin(phi) * std::cos(theta), std::sin(phi)*std::sin(theta)));
		}
	}
	sphere->m_vertexTexCoords.push_back(glm::vec2(0, 0));
	
	//sphere->recomputePerVertexNormals();

	return sphere;
}