#version 450 core // Minimal GL version support expected from the GPU

layout(location=0) in vec3 vPosition; // The 1st input attribute is the position (CPU side: glVertexAttrib 0)
layout(location=1) in vec3 vNormal; // previously: vColor The 2nd input attribute is the vertex color (CPU side: glVertexAttrib 1)

uniform mat4 projectionMat, modelMat, viewMat, normalMat; 

out vec3 fPosition;
out vec3 fNormal; // The vertex shader outpus a vec3 capturing vertex color

void main() {
    vec4 p =  viewMat * modelMat * vec4 (vPosition, 1.0);; // mandatory to fire rasterization properly
	glPosition = projectionMat * p;
	fPosition = p.xyz;
	vec4 n = normalMat * vec4(vNormal, 1.0);
	fNormal = normalize(n.xyz);
}