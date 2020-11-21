/*
 * CS330_Mod7-1_TAlberts.cpp
 *
 *    Due Date: June 21, 2020
 *      Author: Tim Alberts (timothy.alberts@snhu.edu)
 *       Class: CS-330-H5112 Comp Graphic and Visualization 20EW5
 *  Instructor: Kurt Diesch
 */


/* Header Inclusions */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <sstream>
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <GL/glew.h>
#include <GL/freeglut.h>

// GLM Math Header inclusions
#include <glm/glm.hpp>
#include <glm/trigonometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Image loading library
#include "SOIL2/SOIL2.h"

/* Shader Program Macro */
#ifndef GLSL
#define GLSL(Version, Source) "#version " #Version "\n" #Source
#endif

/* Subject Vertex Shader Source Code */
const GLchar * subjectVertexShaderSource = GLSL(330,
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
);

/* Subject Fragment Shader Source Code */
const GLchar * subjectFragmentShaderSource = GLSL(330,

	struct Material
	{
		vec3 ambient;
		sampler2D diffuseMap;
		vec3 specular;
		float shininess;
	};

	struct Light
	{
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
);

/* Lamp Vertex Shader Source Code */
const GLchar * lampVertexShaderSource = GLSL(330,

	layout (location = 0) in vec3 position; // VAO position 0 for vertex position data

	uniform mat4 model;
	uniform mat4 view;
	uniform mat4 projection;

	void main()
	{
		gl_Position = projection * view * model * vec4(position, 1.0f);
	}
);

/* Lamp Fragment Shader Source Code */
const GLchar * lampFragmentShaderSource = GLSL(330,

	out vec4 color;

	uniform vec3 lampColor;

	void main()
	{
		color = vec4(lampColor, 1.0f);
	}
);

/**
 * ShaderProgram class - encapsulate the loading and handling of shaders
 */
class ShaderProgram
{
public:
	ShaderProgram();
	~ShaderProgram();

	enum ShaderType
	{
		VERTEX,
		FRAGMENT,
		PROGRAM
	};

	bool loadShaders(const GLchar* vsSourcePtr, const GLchar* fsSourcePtr);
	void use();

	void setUniform(const GLchar* name, const glm::vec2& v);
	void setUniform(const GLchar* name, const glm::vec3& v);
	void setUniform(const GLchar* name, const glm::vec4& v);
	void setUniform(const GLchar* name, const glm::mat4& m);
	void setUniform(const GLchar* name, const GLfloat f);
	void setUniform(const GLchar* name, const GLint v);
	void setUniformSampler(const GLchar* name, const GLint& slot);

	GLuint getProgram() const;

private:

	void checkCompileErrors(GLuint shader, ShaderType type);
	GLint getUniformLocation(const GLchar* name);

	GLuint mHandle;
	std::map<std::string, GLint> mUniformLocations;
};

ShaderProgram::ShaderProgram()
	: mHandle(0)
{}

ShaderProgram::~ShaderProgram()
{
	glDeleteProgram(mHandle);
}

bool ShaderProgram::loadShaders(const GLchar* vsSourcePtr, const GLchar* fsSourcePtr)
{

	GLuint vs = glCreateShader(GL_VERTEX_SHADER); // create vertex shader
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER); // create fragment shader

	glShaderSource(vs, 1, &vsSourcePtr, NULL);
	glShaderSource(fs, 1, &fsSourcePtr, NULL);

	glCompileShader(vs);
	checkCompileErrors(vs, VERTEX);

	glCompileShader(fs);
	checkCompileErrors(fs, FRAGMENT);

	// Create the shader program
	mHandle = glCreateProgram();
	glAttachShader(mHandle, vs);
	glAttachShader(mHandle, fs);
	glLinkProgram(mHandle);

	checkCompileErrors(mHandle, PROGRAM);

	// House keeping (don't leak memory) - we already compiled and linked the shaders, so delete them.
	glDeleteShader(vs);
	glDeleteShader(fs);

	mUniformLocations.clear();
	return true;
}

