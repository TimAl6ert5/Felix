/*
 *    Due Date: 2020-12-13
 *      Author: Tim Alberts (timothy.alberts@snhu.edu)
 *       Class: CS-499-X2150 Computer Science Capstone 20EW2
 *  Instructor: Joseph Conlan
 *  	Module: 7-1
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

#include "scene.h"

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

Scene scene;

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

const float MOUSE_SENSITIVITY = 0.25f;

bool mode_wireframe = false;
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

	scene.SetSubjectFiles(argv[1], argv[2]);

	InitializeWindow(argc, argv);
	InitializeApp(argc, argv);
	glutMainLoop();
	exit(EXIT_SUCCESS);
}
#endif

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

// Implements the InitializeApp function
void InitializeApp(int argc, char* argv[]) {

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		std::cout << "Failed to initialize GLEW" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Display GPU OpenGL version
	fprintf(stdout, "INFO: OpenGL Version: %s\n", glGetString(GL_VERSION));

	scene.LoadShaders();

	// Load mesh objects
	if(!scene.LoadSubjectMesh() ) {
		std::cout << "Failed to load subject Object file!" << std::endl;
		exit(EXIT_FAILURE);
	}

	if(!scene.LoadKeyLightMesh() ) {
		std::cout << "Failed to load lamp Object file!" << std::endl;
		exit(EXIT_FAILURE);
	}

	if(!scene.LoadFillLightMesh() ) {
		std::cout << "Failed to load lamp Object file!" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Init buffers
	scene.InitMeshBuffers();

	// Load Textures
	scene.LoadSubjectTexture();

	glViewport(0, 0, window_width, window_height);
	glEnable(GL_DEPTH_TEST); // Enable z-depth
	glClearColor(default_bg_r, default_bg_g, default_bg_b, default_bg_a);
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

	scene.RenderScene(window_width, window_height);

	glutSwapBuffers(); // Flip the front and back buffers (equivalent to GL flush)
}

/*
 * Handle keyboard events
 */
void HandleKeyboard(unsigned char key, GLint x, GLint y) {
	switch(key) {
	case 'W':
	case 'w':
		mode_wireframe = !mode_wireframe;
		if (mode_wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;

	case 'O':
	case 'o':
		scene.SetOrthographic();
		break;

	case 'P':
	case 'p':
		scene.SetPerspective();
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
		scene.SetCameraYaw(((float)x - last_mouse_pos.x) * MOUSE_SENSITIVITY);
		scene.SetCameraPitch(((float)y - last_mouse_pos.y) * MOUSE_SENSITIVITY);
	}

	// Change orbit camera radius with the Right Mouse Button
	if (right_mouse_held) {
		float dx = 0.01f * ((float)x - last_mouse_pos.x);
		float dy = 0.01f * ((float)y - last_mouse_pos.y);
		scene.SetCameraRadius(dx - dy);
	}

	last_mouse_pos.x = (float)x;
	last_mouse_pos.y = (float)y;
}
