#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

// Global variables for the transform matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;
out vec3 Normal; // For outgoing normals to fragment shader
out vec3 FragmentPos; // For outgoing color / pixels to fragment shader

void main() {
	gl_Position = projection * view * model * vec4(position, 1.0f); // transforms vertex to clip coordinates

	FragmentPos = vec3(model * vec4(position, 1.0f)); // Gets fragment / pixel position in world space only (exclude view and projection)

	TexCoord = vec2(texCoord.x, 1.0f - texCoord.y); // flips the texture horizontally ... TODO better to flip the texture at load time?

	Normal = mat3(transpose(inverse(model))) * normal; // get normal vectors in world space only and exclude normal translation properties
}