void ShaderProgram::use()
{
	if (mHandle > 0)
	{
		glUseProgram(mHandle);
	}
}

void ShaderProgram::checkCompileErrors(GLuint shader, ShaderType type)
{
	int status = 0;

	if (type == PROGRAM)
	{
		glGetProgramiv(mHandle, GL_LINK_STATUS, &status);
		if (status == GL_FALSE)
		{
			GLint length = 0;
			glGetProgramiv(mHandle, GL_INFO_LOG_LENGTH, &length);

			std::string errorLog(length, ' ');
			glGetProgramInfoLog(mHandle, length, &length, &errorLog[0]);
			std::cerr << "Error! Program failed to link. " << errorLog << std::endl;
		}
	}
	else // VERTEX OR FRAGMENT
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE)
		{
			GLint length = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

			std::string errorLog(length, ' ');
			glGetShaderInfoLog(shader, length, &length, &errorLog[0]);
			std::cerr << "Error! Shader failed to compile. " << errorLog << std::endl;
		}
	}
}

GLuint ShaderProgram::getProgram() const
{
	return mHandle;
}

GLint ShaderProgram::getUniformLocation(const GLchar* name)
{
	std::map<std::string, GLint>::iterator it = mUniformLocations.find(name);

	if (it == mUniformLocations.end())
	{
		mUniformLocations[name] = glGetUniformLocation(mHandle, name);
	}

	return mUniformLocations[name];
}

void ShaderProgram::setUniform(const GLchar* name, const glm::vec2& v)
{
	GLint loc = getUniformLocation(name);
	glUniform2f(loc, v.x, v.y);
}

void ShaderProgram::setUniform(const GLchar* name, const glm::vec3& v)
{
	GLint loc = getUniformLocation(name);
	glUniform3f(loc, v.x, v.y, v.z);
}

void ShaderProgram::setUniform(const GLchar* name, const glm::vec4& v)
{
	GLint loc = getUniformLocation(name);
	glUniform4f(loc, v.x, v.y, v.z, v.w);
}

void ShaderProgram::setUniform(const GLchar* name, const glm::mat4& m)
{
	GLint loc = getUniformLocation(name);
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(m));
}

void ShaderProgram::setUniform(const GLchar* name, const GLfloat f)
{
	GLint loc = getUniformLocation(name);
	glUniform1f(loc, f);
}

void ShaderProgram::setUniform(const GLchar* name, const GLint v)
{
	GLint loc = getUniformLocation(name);
	glUniform1i(loc, v);
}

void ShaderProgram::setUniformSampler(const GLchar* name, const GLint& slot)
{
	glActiveTexture(GL_TEXTURE0 + slot);
	GLint loc = getUniformLocation(name);
	glUniform1i(loc, slot);
}


struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

/*
 * Mesh Class - encapsulate the loading of mesh from Wavefront Obj files and drawing
 */
class Mesh
{
public:
	Mesh();
	~Mesh();

	bool loadOBJ(const std::string& filename);
	void draw();

private:

	void initBuffers();

	bool mLoaded;
	std::vector<Vertex> mVertices;
	GLuint mVBO, mVAO;
};

/* Split a string on a delimiter*/
std::vector<std::string> split(std::string s, std::string t)
{
	std::vector<std::string> res;
	while (1)
	{
		int pos = s.find(t);
		if (pos == -1)
		{
			res.push_back(s);
			break;
		}
		res.push_back(s.substr(0, pos));
		s = s.substr(pos + 1, s.size() - pos - 1);
	}
	return res;
}

Mesh::Mesh()
	:mLoaded(false)
{}

Mesh::~Mesh()
{
	// Destroys Buffer objects
	glDeleteVertexArrays(1, &mVAO);
	glDeleteBuffers(1, &mVBO);
}

