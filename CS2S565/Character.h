
#pragma once

#include <glew\glew.h>
#include <freeglut\freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "texture_loader.h"
#include "shader_setup.h"


class Square;

class Character {

	// Snowman made up of multiple circle objects (we store just one instance and render this multiple times)
	Square *characterComponent;

	// Textures for snowman body and head
	GLuint bodyTexture, headTexture;

	// Shader program for drawing the snowman
	GLuint characterShader;

	// Uniform variable locations in snowmanShader
	GLuint locT;


public:

	// Default constructor
	Character();

	// Render snowman object.  x, y represent the position of the snowman's body, scale determines the scale of the snowman and orientation is the angle of the snowman (in degrees)
	void renderCharacter(float x, float y, float scale, float orientation);

	~Character();

};
