/*
 *    Due Date: 2020-11-15
 *      Author: Tim Alberts (timothy.alberts@snhu.edu)
 *       Class: CS-499-X2150 Computer Science Capstone 20EW2
 *  Instructor: Joseph Conlan
 *  	Module: 3-2
 */

#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "camera.h"
#include "mesh.h"
#include "shader_program.h"
#include "texture2D.h"

//#define RUN_TESTS

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
GLint window_width = 1024, window_height = 768;

/*
 * Subject configuration
 */
std::string subject_mesh_filename;
std::string subject_texture_filename;
glm::vec3 subject_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 subject_scale = glm::vec3(0.6f);

const char* kSubjectVertexShaderSrc = "shader_subject.vert";
const char* kSubjectFragmentShaderSrc = "shader_subject.frag";

Mesh subject_mesh;
ShaderProgram subject_shader;
Texture2D subject_texture;

/*
 * Lighting configuration (key, fill, no backlight)
 */
const std::string kLampMeshFilename = "LightSphere.obj";
glm::vec3 key_light_position = glm::vec3(4.0f, 4.0f, -4.0f);
glm::vec3 key_light_scale = glm::vec3(0.1f, 0.1f, 0.1f);
glm::vec3 key_light_color(1.0f, 1.0f, 1.0f); // White
float key_light_intensity = 1.0f;
Mesh key_light_mesh;

glm::vec3 fill_light_position = glm::vec3(-4.0f, 1.0f, 4.0f);
glm::vec3 fill_light_scale = glm::vec3(0.1f, 0.1f, 0.1f);
glm::vec3 fill_light_color(0.8f, 0.8f, 1.0f); // Light Blue
float fill_light_intensity = 0.3f;
Mesh fill_light_mesh;

const char* kLightVertexShaderSrc = "shader_lighting.vert";
const char* kLightFragmentShaderSrc = "shader_lighting.frag";

ShaderProgram lamp_shader;

/**
 * Camera configuration
 */
glm::vec3 camera_position(0.0f, 0.0f, -6.0f);
float camera_rotation = glm::radians(-25.0f);

Camera orbit_camera;
float camera_yaw = 0.0f;
float camera_pitch = 0.0f;
float camera_radius = 12.0f;
const float MOUSE_SENSITIVITY = 0.25f;

/*
 * Interface state variables
 */
bool mode_wireframe = false;
bool mode_perspective = true;
bool left_mouse_held = false;
bool right_mouse_held = false;


static void ShowUsage(std::string name) {
	std::cerr << "Usage: " << name << " OBJECT_FILE, TEXTURE_FILE"
			<< std::endl;
}

#ifndef RUN_TESTS
/* Main Program */
int main(int argc, char* argv[]) {

	if (argc != 3) {
		ShowUsage(argv[0]);
		exit(EXIT_FAILURE);
	}

	subject_mesh_filename = argv[1];
	subject_texture_filename = argv[2];

	InitializeWindow(argc, argv);
	InitializeApp(argc, argv);
	glutMainLoop();
	exit(EXIT_SUCCESS);
}
#endif