bool Mesh::loadOBJ(const std::string& filename)
{
	std::vector<unsigned int> vertexIndices, normalIndices, uvIndices;
	std::vector<glm::vec3> tempVertices;
	std::vector<glm::vec3> tempNormals;
	std::vector<glm::vec2> tempUVs;


	if (filename.find(".obj") != std::string::npos)
	{
		std::ifstream fin(filename, std::ios::in);
		if (!fin)
		{
			std::cerr << "Cannot open " << filename << std::endl;
			return false;
		}

		std::cout << "Loading OBJ file " << filename << " ..." << std::endl;

		std::string lineBuffer;
		while (std::getline(fin, lineBuffer))
		{
			std::stringstream ss(lineBuffer);
			std::string cmd;
			ss >> cmd;



			if (cmd == "v")
			{
				glm::vec3 vertex;
				int dim = 0;
				while (dim < 3 && ss >> vertex[dim])
					dim++;

				tempVertices.push_back(vertex);
			}
			else if (cmd == "vt")
			{
				glm::vec2 uv; // ignores the third component 'w'
				int dim = 0;
				while (dim < 2 && ss >> uv[dim])
					dim++;

				tempUVs.push_back(uv);
			}
			else if (cmd == "vn")
			{
				glm::vec3 normal;
				int dim = 0;
				while (dim < 3 && ss >> normal[dim])
					dim++;

				normal = glm::normalize(normal); // normalize the normal for magnitude of 1
				tempNormals.push_back(normal);
			}
			else if (cmd == "f")
			{
				// Note: only loads triangles ... will fail if there are quads
				// does allow for faces to not have texture

				std::string faceData;
				int vertexIndex, uvIndex, normalIndex; /* v/vt/vn */

				while (ss >> faceData)
				{
					std::vector<std::string> data = split(faceData, "/");

					// vertex index
					if (data[0].size() > 0)
					{
						sscanf_s(data[0].c_str(), "%d", &vertexIndex);
						vertexIndices.push_back(vertexIndex);
					}

					// if the face vertex has a texture coordinate index
					if (data.size() >= 1)
					{
						if (data[1].size() > 0)
						{
							sscanf_s(data[1].c_str(), "%d", &uvIndex);
							uvIndices.push_back(uvIndex);
						}
					}

					// does the face vertex have a normal index
					if (data.size() >= 2)
					{
						if (data[2].size() > 0)
						{
							sscanf_s(data[2].c_str(), "%d", &normalIndex);
							normalIndices.push_back(normalIndex);
						}
					}
				}
			}
		}

		// Close the file
		fin.close();


		// For each vertex of each triangle
		for (unsigned int i = 0; i < vertexIndices.size(); i++)
		{
			Vertex meshVertex;

			if (tempVertices.size() > 0)
			{
				glm::vec3 vertex = tempVertices[vertexIndices[i] - 1];
				meshVertex.position = vertex;
			}

			if (tempNormals.size() > 0)
			{
				glm::vec3 normal = tempNormals[normalIndices[i] - 1];
				meshVertex.normal = normal;
			}

			if (tempUVs.size() > 0)
			{
				glm::vec2 uv = tempUVs[uvIndices[i] - 1];
				meshVertex.texCoords = uv;
			}

			mVertices.push_back(meshVertex);
		}

		// Create and initialize the buffers
		initBuffers();

		return (mLoaded = true);
	}

	// Shouldn't get here so return failure
	return false;
}

void Mesh::draw()
{
	if(!mLoaded) return;

	glBindVertexArray(mVAO);
	glDrawArrays(GL_TRIANGLES, 0, mVertices.size());

	glBindVertexArray(0); // 'unbind'
}

