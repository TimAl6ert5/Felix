#version 330 core

struct Material {
	vec3 ambient;
	sampler2D diffuseMap;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float intensity;
};

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragmentPos;

out vec4 frag_color; // outgoing color to the GPU

uniform Light lights[2];
uniform Material material;
uniform vec3 viewPosition;

vec4 calcLight(Light light);

void main() {
	// Initialize the color
	frag_color = vec4(0.0f);

	// Loop over the light sources
	for (int i = 0; i < 2; i++) {
		frag_color += calcLight(lights[i]);
	}
}

vec4 calcLight(Light light) {
    // Ambient
    //vec3 ambient = light.ambient * material.ambient;
    vec3 ambient = light.ambient * material.ambient * vec3(texture(material.diffuseMap, TexCoord));

    // Diffuse
    vec3 normal = normalize(Normal); // Normalize vectors to 1 unit
    vec3 lightDir = normalize(light.position - FragmentPos); // Calculate distance (light direction) between light source and fragments/pixels on
    float NDotL = max(dot(normal, lightDir), 0.0f); // Calculate diffuse impact by generating dot product of normal and light (clamped)
    vec3 diffuse = light.diffuse * vec3(texture(material.diffuseMap, TexCoord)) * NDotL;

    // Specular (Blinn-Phong)
    vec3 viewDir = normalize(viewPosition - FragmentPos); // Calculate view direction
    vec3 halfDir = normalize(lightDir + viewDir); // Blinn half vector between the light and the view
    float NDotH = max(dot(normal, halfDir), 0.0f); // dot normal and half vectors and clamp
    vec3 specular = light.specular * material.specular * pow(NDotH, material.shininess);

    return vec4(ambient + diffuse + specular, 1.0f) * light.intensity;
}
