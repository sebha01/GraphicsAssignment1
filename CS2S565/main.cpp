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


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
	WELCOME TO MY GRAPHICS ASSIGNMENT ONE PROJECT SOLUTION, ALL ASSET AND OUTSIDE HELP HAVE BEEN REFERENCED THROUGH THE CODE, I ALSO HAVE MADE A 
	GITHUB REPOSITORY FOR THIS ASSIGNMENT WHICH HAS AN ISSUE REGARDING ASSET LINKS AND LICENSING IN BETTER DETAIL

	HERE IS THE LINK TO THIS BELOW:

	https://github.com/sebha01/GraphicsAssignment1.git
*/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
//Functions for the floor and platforms
void setUpFloorVAOandVBO(void);
void drawFloorVAOandVBO(void);
void setUpPlatformVAOandVBO(void);
void drawPlatformsVAOandVBO(void);
//Collectable item
void setUpCollectable(void);
void drawCollectable(void);
// Mouse input (rotation) example
void mouseButtonDown(int button_id, int state, int x, int y);
void mouseMove(int x, int y);
void keyDown(unsigned char key, int x, int y);

////////////////////////////////////////
// Globals
////////////////////////////////////////
Character* myCharacter = nullptr;
float characterX = 0.0f;
float characterY = -0.61f;
float characterOrientation = 0.0f;
//
// Demo model
//
float theta0 = 0.0f;
float theta1 = 0.1f;
float theta2 = 0.0f;
float theta1b = glm::radians(45.0f);

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
	1.0f, -1.0f, 0.0f,		3.0f, 1.0f,// Bottom right
	1.0f, -0.7f, 0.0f,		3.0f, 0.0f //Top right
};

/////////////////////////////////////////
// Platform varirables
////////////////////////////////////////
GLuint platformVAO, platformVBO, platformTexture;
glm::mat4 platformTransformation;

GLfloat platformVertices[] =
{
	//Vertices			//Texture coords
	0.0f, 0.1f, 0.0f,		0.0f, 0.0f,//Top left 
	0.0f, -0.1f, 0.0f,		0.0f, 1.0f,//Bottom left
	0.4f, -0.1f, 0.0f,		1.0f, 1.0f,// Bottom right
	0.4f, 0.1f, 0.0f,		1.0f, 0.0f //Top right
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

GLuint collectableVAO, collectableVBO, collectableEBO, collectableColoursVBO, collectableT;

/////////////////////////////////
// Character variables
/////////////////////////////////
GLuint bodyTexture, headTexture;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// MAIN FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) 
{
	init(argc, argv);
	/*
	Got the idea of using this from this video -  https://www.cs.iusb.edu/~danav/teach/c481/c481_07a_anim2d.html 
	Originally i did try and use glutIdleFunc but it just became annoyinfg because it was hard to control the animation speed
	and it felt too fast so I switched to glutTimeFunc()
	The initial function call takes 0 ms as it is calling it for the first time where as the timer is set to 50 ms in the recursive 
	call at the bottom of updateSunPosition so that the sun moves at a controlled pace
	*/
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
	setUpPlatformVAOandVBO();
	setUpCollectable();

	// 3. Initialise OpenGL settings and objects we'll use in our scene
	glClearColor(0.07f, 0.13f, 0.17f, 1.0f);

	// Shader setup - more on this next week!!!
	myShaderProgram = setupShaders(string("Shaders\\basic_vertex_shader.txt"), string("Shaders\\basic_fragment_shader.txt"));
	locT = glGetUniformLocation(myShaderProgram, "T");
	locIT = glGetUniformLocation(myShaderProgram, "isTexture");

	//Texture loading
	//background
	// Backgrounds obtained from https://szadiart.itch.io/background-desert-mountains
	backGroundTextures.push_back(backGroundTexture1 =
		wicLoadTexture(L"..\\..\\Common\\Resources\\Textures\\background1.png"));

	backGroundTextures.push_back(backGroundTexture2 =
		wicLoadTexture(L"..\\..\\Common\\Resources\\Textures\\background2.png"));

	backGroundTextures.push_back(backGroundTexture3 =
		wicLoadTexture(L"..\\..\\Common\\Resources\\Textures\\background3.png"));

	//Clouds
	//clouds also obtained from https://szadiart.itch.io/background-desert-mountains
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
		-0.6f, -0.3f, 0.7f, 0.9f
	});
	
	//Both the platform and floor were obtained from https://szadiart.itch.io/pixel-fantasy-caves to which modified after
	floorTexture =
		wicLoadTexture(L"..\\..\\Common\\Resources\\Textures\\Floor.png");

	platformTexture =
		wicLoadTexture(L"..\\..\\Common\\Resources\\Textures\\Platform.png");

	//Sun texture was one I created myself so there is nothing to reference for it
	sunTexture = 
		wicLoadTexture(L"..\\..\\Common\\Resources\\Textures\\Sun.png");

	//Character obtained from  https://penzilla.itch.io/hooded-protagonist
	bodyTexture = 
		wicLoadTexture(L"..\\..\\Common\\Resources\\Textures\\Body.png");

	headTexture = 
		wicLoadTexture(L"..\\..\\Common\\Resources\\Textures\\Head.png");

	// Create new Snowman instance
	myCharacter = new Character(myShaderProgram, bodyTexture, headTexture, locIT, locT);
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
	
	//Draw platform, floor and collectable
	drawFloorVAOandVBO();
	drawPlatformsVAOandVBO();
	drawCollectable();

	//Render character shape hierarchy
	myCharacter->renderCharacter(characterX, characterY, 0.5f, characterOrientation);
	
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

	//For loop to draw all 3 background textures
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

	//Disable blend
	glDisable(GL_BLEND);
}