void Mesh::initBuffers()
{
	glGenBuffers(1, &mVBO); // create one vbo and save the identifier in vbo ... creates a chunk of memory in the graphics card
	glBindBuffer(GL_ARRAY_BUFFER, mVBO); // define the type of buffer and give it an identifier ... makes this buffer the current buffer
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW); // give the buffer type, size and content along with how to treat the buffer (GL_STATIC_DRAW, GL_DYNAMIC_DRAW, GL_STREAM_DRAW) ... meaning how often the data will be updated

	glGenVertexArrays(1, &mVAO); // create 1 vao and store identifier
	glBindVertexArray(mVAO); // make it the active one

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL); // tell the graphics card how to interpret the buffer data
	glEnableVertexAttribArray(0); // enable the vao

	// Normals
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1); // enable the vao

	// tex coord
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2); // enable the vao

	glBindVertexArray(0); // 'unbind'
}

/*
 * Texture2D Class - encapsulate loading and handling of textures
 */
class Texture2D
{
public:
	Texture2D();
	virtual ~Texture2D();

	void loadTexture(const char* filename);
	void bind(GLuint texUnit = 0);
	void unbind(GLuint texUnit = 0);

private:

	GLuint mTexture;

};

Texture2D::Texture2D()
	: mTexture(0)
{}

Texture2D::~Texture2D()
{}

void Texture2D::loadTexture(const char* filename)
{
	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);

	int width, height;

	unsigned char* image = SOIL_load_image(filename, &width, &height, 0, SOIL_LOAD_RGB);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::bind(GLuint texUnit)
{
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, mTexture);
}

void Texture2D::unbind(GLuint texUnit)
{
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, 0);
}


/*
 * Camera Class - encapsulate the behavior of an orbit style camera
 */
class Camera
{
public:
	Camera();

	glm::mat4 getViewMatrix() const;
	void rotate(float yaw, float pitch); // rotate camera on XY axis ... in degrees
	void setLookAt(const glm::vec3& target);
	void setRadius(float radius);
	const glm::vec3& getPosition() const;

private:

	void updateCameraVectors();

	glm::vec3 mPosition;
	glm::vec3 mTargetPos;
	glm::vec3 mUp;

	// Eulers angles (in radians)
	float mYaw;
	float mPitch;
	float mRadius;
};

Camera::Camera()
	:mPosition(glm::vec3(0.0f, 0.0f, 0.0f)),
	mTargetPos(glm::vec3(0.0f, 0.0f, 0.0f)),
	mUp(0.0f, 1.0f, 0.0f), // Y is up
	mYaw(0.0f),
	mPitch(0.0f),
	mRadius(0.0f)
{}

glm::mat4 Camera::getViewMatrix() const
{
	return glm::lookAt(mPosition, mTargetPos, mUp);
}

void Camera::setLookAt(const glm::vec3& target)
{
	mTargetPos = target;
}

void Camera::setRadius(float radius)
{
	mRadius = glm::clamp(radius, 2.0f, 80.0f);
}

const glm::vec3& Camera::getPosition() const
{
	return mPosition;
}

void Camera::rotate(float yaw, float pitch)
{
	mYaw = glm::radians(yaw);
	mPitch = glm::radians(pitch);

	mPitch = glm::clamp(mPitch, -glm::pi<float>() / 2.0f + 0.1f, glm::pi<float>() / 2.0f - 0.1f);
	updateCameraVectors();
}

void Camera::updateCameraVectors()
{
	// Spherical to Cartesian coordinates
	// https://en.wikipedia.org/wiki/Spherical_coordinate_system (Note: our coordinate system has Y up not Z)
	mPosition.x = mTargetPos.x + mRadius * cosf(mPitch) * sinf(mYaw);
	mPosition.y = mTargetPos.y + mRadius * sinf(mPitch);
	mPosition.z = mTargetPos.z + mRadius * cosf(mPitch) * cosf(mYaw);
}


/*
 * Function Prototypes
 */
