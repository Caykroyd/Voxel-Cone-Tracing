#version 450 core // Minimal GL version support expected from the GPU

struct LightSource {
	vec3 position;
	vec3 color;
	float intensity;
}

uniform LightSource lightSource;

in vec3 fPosition;
in vec3 fNormal; // Shader input, linearly interpolated by default from the previous stage (here the vertex shader)

out vec4 colorResponse; // Shader output: the color response attached to this fragment

void main() {
	vec3 wi = normalize (lightSource.position – fPosition);
	float lambertianTerm = max (0.0, dot (fNormal, wi));
	vec3 radiance = lambertianTerm * lightSource.color * lightSource.intensity; 
	colorResponse = vec4 (radiance, 1.0);
}