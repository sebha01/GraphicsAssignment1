#include <FreeImage\FreeImagePlus.h>
#include <wincodec.h>
#include <glew\glew.h>
#include <freeglut\freeglut.h>
#include <iostream>
#include <vector>

#include "texture_loader.h"
#include "shader_setup.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stack>
#include "Character.h"

using namespace std;



/////////////////////////////////////
// Main function prototypes
////////////////////////////////////
void init(int argc, char* argv[]);
void display(void);
//Background function prototype
void drawBackGround(void);
//Cloud function prototype
void drawOneCloud(GLuint cloudTexture, float x1, float x2, float y1, float y2);
void drawClouds(void);
//Functions for the sun
void drawSun(void);
void updateSunPosition(int value);
//Functions for the floor sprite
void setUpFloorVAOandVBO(void);
void drawFloorVAOandVBO(void);
//Collectable item
void setUpCollectable(void);
void drawCollectable(void);
void drawHierarchy(glm::mat4x4& R);
void drawTexturedQuad(void);
// Mouse input (rotation) example
void mouseButtonDown(int button_id, int state, int x, int y);
void mouseMove(int x, int y);
void keyDown(unsigned char key, int x, int y);



////////////////////////////////////////
// Globals
////////////////////////////////////////
Character* myCharacter = nullptr;
float characterX = 0.0f;
float characterY = -0.6f;
float characterOrientation = 0.0f;
//
// Demo model
//
float theta0 = 0.0f;
float theta1 = 0.1f;
float theta2 = 0.0f;
float theta1b = glm::radians(45.0f);

const float quadLength = 0.4f;

GLuint rustTexture;
// Matrix stack
stack<glm::mat4x4> matrixStack;
// Variable we'll use to animate (rotate) our star object
float theta = 0.0f;
///////////////////////////////
// Variables needed to track where the mouse pointer is so we can determine which direction it's moving in
int mouse_x, mouse_y;
bool mDown = false;
glm::mat4 model_view = glm::mat4(1);
//////////////////////////
// Shader program object
GLuint myShaderProgram;
GLuint locT; // Location of "T" uniform variable
GLuint locIT;
////////////////////////
//background variables
////////////////////////
vector<GLuint> backGroundTextures;
GLuint backGroundTexture1, backGroundTexture2, backGroundTexture3;
/////////////////
//Cloud variables
//////////////////
struct Cloud 
{
	GLuint texture;
	float x1, x2, y1, y2;
};
vector<Cloud> Clouds;
/////////////////////////
//Sun variables 
///////////////////////
GLuint sunTexture;
float sunY = 1.0f; 
float sunSpeed = 0.001f; 
bool movingUp = true; 
////////////////////////////////////////
//Floor vairables
////////////////////////////////////////
GLuint floorVAO, floorVBO ,floorTexture;
GLfloat floorVertices[] =
{
	//Vertices			//Texture coords
	-1.0f, -0.7f, 0.0f,		0.0f, 0.0f,//Top left 
	-1.0f, -1.0f, 0.0f,		0.0f, 1.0f,//Bottom left
	1.0f, -1.0f, 0.0f,		1.0f, 1.0f,// Bottom right
	1.0f, -0.7f, 0.0f,		1.0f, 0.0f //Top right
};
/////////////////////////////////////////
// Collectable variables
/////////////////////////////////////////
GLfloat collectableVertices[] =
{
	0.09f, 0.1f * float(sqrt(3)) / 3, 0.0f, 	
	-0.09f, 0.1f * float(sqrt(3)) / 3, 0.0f,	
	0.0f, -0.1f * float(sqrt(3)) * 2 / 3, 0.0f,	
	0.09f / 2, -0.1f * float(sqrt(3)) / 6, 0.0f,	
	-0.09f / 2, -0.1f * float(sqrt(3)) / 6, 0.0f,	
	0.0f, 0.1f * float(sqrt(3)) / 3, 0.0f	
};
GLuint collectableIndices[] =
{
	0, 3, 5,
	3, 2, 4,
	5, 4, 1
};
GLfloat collectableColours[] =
{
	1.0f, 1.0f, 1.0f,
	0.5f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.2f, 0.02f, 0.8f,
};
GLuint collectableVAO, collectableVBO, collectableEBO, collectableColoursVBO;



//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) 
{
	init(argc, argv);
	glutTimerFunc(0, updateSunPosition, 0);
	glutMainLoop();

	return 0;
}


