#include "Square.h"

using namespace std;

struct GUVector2
{
	float x;
	float y;
};

Square::Square() 
{
	
	// Setup vertex xy coordinates and texture coordinates (modelling coordinates)
	GUVector2* vertex_xy_coords = (GUVector2*)malloc(4 * sizeof(GUVector2)); // Only need 4 vertices for a square
	GUVector2* vertex_texture_coords = (GUVector2*)malloc(4 * sizeof(GUVector2));

	if (vertex_xy_coords && vertex_texture_coords) 
	{
		// Square vertices
		vertex_xy_coords[0].x = 0.0f; vertex_xy_coords[0].y = -0.2f; // Bottom left
		vertex_xy_coords[1].x = 0.3f; vertex_xy_coords[1].y = -0.2f; // Bottom right
		vertex_xy_coords[2].x = 0.3f; vertex_xy_coords[2].y = 0.2f; // Top right
		vertex_xy_coords[3].x = 0.0f; vertex_xy_coords[3].y = 0.2f; // Top left

		// Define corresponding texture coordinates (assuming the texture spans the whole square)
		vertex_texture_coords[0].x = 0.0f; vertex_texture_coords[0].y = 1.0f; // Bottom left
		vertex_texture_coords[1].x = 1.0f; vertex_texture_coords[1].y = 1.0f; // Bottom right
		vertex_texture_coords[2].x = 1.0f; vertex_texture_coords[2].y = 0.0f; // Top right
		vertex_texture_coords[3].x = 0.0f; vertex_texture_coords[3].y = 0.0f; // Top left

		// Setup VBOs

		glGenVertexArrays(1, &squareVAO);
		glBindVertexArray(squareVAO);

		// Copy vertex position data to VBO
		glGenBuffers(1, &squareVertexPositionVBO);
		glBindBuffer(GL_ARRAY_BUFFER, squareVertexPositionVBO);
		glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(GUVector2), vertex_xy_coords, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);

		// Copy texture coordinate data to VBO
		glGenBuffers(1, &squareTexCoordVBO);
		glBindBuffer(GL_ARRAY_BUFFER, squareTexCoordVBO);
		glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(GUVector2), vertex_texture_coords, GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);

		// Enable position and texture coordinate buffer inputs
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);

		// Once VBOs setup we can dispose of the buffers in system memory - they are no longer needed
		free(vertex_xy_coords);
		free(vertex_texture_coords);
	}
}


void Square::render(void) 
{
	glBindVertexArray(squareVAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}
