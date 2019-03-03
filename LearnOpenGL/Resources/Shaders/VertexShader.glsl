#version 450 core // Minimal GL version support expected from the GPU

struct LightSource {
	vec3 position;
	vec3 color;
	float intensity;
};

uniform LightSource lightSource;

layout(location=0) in vec3 vPosition; // The 1st input attribute is the position (CPU side: glVertexAttrib 0)
layout(location=1) in vec3 vNormal;
layout(location=2) in vec2 vTexCoord;

uniform mat4 M_Projection, M_Model, M_View, M_Model_1t, M_View_1t; 

out vec3 fPosition;
out vec3 fNormal;
out vec3 fLightPosition;
out vec2 fTexCoord;

void main() {
	vec4 p = M_View * M_Model * vec4 (vPosition, 1.0);
    gl_Position =  M_Projection * p; // mandatory to fire rasterization properly
    vec4 n = M_View_1t * M_Model_1t * vec4 (vNormal, 1.0);
	fLightPosition = vec3(M_View * vec4(lightSource.position, 1.0));
    fPosition = p.xyz;
    fNormal = normalize (n.xyz);
    fTexCoord = vTexCoord;
}