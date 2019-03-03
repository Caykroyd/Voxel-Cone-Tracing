#version 450 core // Minimal GL version support expected from the GPU

struct LightSource {
	vec3 position;
	vec3 color;
	float intensity;
};

uniform LightSource lightSource;

struct Material {
	vec3 albedo;
	float shininess;
	float kd;
	float ks;
};

uniform Material material;

in vec3 fPosition; // Shader input, linearly interpolated by default from the previous stage (here the vertex shader)
in vec3 fNormal;
in vec2 fTexCoord;

out vec4 colorResponse; // Shader output: the color response attached to this fragment

void main() {
	vec3 n = normalize (fNormal); // Linear barycentric interpolation does not preserve unit vectors
	vec3 wi = normalize (lightSource.position - fPosition);
	vec3 wo = normalize (-fPosition);
	float lambertianTerm = max (0.0, dot (n, wi));
	//vec3 fr = material.albedo;
	vec3 ambientLight = 0.1 * lightSource.color;
	//vec3 Li = lambertianTerm * lightSource.color * lightSource.intensity;
	//vec3 radiance = (ambientLight + lambertianTerm) * vec3(1.0,0.0,0.0);
	
	vec3 fd = material.kd * material.albedo;
	vec3 wh = normalize(wi + wo);
	vec3 fs = vec3(1.0) * material.ks * pow(max(0.0,dot(wh,n)),material.shininess);
	
	vec3 Li = lightSource.color * lightSource.intensity;
	vec3 radiance = Li * (fd + fs) * max(0.0, dot(n, wi));
    colorResponse = vec4 (radiance, 1.0); // Building an RGBA value from an RGB one. 
	
	//colorResponse = vec4(n,1.0); // shows the normals as colors
}