void InitializeApp(int, char*[]);
void InitializeWindow(int, char*[]);
void TimerFunction(int);
void IdleFunction(void);
void HandleInitError(const char *fmt, va_list ap);
void HandleInitWarning(const char *fmt, va_list ap);
void HandleWindowResize(int, int);
void RenderGraphics(void);
void HandleKeyboard(unsigned char key, int x, int y);
void HandleMouseClick(int button, int state, int x, int y);
void HandleMousePressedMove(int x, int y);


/*
 * Global variables
 */
unsigned frame_count = 0;

/*
 * Window configuration
 */
char title[] = "Tim Alberts (Module 6-2)";
int default_position_x = 50;
int default_position_y = 50;
float default_bg_r = 0.0f;
float default_bg_g = 0.05f;
float default_bg_b = 0.1f;
float default_bg_a = 0.0f;
GLint WindowWidth = 1024, WindowHeight = 768;

/*
 * Subject configuration
 */
#define SUBJECT_MESH_FILENAME "DollHighChair03.obj"
#define SUBJECT_TEXTURE_FILENAME "snapshot.png"
glm::vec3 subjectPosition = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 subjectScale = glm::vec3(0.6f);

Mesh subjectMesh;
ShaderProgram subjectShader;
Texture2D subjectTexture;

/*
 * Lighting configuration (key, fill, no backlight)
 */
#define LAMP_MESH_FILENAME "LightSphere.obj"
glm::vec3 keyLightPosition = glm::vec3(4.0f, 4.0f, -4.0f);
glm::vec3 keyLightScale = glm::vec3(0.1f, 0.1f, 0.1f);
glm::vec3 keyLightColor(1.0f, 1.0f, 1.0f); // White
float keyLightIntensity = 1.0f;
Mesh keyLightMesh;

glm::vec3 fillLightPosition = glm::vec3(-4.0f, 1.0f, 4.0f);
glm::vec3 fillLightScale = glm::vec3(0.1f, 0.1f, 0.1f);
glm::vec3 fillLightColor(0.8f, 0.8f, 1.0f); // Light Blue
float fillLightIntensity = 0.3f;
Mesh fillLightMesh;

ShaderProgram lampShader;

/**
 * Camera configuration
 */
glm::vec3 cameraPosition(0.0f, 0.0f, -6.0f);
float cameraRotation = glm::radians(-25.0f);

Camera orbitCamera;
float gYaw = 0.0f;
float gPitch = 0.0f;
float gRadius = 12.0f;
const float MOUSE_SENSITIVITY = 0.25f;

/*
 * Interface state variables
 */
bool mode_wireframe = false;
bool mode_perspective = true;
bool leftMouse = false;
bool rightMouse = false;


/* Main Program */
int main(int argc, char* argv[])
{
	InitializeApp(argc, argv);
	glutMainLoop();

	exit(EXIT_SUCCESS); // Terminates the program successfully
}