void drawOneCloud(GLuint cloudTexture, float x1, float x2, float y1, float y2)
{
	//Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Bind and enable texture
	glBindTexture(GL_TEXTURE_2D, cloudTexture);
	glEnable(GL_TEXTURE_2D);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); glVertex2f(x1, y1);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(x2, y1);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(x2, y2);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(x1, y2);
	glEnd();

	//Disable texturing
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void drawClouds() 
{
	//For loop to draw all 6 clouds that are stored in Clouds array
	for (int i = 0; i < Clouds.size(); i++)
	{
		drawOneCloud(Clouds[i].texture, Clouds[i].x1, Clouds[i].x2, Clouds[i].y1, Clouds[i].y2);
	}
}

void drawSun(void)
{
	//Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	//Set and bind texture and enable texture 2d
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, sunTexture);

	glBegin(GL_TRIANGLE_STRIP);

	glTexCoord2f(0.0f, 1.0f); glVertex2f(-0.95f, sunY - 0.4f);
	glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.95f, sunY);
	glTexCoord2f(1.0f, 1.0f); glVertex2f(-0.65f, sunY - 0.4f);
	glTexCoord2f(1.0f, 0.0f); glVertex2f(-0.65f, sunY);

	glEnd();

	//Disable texturing and blending
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void updateSunPosition(int value)
{
	/* 
	Code for this was obtained from https://www.cs.iusb.edu/~danav/teach/c481/c481_07a_anim2d.html 
	where afterwards it was modified to be what it is now
	*/

	/*Sun will move upwards or downwards depending on it it has reached a certain coordinate on the Y axis,
	boolean value will then dictate if it is moving up or down*/
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

	//Redraw the window to update the position of the sun
	glutPostRedisplay();
	//Recursively call function so it continues to update position every 16 milliseconds
	glutTimerFunc(50, updateSunPosition, 0);
}

