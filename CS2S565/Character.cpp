

#include "Character.h"
#include "Square.h"

using namespace std;



Character::Character() {

	characterComponent = new Square();

	// Load texture images
	bodyTexture = wicLoadTexture(wstring(L"Resources\\Textures\\Square.png"));
	headTexture = wicLoadTexture(wstring(L"Resources\\Textures\\Head.png"));

	// Load shaders that make up the snowmanShader program
	characterShader = setupShaders(string("Shaders\\basic_vertex_shader.txt"), string("Shaders\\basic_fragment_shader.txt"));

	// Setup uniform locations
	locT = glGetUniformLocation(characterShader, "T");
}



// Render snowman object.  x, y represent the position of the snowman's body, scale determines the scale of the snowman and orientation is the angle of the snowman (in degrees)
void Character::renderCharacter(float x, float y, float scale, float orientation) {

	// "Plug in" the snowman shader into the GPU
	glUseProgram(characterShader);


	// 1. Draw the main body

	// Create matrices based on input parameters
	glm::mat4x4 bodyTransform = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f))*
		glm::rotate(glm::mat4(1.0f), glm::radians(orientation), glm::vec3(0.0f, 0.0f, 1.0f))*
		glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, 1.0f));

	// Upload body transform to shader
	glUniformMatrix4fv(locT, 1, GL_FALSE, (GLfloat*)&bodyTransform);

	// Use the snow texture for the main body
	glBindTexture(GL_TEXTURE_2D, bodyTexture);

	// Draw the circle for the body
	characterComponent->render();



	// 2. draw head

	// - Setup the RELATIVE transformation from the centre of the body to where the head's origin will be
	// - Like the body, the head uses the same circle model so the origin will be in the middle of the head
	// - Offsets are calculated in terms of the parent object's modelling coordinates.  Any scaling, rotation etc. of the parent will be applied later in the matrix sequence.
	// - This makes relative modelling easier - we don't worry about what transforms have already been applied to the parent.
	glm::mat4x4 body_to_head_transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.25f, 0.0f)) * glm::scale(glm::mat4(1.0f), glm::vec3(0.65f, 0.65f, 1.0f));

	// Since we're working with a relative transform, we must multiply this with the parent objects's (that is, the body's) transform also
	// REMEMBER: THE RELATIVE TRANSFORM GOES LAST IN THE MATRIX MULTIPLICATION SO IT HAPPENS FIRST IN THE SEQUENCE OF TRANSFORMATIONS
	glm::mat4x4 headTransform = bodyTransform * body_to_head_transform;

	// Upload the final head transform to the shader
	glUniformMatrix4fv(locT, 1, GL_FALSE, (GLfloat*)&headTransform);

	// Bind the head texture
	glBindTexture(GL_TEXTURE_2D, headTexture);

	// Draw the circle for the head
	characterComponent->render();
}
