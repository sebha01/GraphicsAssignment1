
#pragma once

#include <glew\glew.h>
#include <freeglut\freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "texture_loader.h"
#include "shader_setup.h"



class Square {

	// Variables to represent the VBO and VAO of the circle object
	GLuint squareVAO, squareVertexPositionVBO, squareTexCoordVBO;

public:

	// Default constructor - setup circle with unit radius
	Square();

	// Render circle - all relevant transformations are assumed to be setup before calling this function
	void render(void);

};