void init(int argc, char* argv[])
{
	// 1. Initialise FreeGLUT
	glutInit(&argc, argv);
	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

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

	//Setup objects using Vertex Buffer Objects (VBOs)
	setUpFloorVAOandVBO();
	setUpCollectable();

	// 3. Initialise OpenGL settings and objects we'll use in our scene
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

	// Shader setup - more on this next week!!!
	myShaderProgram = setupShaders(string("Shaders\\basic_vertex_shader.txt"), string("Shaders\\basic_fragment_shader.txt"));
	locT = glGetUniformLocation(myShaderProgram, "T");
	locIT = glGetUniformLocation(myShaderProgram, "isTexture");

	//Texture loading
	//background
	backGroundTextures.push_back(backGroundTexture1 =
		wicLoadTexture(L"..\\..\\Common\\Resources\\Textures\\background1.png"));

	backGroundTextures.push_back(backGroundTexture2 =
		wicLoadTexture(L"..\\..\\Common\\Resources\\Textures\\background2.png"));

	backGroundTextures.push_back(backGroundTexture3 =
		wicLoadTexture(L"..\\..\\Common\\Resources\\Textures\\background3.png"));

	//clouds
	Clouds.push_back
	({
		wicLoadTexture(L"..\\..\\Common\\Resources\\Textures\\cloud8.png"),
		0.0f, 0.3f, 0.4f, 0.6f
	});
	Clouds.push_back
	({
		wicLoadTexture(L"..\\..\\Common\\Resources\\Textures\\cloud2.png"),
		0.5f, 0.9f, 0.7f, 0.9f
	});
	Clouds.push_back
	({
		wicLoadTexture(L"..\\..\\Common\\Resources\\Textures\\cloud7.png"),
		-0.4f, -0.8f, 0.3f, 0.5f
	});
	Clouds.push_back
	({
		wicLoadTexture(L"..\\..\\Common\\Resources\\Textures\\cloud4.png"),
		-0.1f, 0.4f, 0.7f, 1.0f
	});
	Clouds.push_back
	({
		wicLoadTexture(L"..\\..\\Common\\Resources\\Textures\\cloud5.png"),
		-0.1f, -0.4f, 0.5f, 0.7f
	});
	Clouds.push_back
	({
		wicLoadTexture(L"..\\..\\Common\\Resources\\Textures\\cloud6.png"),
		0.6f, 0.9f, 0.3f, 0.5f
	});
	
	floorTexture =
		wicLoadTexture(L"..\\..\\Common\\Resources\\Textures\\Floor.png");


	sunTexture = 
		wicLoadTexture(L"..\\..\\Common\\Resources\\Textures\\Sun.png");

	// Set Projection Matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(-1.0f, 1.0f, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glm::vec3 origin(0, 0, -0.5);
	glm::vec3 target(0, 0, 0);
	glm::vec3 up(0, 1, 0);
	model_view = glm::lookAt(origin, target, up);


	// Create new Snowman instance
	myCharacter = new Character();
}

void display(void) 
{
	//clear the buffers to the following preset values
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

	glUseProgram(0);

	//draw scene background
	drawBackGround();
	drawClouds();
	drawSun();

	glUseProgram(myShaderProgram);
	glm::mat4 T = glm::translate(glm::mat4(1), glm::vec3(0.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(locT, 1, GL_FALSE, (GLfloat*)&T);
	
	drawFloorVAOandVBO();
	drawCollectable();

	//call our function to render our shape

	/*glm::mat4 R = glm::rotate(glm::mat4(1), theta, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4  T = glm::translate(glm::mat4(1), glm::vec3(-0.8f, -0.25f, 0.0f));
	R = T * R;
	glLoadMatrixf(glm::value_ptr(R));*/

	//glUseProgram(0);
	//drawHierarchy(R);
	//T = glm::translate(glm::mat4(1), glm::vec3(0.3f, 0.0f, 0.0f));
	//for (int i = 0; i < 5; i++)
	//{
		//R = T * R;
		//drawHierarchy(R);
	//}

	myCharacter->renderCharacter(characterX, characterY, 0.5f, characterOrientation);

	//call our function to render our shape hierarchy

	//instructs the rendering that you are done with the current frame and buffers should be swapped to work on the next one.
	glutSwapBuffers();
}

////////////////////////////////////////////////////////////////////////////////
// BACKGROUND SCENE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////
void drawBackGround(void)
{
	// Enable blending for transparency
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (int i = 0; i < backGroundTextures.size(); i++)
	{
		glBindTexture(GL_TEXTURE_2D, backGroundTextures[i]);
		glEnable(GL_TEXTURE_2D);

		glBegin(GL_TRIANGLE_STRIP);

		glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, 1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, 1.0f);

		glEnd();
		glDisable(GL_TEXTURE_2D);
	}

	glDisable(GL_BLEND);
}

void drawOneCloud(GLuint cloudTexture, float x1, float x2, float y1, float y2)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D, cloudTexture);
	glEnable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(x1, y1);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(x2, y1);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(x2, y2);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(x1, y2);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void drawClouds() 
{
	for (int i = 0; i < Clouds.size(); i++)
	{
		drawOneCloud(Clouds[i].texture, Clouds[i].x1, Clouds[i].x2, Clouds[i].y1, Clouds[i].y2);
	}
}