// Implements the InitializeApp function
void InitializeApp(int argc, char* argv[]) {

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Display GPU OpenGL version
	fprintf(stdout, "INFO: OpenGL Version: %s\n", glGetString(GL_VERSION));

	// Load shaders
	subject_shader.LoadShadersFiles(kSubjectVertexShaderSrc, kSubjectFragmentShaderSrc);
	lamp_shader.LoadShadersFiles(kLightVertexShaderSrc, kLightFragmentShaderSrc);

	// Load mesh objects
	if(!subject_mesh.LoadOBJ(subject_mesh_filename) ) {
		std::cout << "Failed to load subject Object file!" << std::endl;
		exit(EXIT_FAILURE);
	}

	if(!key_light_mesh.LoadOBJ(kLampMeshFilename) ) {
		std::cout << "Failed to load lamp Object file!" << std::endl;
		exit(EXIT_FAILURE);
	}
	if(!fill_light_mesh.LoadOBJ(kLampMeshFilename) ) {
		std::cout << "Failed to load lamp Object file!" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Init buffers
	subject_mesh.InitBuffers();
	key_light_mesh.InitBuffers();
	fill_light_mesh.InitBuffers();

	// Load Textures
	subject_texture.LoadTexture(subject_texture_filename.c_str());

	glViewport(0, 0, window_width, window_height);
	glEnable(GL_DEPTH_TEST); // Enable z-depth
	glClearColor(default_bg_r, default_bg_g, default_bg_b, default_bg_a);
}

void InitializeWindow(int argc, char* argv[]) {
	// Initializes FreeGLUT
	glutInit(&argc, argv);

	// Show version info
	fprintf(stdout,"INFO: GLUT_VERSION=%d\n", glutGet(GLUT_VERSION));

	// Memory buffer setup for display
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(window_width, window_height);
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
 * GLUT idle callback function to tell freeGlut to redraw asap for performance
 */
void IdleFunction(void) {
	glutPostRedisplay();
}

/*
 * GLUT timer callback function used to update the application window title
 * every quarter second to display the FPS.
 */
void TimerFunction(int update_value) {
	// Don't update on the first run (since nothing's been drawn yet)
	if (0 != update_value) {
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
void HandleWindowResize(int w, int h) {
	window_width = w;
	window_height = h;
	glViewport(0, 0, window_width, window_height);
}

/*
 * Render Graphics Implementation
 */
void RenderGraphics(void) {
	// track the frame count
	++frame_count;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clears the screen

	glm::mat4 model, view, projection;

	// Transform the camera
	orbit_camera.SetLookAt(subject_position);
	orbit_camera.Rotate(camera_yaw, camera_pitch);
	orbit_camera.SetRadius(camera_radius);
	view = orbit_camera.GetViewMatrix();

	// Create perspective or orthographic projection
	if(mode_perspective) {
		projection = glm::perspective(glm::radians(45.0f), (GLfloat)window_width / (GLfloat)window_height, 0.1f, 100.0f);
	} else {
		projection = glm::ortho(-5.0f,5.0f,-5.0f,5.0f,0.1f,100.0f);
	}

	subject_shader.Use();

	subject_shader.SetUniform("view", view);
	subject_shader.SetUniform("projection", projection);
	subject_shader.SetUniform("viewPosition", orbit_camera.GetPosition());

	subject_shader.SetUniform("lights[0].ambient", glm::vec3(0.1f, 0.1f, 0.1f));
	subject_shader.SetUniform("lights[0].diffuse", key_light_color);
	subject_shader.SetUniform("lights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));// white
	subject_shader.SetUniform("lights[0].position", key_light_position);
	subject_shader.SetUniform("lights[0].intensity", key_light_intensity);

	subject_shader.SetUniform("lights[1].ambient", glm::vec3(0.1f, 0.1f, 0.1f));
	subject_shader.SetUniform("lights[1].diffuse", fill_light_color);
	subject_shader.SetUniform("lights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));// white
	subject_shader.SetUniform("lights[1].position", fill_light_position);
	subject_shader.SetUniform("lights[1].intensity", fill_light_intensity);


	// Transform the subject into the world
	model = glm::translate(model, subject_position);
	model = glm::scale(model, subject_scale);

	subject_shader.SetUniform("model", model);

	subject_shader.SetUniform("material.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	subject_shader.SetUniformSampler("material.diffuseMap", 0);
	subject_shader.SetUniform("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
	subject_shader.SetUniform("material.shininess", 64.0f);

	subject_texture.Bind(0);
	subject_mesh.Draw();
	subject_texture.Unbind(0);


	// Render the key light
	model = glm::translate(glm::mat4(), key_light_position);
	model = glm::scale(model, key_light_scale);
	lamp_shader.Use();
	lamp_shader.SetUniform("lampColor", key_light_color);
	lamp_shader.SetUniform("model", model);
	lamp_shader.SetUniform("view", view);
	lamp_shader.SetUniform("projection", projection);
	key_light_mesh.Draw();


	// Render the fill light
	model = glm::translate(glm::mat4(), fill_light_position);
	model = glm::scale(model, fill_light_scale);
	lamp_shader.Use();
	lamp_shader.SetUniform("lampColor", fill_light_color);
	lamp_shader.SetUniform("model", model);
//	lampShader.SetUniform("view", view);
//	lampShader.SetUniform("projection", projection);
	fill_light_mesh.Draw();

	glutSwapBuffers(); // Flip the front and back buffers (equivalent to GL flush)
}

/*
 * Handle keyboard events
 */
void HandleKeyboard(unsigned char key, GLint x, GLint y) {
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
void HandleMouseClick(int button, int state, int x, int y) {
	if((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN)){
		left_mouse_held = true;
	}

	if((button == GLUT_LEFT_BUTTON) && (state == GLUT_UP)){
		left_mouse_held = false;
	}

	if((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN)){
		right_mouse_held = true;
	}

	if((button == GLUT_RIGHT_BUTTON) && (state == GLUT_UP)){
		right_mouse_held = false;
	}
}

/*
 * Handle MousePressedMove events
 */
void HandleMousePressedMove(int x, int y) {
	static glm::vec2 last_mouse_pos = glm::vec2(0, 0);

	// Update angles based on Left Mouse Button input to orbit around the chair
	if (left_mouse_held) {
		// each pixel represents a quarter of a degree rotation (this is our mouse sensitivity)
		camera_yaw -= ((float)x - last_mouse_pos.x) * MOUSE_SENSITIVITY;
		camera_pitch += ((float)y - last_mouse_pos.y) * MOUSE_SENSITIVITY;
	}

	// Change orbit camera radius with the Right Mouse Button
	if (right_mouse_held) {
		float dx = 0.01f * ((float)x - last_mouse_pos.x);
		float dy = 0.01f * ((float)y - last_mouse_pos.y);
		camera_radius += dx - dy;
	}

	last_mouse_pos.x = (float)x;
	last_mouse_pos.y = (float)y;
}

