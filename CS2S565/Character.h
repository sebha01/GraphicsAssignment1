#pragma once

#include <glew\glew.h>
#include <freeglut\freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "texture_loader.h"
#include "shader_setup.h"


class Square;

class Character 
{
	// Charcater made up of 2 squares, head and body
	Square *characterComponent;

	// Textures for character body and head
	GLuint bodyTexture, headTexture;

	// Shader program for drawing the character
	GLuint characterShader;

	// Uniform character texture variable location in character shader
	GLuint locT;
	GLuint locCT;

	public:
		// Default constructor
		Character(GLuint& shader, GLuint& texture1, GLuint& texture2, GLuint locIT, GLuint T);

		// Render snowman object.  x, y represent the position of the snowman's body, scale determines the scale of the snowman and orientation is the angle of the snowman (in degrees)
		void renderCharacter(float x, float y, float scale, float orientation);
};
