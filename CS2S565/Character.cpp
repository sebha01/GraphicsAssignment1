#include "Character.h"
#include "Square.h"

using namespace std;

Character::Character(GLuint& shader, GLuint& texture1, GLuint& texture2, GLuint locIT, GLuint T) 
{
	characterComponent = new Square();

	// Load texture images
	this->bodyTexture = texture1;
	this->headTexture = texture2;

	// Load shaders that make up the character program
	this->characterShader = shader;

	// Setup uniform locations
	this->locT = T;
	this->locCT = locIT;
}

// Render character object.  x, y represent the position of the snowman's body, scale determines 
// the scale of the snowman and orientation is the angle of the character (in degrees)
void Character::renderCharacter(float x, float y, float scale, float orientation) 
{
	//// "Plug in" the snowman shader into the GPU
	glUseProgram(characterShader);

	// Draw the main body
	// Create matrices based on input parameters
	glm::mat4x4 bodyTransform = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f))*
								glm::rotate(glm::mat4(1.0f), glm::radians(orientation), glm::vec3(0.0f, 0.0f, 1.0f))*
								glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, 1.0f));

	// Upload body transform to shader
	glUniformMatrix4fv(locT, 1, GL_FALSE, (GLfloat*)&bodyTransform);

	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE0);

	// Use the snow texture for the main body
	glBindTexture(GL_TEXTURE_2D, bodyTexture);
	glUniform1i(locCT, 2);

	// Draw the square for the body
	characterComponent->render();

	//Unbvid uniforms and arrays and disable textures and blending
	glBindVertexArray(0);
	glUniform1i(locCT, 0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	// draw head
	glm::mat4x4 body_to_head_transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.001f, 0.345, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

	// multiply head object with parent object
	glm::mat4x4 headTransform = bodyTransform * body_to_head_transform;

	// Upload the final head transform to the shader
	glUniformMatrix4fv(locT, 1, GL_FALSE, (GLfloat*)&headTransform);

	// Bind the head texture
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, headTexture);

	glUniform1i(locCT, 2);

	// Draw the square for the head
	characterComponent->render();

	glBindVertexArray(0);
	glUniform1i(locCT, 0);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}