void drawSun(void)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, sunTexture);

	glBegin(GL_TRIANGLE_STRIP);

	glTexCoord2f(0.0f, 1.0f); glVertex2f(-0.95f, sunY - 0.4f);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.95f, sunY);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(-0.65f, sunY - 0.4f);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(-0.65f, sunY);

	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void updateSunPosition(int value)
{
	if (movingUp) 
	{
		sunY += sunSpeed; 
		if (sunY >= 1.0f)
		{
			movingUp = false;
		}
	}
	else 
	{
		sunY -= sunSpeed; 
		if (sunY <= 0.8f)
		{
			movingUp = true;
		} 
	}

	glutPostRedisplay();
	glutTimerFunc(16, updateSunPosition, 0);
}

///////////////////////////////////////////////////////////////////////////////////
// FLOOR FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////
void setUpFloorVAOandVBO(void)
{
	//Generate the VAO
	glGenVertexArrays(1, &floorVAO);
	//Generate the VBO
	glGenBuffers(1, &floorVBO);

	//Bind the vertex array
	glBindVertexArray(floorVAO);
	//Bind the buffer object for the VBO  
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	//Store vertices in VBO      
	glBufferData(
		GL_ARRAY_BUFFER,	//Specify type of buffer
		sizeof(floorVertices),		//Total size of data
		floorVertices,				//Give the actual vertices
		GL_STATIC_DRAW				//Specify the use of the data
	);		

	//Pass the index of the attribute we want to use
	glVertexAttribPointer(
		0,						//Position of the vertex
		3,						//How many values we have per vertex
		GL_FLOAT,				//Tell what data types we have
		GL_FALSE,				//Only matter if we have the values as integers
		5 * sizeof(float),		//Stride of our vertices, the amount of data between each vertex
		(void*)0				//Offset, pointer to where our vertices begin in the array
	);

	// Texture coordinates attribute (assuming it is the next attribute)
	glVertexAttribPointer(
		2,                        // Index for texture coordinates
		2,                        // 2 values (s, t)
		GL_FLOAT,                 // Tell what data types we have
		GL_FALSE,                 // Not normalized
		5 * sizeof(float),        // Updated stride: 5
		(void*)(3 * sizeof(float)) // Offset to the texture coords
	);

	//Now the vertex attribute has been configured we need to enable it
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(2); // Enable texture coordinate

	//Bind both the VBA and the VBO by binding them both to 0
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void drawFloorVAOandVBO(void)
{
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, floorTexture);
	glUniform1i(locIT, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   // Wrap horizontally
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);   // Wrap vertically
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Minification filter

	glBindVertexArray(floorVAO);
	glDrawArrays(GL_QUADS, 0, 4);
		
	glBindVertexArray(0);
	glUniform1i(locIT, 0);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

////////////////////////////////////////////////////////////////////////////////////
// COLLECTABLE FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////
void setUpCollectable(void)
{
	glGenVertexArrays(1, &collectableVAO);
	glGenBuffers(1, &collectableVBO);
	glGenBuffers(1, &collectableEBO);
	glGenBuffers(1, &collectableColoursVBO);

	glBindVertexArray(collectableVAO);

	glBindBuffer(GL_ARRAY_BUFFER, collectableVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(collectableVertices), collectableVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, collectableEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(collectableIndices), collectableIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Color VBO
	glBindBuffer(GL_ARRAY_BUFFER, collectableColoursVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(collectableColours), collectableColours, GL_STATIC_DRAW);

	// Set color attribute (location = 1 in the shader)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void drawCollectable(void)
{
	glBindVertexArray(collectableVAO);
	glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

// Draw example hierarchical object - pass in the current transform matrix so we can 'append' new transforms to this.
void drawHierarchy(glm::mat4x4& R) 
{
	matrixStack.push(R);

	// Draw base base of arm


	R = R * glm::rotate(glm::mat4(1.0f), theta0, glm::vec3(0.0f, 0.0f, 1.0f));
	glLoadMatrixf((GLfloat*)&R);

	drawTexturedQuad();


	matrixStack.push(R);

	// Draw first segment


	R = R * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, quadLength, 0.0f)) * glm::rotate(glm::mat4(1.0f), theta1, glm::vec3(0.0f, 0.0f, 1.0f));


	glLoadMatrixf((GLfloat*)&R);

	drawTexturedQuad();


	matrixStack.push(R);

	// Draw first branch

	R = R * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, quadLength, 0.0f)) * glm::rotate(glm::mat4(1.0f), theta2, glm::vec3(0.0f, 0.0f, 1.0f));

	glLoadMatrixf((GLfloat*)&R);

	drawTexturedQuad();

	R = matrixStack.top();
	matrixStack.pop();


	matrixStack.push(R);

	// Draw second branch


	R = R * glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, quadLength, 0.0f)) * glm::rotate(glm::mat4(1.0f), theta1b, glm::vec3(0.0f, 0.0f, 1.0f));
	glLoadMatrixf((GLfloat*)&R);

	drawTexturedQuad();

	R = matrixStack.top();
	matrixStack.pop();


	R = matrixStack.top();
	matrixStack.pop();


	R = matrixStack.top();
	matrixStack.pop();
}

