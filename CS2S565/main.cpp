#include <FreeImage\FreeImagePlus.h>
#include <wincodec.h>
#include <glew\glew.h>
#include <freeglut\freeglut.h>
#include <iostream>

#include "texture_loader.h"
#include "shader_setup.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stack>

using namespace std;

// Main function prototypes
void init(int argc, char* argv[]);
void display(void);

//Background function prototype
void drawBackGround(void);

// Mouse input (rotation) example
void mouseButtonDown(int button_id, int state, int x, int y);
void mouseMove(int x, int y);
void keyDown(unsigned char key, int x, int y);

////////////////////////////////////////
// Globals
////////////////////////////////////////
float snowmanX = 0.0f;
float snowmanY = 0.0f;
float snowmanOrientation = 0.0f;
//
// Demo model
//
float theta0 = 0.0f;
float theta1 = 0.1f;
float theta2 = 0.0f;
float theta1b = glm::radians(45.0f);

//TEXTURES
GLuint backGroundTexture1;
GLuint backGroundTexture2;


// Variables needed to track where the mouse pointer is so we can determine which direction it's moving in
int mouse_x, mouse_y;
bool mDown = false;
glm::mat4 model_view= glm::mat4(1);


// Shader program object
GLuint myShaderProgram;
GLuint locT; // Location of "T" uniform variable

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) 
{
	init(argc, argv);

	glutMainLoop();

	return 0;
}


void init(int argc, char* argv[]) 
{
	// 1. Initialise FreeGLUT
	glutInit(&argc, argv);

	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

	glutInitWindowSize(1500, 1000);
	glutInitWindowPosition(500, 150);
	glutCreateWindow("30077548 Graphics Assignment 1");

	// Display callback
	glutDisplayFunc(display);

	// Register keyboard and mouse callback functions
	glutKeyboardFunc(keyDown);
	glutMouseFunc(mouseButtonDown);
	glutMotionFunc(mouseMove);

	// 2. Initialise GLEW library
	GLenum err = glewInit();

	// Ensure the GLEW library was initialised successfully before proceeding
	if (err == GLEW_OK) {

		cout << "GLEW initialised okay\n";
	}
	else {

		cout << "GLEW could not be initialised!\n";
		throw;
	}

	// Example query OpenGL state (get version number)
	int majorVersion, minorVersion;

	glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

	cout << "OpenGL version " << majorVersion << "." << minorVersion << "\n\n";

	// Report maximum number of vertex attributes
	GLint numAttributeSlots;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &numAttributeSlots);
	cout << "GL_MAX_VERTEX_ATTRIBS = " << numAttributeSlots << endl;

	// 3. Initialise OpenGL settings and objects we'll use in our scene
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Shader setup - more on this next week!!!
	myShaderProgram = setupShaders(string("Shaders\\basic_vertex_shader.txt"), string("Shaders\\basic_fragment_shader.txt"));
	locT = glGetUniformLocation(myShaderProgram, "T");

	// Setup objects using Vertex Buffer Objects (VBOs)
	
	//Texture loading
	backGroundTexture1 =
		fiLoadTexture("..\\..\\Common\\Resources\\Textures\\background1.png");
	backGroundTexture2 = 
		fiLoadTexture("..\\..\\Common\\Resources\\Textures\\background2.jpg");
}



void display(void) 
{
	//clear the buffers to the following preset values
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//draw scene background
	drawBackGround();

	//call our function to render our shape hierarchy

	//instructs the rendering that you are done with the current frame and buffers should be swapped to work on the next one.
	glutSwapBuffers();
}

////////////////////////////////////////////////////////////////////////////////
// SCENE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////


void drawBackGround(void)
{

}


////////////////////////////////////////////////////////////////////////////////
// EVENT HANDLING FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

#pragma region Event handling functions

void mouseButtonDown(int button_id, int state, int x, int y) 
{
	if (button_id == GLUT_LEFT_BUTTON) 
	{
		if (state == GLUT_DOWN) 
		{
			mouse_x = x;
			mouse_y = y;

			mDown = true;
		}
		else if (state == GLUT_UP) 
		{
			mDown = false;
		}
	}
}

void mouseMove(int x, int y) 
{
	if (mDown) 
	{
		int dx = x - mouse_x;
		int dy = y - mouse_y;

		// Ctrl click to rotate, click on its own to move...
		if (glutGetModifiers() == GLUT_ACTIVE_CTRL) 
		{
			theta1 += float(dy) * 0.01;
			theta1b += float(dy) * 0.01;
			theta2 += float(dy) * 0.01;

			snowmanOrientation += float(dy);
		}
		else 
		{
			snowmanX += float(dx) * 0.0025f;
			snowmanY -= float(dy) * 0.0025f;
		}

		mouse_x = x;
		mouse_y = y;

		glutPostRedisplay();
	}
}


void keyDown(unsigned char key, int x, int y) 
{
	if (key == 'r') 
	{
		snowmanX = 0.0f;
		snowmanY = 0.0f;
		snowmanOrientation = 0.0f;

		glutPostRedisplay();
	}
}

