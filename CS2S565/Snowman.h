
#pragma once

#include <glew\glew.h>
#include <freeglut\freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "texture_loader.h"
#include "shader_setup.h"


class Circle;

class Snowman {

	// Snowman made up of multiple circle objects (we store just one instance and render this multiple times)
	Circle *snowmanComponent;

	// Textures for snowman body and head
	GLuint bodyTexture, headTexture;

	// Shader program for drawing the snowman
	GLuint snowmanShader;

	// Uniform variable locations in snowmanShader
	GLuint locT;


public:

	// Default constructor
	Snowman();

	// Render snowman object.  x, y represent the position of the snowman's body, scale determines the scale of the snowman and orientation is the angle of the snowman (in degrees)
	void renderSnowman(float x, float y, float scale, float orientation);

};
