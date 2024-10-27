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

using namespace std;

//Vertex Shader Source code
const char* vertexShaderSource = 
R"(
	#version 330 core

	layout (location = 0) in vec2 aPos;
	layout (location = 1) in vec2 textureCoords;

	out Vec2 TexCoords;

	void main()
	{
		gl_Position = vec4(aPos.x, aPos.y, 1.0f);
		TexCoords = textureCoords;
	}
)";


//Fragment Shader source code
const char* fragmentShaderSource = 
R"(
	#version 330 core

	in Vec2 TexCoords;
	layout (location = 0) out vec4 FragColor;

	uniform sampler2D sunTexture;

	void main()
	{
	   FragColor = texture(sunTexture, TexCoords);
	}
)";

// Main function prototypes
void init(int argc, char* argv[]);
void display(void);

//Background function prototype
void drawBackGround(void);
//Cloud function prototype
void drawOneCloud(GLuint cloudTexture, float x1, float x2, float y1, float y2);
void drawClouds(void);
//Functions for the sun
void setUpSunVAOandVBO(void);
void drawSunVAOandVBO(void);
//Functions for the floor sprite
void setUpFloorVAOandVBO(void);
void drawFloorVAOandVBO(void);

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
GLuint sunVAO, sunVBO, sunTexVBO, sunTexture;

GLfloat sunVertices[] =
{
	-0.95f, 0.5f,
	-0.95f, 0.9f,
	-0.65f, 0.5f,
	-0.65f, 0.9f

};

GLfloat sunTextureCoords[] =
{
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 0.0f
};

////////////////////////////////////////
//Floor vairables
////////////////////////////////////////
GLuint floorVAO, floorVBO;

GLfloat floorVertices[] =
{
	-1.0f, -0.7f,//Top left
	-1.0f, -1.0f, //Bottom left
	1.0f, -1.0f, // Bottom right
	1.0f, -0.7f //Top right
};

///////////////////////////////
// Variables needed to track where the mouse pointer is so we can determine which direction it's moving in
int mouse_x, mouse_y;
bool mDown = false;
glm::mat4 model_view= glm::mat4(1);

//////////////////////////
// Shader program object
GLuint vertexShader;
GLuint fragmentShader;
GLuint myShaderProgram;
GLuint locT; // Location of "T" uniform variable

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) 
{
	init(argc, argv);

	glutMainLoop();

	//To keep things clean delete the objects and programs
	glDeleteVertexArrays(1, &floorVAO);
	glDeleteBuffers(1, &floorVBO);
	glDeleteProgram(myShaderProgram);

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

	//Set up vertex shader
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	//set up fragment shader
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);

	//Set up myShaderProgram
	myShaderProgram = glCreateProgram();
	glAttachShader(myShaderProgram, vertexShader);
	glAttachShader(myShaderProgram, fragmentShader);
	glLinkProgram(myShaderProgram);

	//Delete the shaders after it has all been set up as they are now in the program
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//Setup objects using Vertex Buffer Objects (VBOs)
	setUpSunVAOandVBO();
	setUpFloorVAOandVBO();

	// 3. Initialise OpenGL settings and objects we'll use in our scene
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	// Shader setup - more on this next week!!!
	//myShaderProgram = setupShaders(string("Shaders\\basic_vertex_shader.txt"), string("Shaders\\basic_fragment_shader.txt"));
	//locT = glGetUniformLocation(myShaderProgram, "T");

	//Texture loading
	//background
	backGroundTextures.push_back(backGroundTexture1 =
		fiLoadTexture("..\\..\\Common\\Resources\\Textures\\background1.png"));

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

	sunTexture = 
		wicLoadTexture(L"..\\..\\Common\\Resources\\Textures\\Sun.png");

}



void display(void) 
{
	//clear the buffers to the following preset values
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//draw scene background
	drawBackGround();
	drawClouds();
	drawSunVAOandVBO();
	drawFloorVAOandVBO();

	//call our function to render our shape hierarchy

	//instructs the rendering that you are done with the current frame and buffers should be swapped to work on the next one.
	glutSwapBuffers();
}

////////////////////////////////////////////////////////////////////////////////
// SCENE FUNCTIONS
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

// New function to draw the Quad with the cloud texture
void drawClouds() 
{
	for (int i = 0; i < Clouds.size(); i++)
	{
		drawOneCloud(Clouds[i].texture, Clouds[i].x1, Clouds[i].x2, Clouds[i].y1, Clouds[i].y2);
	}
}

void setUpSunVAOandVBO(void)
{
	// Generate and bind the VAO
	glGenVertexArrays(1, &sunVAO);
	glBindVertexArray(sunVAO);

	// Generate VBO for positions and bind data
	glGenBuffers(1, &sunVBO);
	glBindBuffer(GL_ARRAY_BUFFER, sunVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sunVertices), sunVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	// Generate VBO for texture coordinates and bind data
	glGenBuffers(1, &sunTexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, sunTexVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sunTextureCoords), sunTextureCoords, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

	// Unbind to avoid unintended modifications
	glBindVertexArray(0);
}

void drawSunVAOandVBO(void)
{
	glUseProgram(myShaderProgram);

	// Set texture uniform
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sunTexture);

	// Bind VAO and draw the quad
	glBindVertexArray(sunVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// Unbind VAO and texture after drawing
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
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
		/*
			After GL_ you choose between:
				
				STREAM, STATIC and DYNAMIC
					Steam means vertices will be modded once and used a few times
					Static means vertices will be modded once and used many many times
					Dynamic means vertices will be modded multiple times and used many many times

			Then after that is decided and you have GL_STATIC_ you choose between:
				DRAW, READ and COPY
					Draw means the vertices will be modified and used to draw an image on the screen
					Video didnt really explain the other two though
		*/
	);		

	//Pass the index of the attribute we want to use
	glVertexAttribPointer(
		0,						//Position of the vertex
		2,						//How many values we have per vertex
		GL_FLOAT,				//Tell what data types we have
		GL_FALSE,				//Only matter if we have the values as integers
		2 * sizeof(float),		//Stride of our vertices, the amount of data between each vertex
		(void*)0				//Offset, pointer to where our vertices begin in the array
	);
	//Now the vertex attribute has been configured we need to enable it
	glEnableVertexAttribArray(0);

	//Bind both the VBA and the VBO by binding them both to 0
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void drawFloorVAOandVBO(void)
{
	//glUseProgram(myShaderProgram);

	glBindVertexArray(floorVAO);
	glDrawArrays(GL_QUADS, 0, 4);
		
	glBindVertexArray(0);
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