// Implements the InitializeApp function
void InitializeApp(int argc, char* argv[])
{
	InitializeWindow(argc, argv); // Creates the window

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Display GPU OpenGL version
	fprintf(stdout, "INFO: OpenGL Version: %s\n", glGetString(GL_VERSION));

	// Load shaders
	subjectShader.loadShaders(subjectVertexShaderSource, subjectFragmentShaderSource);
	lampShader.loadShaders(lampVertexShaderSource, lampFragmentShaderSource);

	// Load mesh objects
	if(!subjectMesh.loadOBJ(SUBJECT_MESH_FILENAME) )
	{
		std::cout << "Failed to load subject Object file!" << std::endl;
		exit(EXIT_FAILURE);
	}

	if(!keyLightMesh.loadOBJ(LAMP_MESH_FILENAME) )
	{
		std::cout << "Failed to load lamp Object file!" << std::endl;
		exit(EXIT_FAILURE);
	}
	if(!fillLightMesh.loadOBJ(LAMP_MESH_FILENAME) )
	{
		std::cout << "Failed to load lamp Object file!" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Load Textures
	subjectTexture.loadTexture(SUBJECT_TEXTURE_FILENAME);

	glViewport(0, 0, WindowWidth, WindowHeight);
	glEnable(GL_DEPTH_TEST); // Enable z-depth
	glClearColor(default_bg_r, default_bg_g, default_bg_b, default_bg_a);
}

void InitializeWindow(int argc, char* argv[])
{
	// Initializes FreeGLUT
	glutInit(&argc, argv);

	// Show version info
	fprintf(stdout,"INFO: GLUT_VERSION=%d\n", glutGet(GLUT_VERSION));

	// Memory buffer setup for display
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(WindowWidth, WindowHeight);
	glutInitWindowPosition(default_position_x, default_position_y);

	// handle initialization errors and warnings
	glutInitErrorFunc(&HandleInitError);
	glutInitWarningFunc(&HandleInitWarning);

	glutCreateWindow(title);

	glutReshapeFunc(HandleWindowResize);

	glutDisplayFunc(RenderGraphics);

	// Set the idle callback function
	glutIdleFunc(IdleFunction);

	// Set the timer callback function to run immediately
	glutTimerFunc(0, TimerFunction, 0);

	glutKeyboardFunc(HandleKeyboard);

	glutMouseFunc(HandleMouseClick);
	glutMotionFunc(HandleMousePressedMove);
}

/*
 * tell freeGlut to redraw asap for performance
 */
void IdleFunction(void)
{
	glutPostRedisplay();
}

/*
 * Update the application window title every quarter second to display the FPS.
 */
void TimerFunction(int Value)
{
	// Don't update on the first run (since nothing's been drawn yet)
	if (0 != Value) {
		// Generate the updated title
		std::ostringstream outs;
		outs.precision(3);
		outs << std::fixed
			<< title << ":  "
			<< frame_count * 4 << " FPS";

		glutSetWindowTitle(outs.str().c_str());
	}

	// reset the counter and set this function to run again.
	frame_count = 0;
	glutTimerFunc(250, TimerFunction, 1);
}

/*
 * Handle freeGlut errors when initializing
 */
void HandleInitError(const char *fmt, va_list ap) {
	// Log the error message
	vfprintf(stderr, fmt, ap);
	// Cleanup resources
	glutExit();
	// Signal an error
	exit(EXIT_FAILURE);
}

/*
 * Handle freeGlut warnings when initializing
 */
void HandleInitWarning(const char *fmt, va_list ap) {
	// Log the warning message
	vfprintf(stderr, fmt, ap);
}

/*
 * Handle Window Resize
 */
void HandleWindowResize(int w, int h)
{
	WindowWidth = w;
	WindowHeight = h;
	glViewport(0, 0, WindowWidth, WindowHeight);
}

/*
 * Render Graphics Implementation
 */
void RenderGraphics(void)
{
	// track the frame count
	++frame_count;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clears the screen

	glm::mat4 model, view, projection;

	// Transform the camera
	orbitCamera.setLookAt(subjectPosition);
	orbitCamera.rotate(gYaw, gPitch);
	orbitCamera.setRadius(gRadius);
	view = orbitCamera.getViewMatrix();

	// Create perspective or orthographic projection
	if(mode_perspective)
	{
		projection = glm::perspective(glm::radians(45.0f), (GLfloat)WindowWidth / (GLfloat)WindowHeight, 0.1f, 100.0f);
	}
	else
	{
		projection = glm::ortho(-5.0f,5.0f,-5.0f,5.0f,0.1f,100.0f);
	}

	subjectShader.use();

	subjectShader.setUniform("view", view);
	subjectShader.setUniform("projection", projection);
	subjectShader.setUniform("viewPosition", orbitCamera.getPosition());

	subjectShader.setUniform("lights[0].ambient", glm::vec3(0.1f, 0.1f, 0.1f));
	subjectShader.setUniform("lights[0].diffuse", keyLightColor);
	subjectShader.setUniform("lights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));// white
	subjectShader.setUniform("lights[0].position", keyLightPosition);
	subjectShader.setUniform("lights[0].intensity", keyLightIntensity);

	subjectShader.setUniform("lights[1].ambient", glm::vec3(0.1f, 0.1f, 0.1f));
	subjectShader.setUniform("lights[1].diffuse", fillLightColor);
	subjectShader.setUniform("lights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));// white
	subjectShader.setUniform("lights[1].position", fillLightPosition);
	subjectShader.setUniform("lights[1].intensity", fillLightIntensity);


	// Transform the subject into the world
	model = glm::translate(model, subjectPosition);
	model = glm::scale(model, subjectScale);

	subjectShader.setUniform("model", model);

	subjectShader.setUniform("material.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	subjectShader.setUniformSampler("material.diffuseMap", 0);
	subjectShader.setUniform("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
	subjectShader.setUniform("material.shininess", 64.0f);

	subjectTexture.bind(0);
	subjectMesh.draw();
	subjectTexture.unbind(0);


	// Render the key light
	model = glm::translate(glm::mat4(), keyLightPosition);
	model = glm::scale(model, keyLightScale);
	lampShader.use();
	lampShader.setUniform("lampColor", keyLightColor);
	lampShader.setUniform("model", model);
	lampShader.setUniform("view", view);
	lampShader.setUniform("projection", projection);
	keyLightMesh.draw();


	// Render the fill light
	model = glm::translate(glm::mat4(), fillLightPosition);
	model = glm::scale(model, fillLightScale);
	lampShader.use();
	lampShader.setUniform("lampColor", fillLightColor);
	lampShader.setUniform("model", model);
//	lampShader.setUniform("view", view);
//	lampShader.setUniform("projection", projection);
	fillLightMesh.draw();

	glutSwapBuffers(); // Flip the front and back buffers (equivalent to GL flush)
}

/*
 * Handle keyboard events
 */
void HandleKeyboard(unsigned char key, GLint x, GLint y)
{
	switch(key) {
	case 'W':
	case 'w':
		std::cout << "Toggle Wireframe Mode" << std::endl;
		mode_wireframe = !mode_wireframe;
		if (mode_wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;

	case 'O':
	case 'o':
		std::cout << "Orthographic" << std::endl;
		mode_perspective = false;
		break;

	case 'P':
	case 'p':
		std::cout << "Perspective" << std::endl;
		mode_perspective = true;
		break;
	}
}

/*
 * Handle mouse click events
 */
void HandleMouseClick(int button, int state, int x, int y)
{
	if((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)){
		leftMouse = true;
	}

	if((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP)){
		leftMouse = false;
	}

	if((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)){
		rightMouse = true;
	}

	if((button == GLUT_RIGHT_BUTTON) && (state == GLUT_UP)){
		rightMouse = false;
	}
}

/*
 * Handle MousePressedMove events
 */
void HandleMousePressedMove(int x, int y)
{
	static glm::vec2 lastMousePos = glm::vec2(0, 0);

	// Update angles based on Left Mouse Button input to orbit around the chair
	if (leftMouse)
	{
		// each pixel represents a quarter of a degree rotation (this is our mouse sensitivity)
		gYaw -= ((float)x - lastMousePos.x) * MOUSE_SENSITIVITY;
		gPitch += ((float)y - lastMousePos.y) * MOUSE_SENSITIVITY;
	}

	// Change orbit camera radius with the Right Mouse Button
	if (rightMouse)
	{
		float dx = 0.01f * ((float)x - lastMousePos.x);
		float dy = 0.01f * ((float)y - lastMousePos.y);
		gRadius += dx - dy;
	}

	lastMousePos.x = (float)x;
	lastMousePos.y = (float)y;
}