///////////////////////////////////////////////////////////////////////////////////
// FLOOR AND PLATFORM FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////
void setUpFloorVAOandVBO(void)
{
	/*
	The code for this was obtained by using a tutorial to understand VBOs and VAOs but was modified to be a quad object
	The link to this is https://www.youtube.com/watch?v=XpBGwZNyUh0&list=PLPaoO-vpZnumdcb4tZc4x5Q-v7CkrQ6M-
	Specifically video 2
	*/

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
	/*
	A tutorial was used for this by changing what https://www.youtube.com/watch?v=XpBGwZNyUh0&list=PLPaoO-vpZnumdcb4tZc4x5Q-v7CkrQ6M-
	showed on their tutorial to be a quad, I later redone the tutorial and set that object as the collectable to better understand it
	as I realised I had made it harder for myself by trying to modify the code there than trying to understand it first then modify it.
	I then got stuck on trying to render the texture for this VBO so I got help from my lecturer Carl to help me and applied the way the
	code for the texture to render onto to the VBO worked on the platforms aswell
	*/

	//Enable texturing and blending
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Bind the texture, set uniform location of texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, floorTexture);
	glUniform1i(locIT, 1);

	//Set texture parameters so floor is not stretched
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   // Wrap horizontally
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);   // Wrap vertically
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // Minification filter

	//Bind and draw object
	glBindVertexArray(floorVAO);
	glDrawArrays(GL_QUADS, 0, 4);
		
	//Unbind object and uniform location
	glBindVertexArray(0);
	glUniform1i(locIT, 0);

	//Disable texturing and blending
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void setUpPlatformVAOandVBO(void)
{
	/*
	This was copied over from the floor code but originated from video 2 of this tutorial:
	https://www.youtube.com/watch?v=XpBGwZNyUh0&list=PLPaoO-vpZnumdcb4tZc4x5Q-v7CkrQ6M-
	*/

	//Generate the VAO
	glGenVertexArrays(1, &platformVAO);
	//Generate the VBO
	glGenBuffers(1, &platformVBO);

	//Bind the vertex array
	glBindVertexArray(platformVAO);
	//Bind the buffer object for the VBO  
	glBindBuffer(GL_ARRAY_BUFFER, platformVBO);
	//Store vertices in VBO      
	glBufferData(
		GL_ARRAY_BUFFER,	//Specify type of buffer
		sizeof(platformVertices),		//Total size of data
		platformVertices,				//Give the actual vertices
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

void drawPlatformsVAOandVBO(void)
{
	//Enable blending and texturing
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//Set the active texture, bind the texture and set the uniform location
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, platformTexture);
	glUniform1i(locIT, 1);

	//Bind VAO
	glBindVertexArray(platformVAO);
	//Draw platform 1
	glDrawArrays(GL_QUADS, 0, 4);

	//Make the transformation 
	platformTransformation = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.4f, 0.0f));
	//Pass transform to shader
	glUniformMatrix4fv(locT, 1, GL_FALSE, glm::value_ptr(platformTransformation));
	
	//Draw platform 2 now transformation has been applied
	glDrawArrays(GL_QUADS, 0, 4);

	//Unbind vertexes, uniforms, textures and blending
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
	/*
	I used videos 2, 3, and parts of 5 for this collectable, here is the tutorial link below:
	https://www.youtube.com/watch?v=XpBGwZNyUh0&list=PLPaoO-vpZnumdcb4tZc4x5Q-v7CkrQ6M-
	*/

	//Generate VAO
	glGenVertexArrays(1, &collectableVAO);
	//Generate VBO
	glGenBuffers(1, &collectableVBO);
	//Generate EBO, this is a buffer that allows users to reuse vertices to create
	// multiple primitives where a primitive is made up of one or more vertices
	glGenBuffers(1, &collectableEBO);
	//Generate VBO for colours
	glGenBuffers(1, &collectableColoursVBO);

	//Bind VAO
	glBindVertexArray(collectableVAO);

	//Bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, collectableVBO);
	//Store vertices in VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(collectableVertices), collectableVertices, GL_STATIC_DRAW);

	//Bind EBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, collectableEBO);
	//Store vertices in EBO
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(collectableIndices), collectableIndices, GL_STATIC_DRAW);

	//Pass index of attribute we want to use
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//Enable vertex attribute
	glEnableVertexAttribArray(0);

	// Bind color VBO
	glBindBuffer(GL_ARRAY_BUFFER, collectableColoursVBO);
	//Pass vertices into Colour VBO
	glBufferData(GL_ARRAY_BUFFER, sizeof(collectableColours), collectableColours, GL_STATIC_DRAW);

	// Set color attribute (location = 1 in the shader)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//Enable vertex attribute
	glEnableVertexAttribArray(1);

	//Unbind buffer, vertex array and buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void drawCollectable(void)
{
	/*
	FOR SOME REASON WHEN I WAS TRYING TO ALTER THE COORDINATES IN THE VERTICES ARRAY FOR THE COLLECTABLE IT ENDED UP MESSING WITH THE SHAPE
	AND MAKING IT ALL SQUASHED SO I DECIDED IT WAS EASIER TO USE A TRANSLLATION MATRIX TO SHIFT THE WHOLE SHAPE TO WHERE I WANTED
	*/

	// Create the translation matrix
	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.2f, 0.22f, 0.0f));

	//Get uniform location
	collectableT = glGetUniformLocation(myShaderProgram, "T");

	// Pass the matrix to the shader
	glUniformMatrix4fv(collectableT, 1, GL_FALSE, glm::value_ptr(translationMatrix));

	//Bind VAO
	glBindVertexArray(collectableVAO);

	//Draw collectable
	glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

	//Unbind vertex array
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
	//Do not want a mouse move
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

			characterOrientation += float(dy);
		}
		else 
		{
			characterX += float(dx) * 0.0025f;
			characterY -= float(dy) * 0.0025f;
		}

		mouse_x = x;
		mouse_y = y;

		glutPostRedisplay();
	}
}


void keyDown(unsigned char key, int x, int y) 
{
	if (key == 'a') 
	{
		characterX -= 0.05f;
		characterY = -0.61;
		characterOrientation = 0.0f;

		glutPostRedisplay();
	}
	else if (key == 'd') 
	{
		characterX += 0.05f;
		characterY = -0.61;
		characterOrientation = 0.0f;

		glutPostRedisplay();
	}
	else if (key == 'r')
	{
		characterX = 0.0f;
		characterY = -0.61;
		characterOrientation = 0.0f;

		glutPostRedisplay();
	}
	else if (key == ' ')
	{
		characterX += 0.05f;
		characterY += 0.1f;
		characterOrientation = 0.0f;

		glutPostRedisplay();
	}
}