void drawTexturedQuad(void) 
{
	//glBindTexture(GL_TEXTURE_2D, myTexture);
	glEnable(GL_TEXTURE_2D);
	//glBegin(GL_TRIANGLE_STRIP);
	//glTexCoord2f(0.0f, 1.0f);
	//glVertex2f(-0.4f, -0.7f);
	//
	//glTexCoord2f(0.0f, 0.0f);
	//glVertex2f(-0.4f, 0.7f);

	//glTexCoord2f(1.0f, 1.0f);
	//glVertex2f(0.4f, -0.7f);

	//glTexCoord2f(1.0f, 0.0f);
	//glVertex2f(0.4f, 0.7f);
	//glEnd();

	glBindTexture(GL_TEXTURE_2D, rustTexture);

	glBegin(GL_TRIANGLE_STRIP);

	glColor3ub(255, 255, 255);

	glTexCoord2f(0.4f, 1.0f);
	glVertex2f(-0.05f, 0.0f);

	glTexCoord2f(0.4f, 0.0f);
	glVertex2f(-0.05f, quadLength);

	glTexCoord2f(0.6f, 1.0f);
	glVertex2f(0.05f, 0.0f);

	glTexCoord2f(0.6f, 0.0f);
	glVertex2f(0.05f, quadLength);

	glEnd();
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
	////Do not want a mouse move
	//if (mDown) 
	//{
	//	int dx = x - mouse_x;
	//	int dy = y - mouse_y;

	//	// Ctrl click to rotate, click on its own to move...
	//	if (glutGetModifiers() == GLUT_ACTIVE_CTRL) 
	//	{
	//		theta1 += float(dy) * 0.01;
	//		theta1b += float(dy) * 0.01;
	//		theta2 += float(dy) * 0.01;

	//		characterOrientation += float(dy);
	//	}
	//	else 
	//	{
	//		characterX += float(dx) * 0.0025f;
	//		characterY -= float(dy) * 0.0025f;
	//	}

	//	mouse_x = x;
	//	mouse_y = y;

	//	glutPostRedisplay();
	//}
}


void keyDown(unsigned char key, int x, int y) 
{
	if (key == 'a') 
	{
		characterX -= 0.0001f;
		characterY = 0.0f;
		characterOrientation = 0.0f;

		glutPostRedisplay();
	}
	else if (key == 'd') 
	{
		characterX += 0.0001f;
		characterY = 0.0f;
		characterOrientation = 0.0f;

		glutPostRedisplay();
	}
	else if (key == 'r')
	{
		characterX = 0.0f;
		characterY = 0.0f;
		characterOrientation = 0.0f;

		glutPostRedisplay();
	}
	else if (key == ' ')
	{
		characterX = 0.0f;
		//Add code for jumping in
		characterY = 0.0f;
		characterOrientation = 0.0f;

		glutPostRedisplay();
	}
}

