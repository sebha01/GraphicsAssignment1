

#include "circle.h"

using namespace std;
struct GUVector2
{
	float x;
	float y;
};
const float gu_pi = 3.1416;

// Global variable to determine number of points on the circle.  Since we draw the filled in circle with triangle fan the actual number of points stored = num_points_on_circle + 2 since we need to store the point at the centre of the circle first and the first point on the circumference at the end of the triangle fan point list to close the loop
const int num_points_on_circle = 32;
const int num_points = num_points_on_circle + 2;

Circle::Circle() {

	// Setup circle VBO

	// Setup vertex xy coordinates and texture coordinates (modelling coordinates)
	GUVector2 *vertex_xy_coords = (GUVector2*)malloc(num_points * sizeof(GUVector2));
	GUVector2 *vertex_texture_coords = (GUVector2*)malloc(num_points * sizeof(GUVector2));

	if (vertex_xy_coords && vertex_texture_coords) {

		// Store point at origin (centre of circle / triangle fan used to render the circle)
		vertex_xy_coords[0].x = 0.0f;
		vertex_xy_coords[0].y = 0.0f;

		vertex_texture_coords[0].x = 0.5f;
		vertex_texture_coords[0].y = 0.5f;

		// Setup points around the circumference of the circle
		float theta = 0.0f;
		float theta_delta = (gu_pi * 2.0f) / float(num_points_on_circle);

		for (int i = 1; i<num_points; ++i, theta += theta_delta) {

			float x = cosf(theta);
			float y = sinf(theta);

			// Setup vertex coordinate
			vertex_xy_coords[i].x = x;
			vertex_xy_coords[i].y = y;

			// Setup corresponding texture coordinate
			vertex_texture_coords[i].x = (x * 0.5f) + 0.5f;
			vertex_texture_coords[i].y = 1.0f - ((y * 0.5f) + 0.5f);
		}

		// Setup VBOs

		glGenVertexArrays(1, &circleVAO);
		glBindVertexArray(circleVAO);

		// Copy vertex position data to VBO
		glGenBuffers(1, &vertexPositionVBO);
		glBindBuffer(GL_ARRAY_BUFFER, vertexPositionVBO);
		glBufferData(GL_ARRAY_BUFFER, num_points * sizeof(GUVector2), vertex_xy_coords, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);

		// Copy texture coordinate data to VBO
		glGenBuffers(1, &texCoordVBO);
		glBindBuffer(GL_ARRAY_BUFFER, texCoordVBO);
		glBufferData(GL_ARRAY_BUFFER, num_points * sizeof(GUVector2), vertex_texture_coords, GL_STATIC_DRAW);
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


void Circle::render(void) {

	glBindVertexArray(circleVAO);
	glDrawArrays(GL_TRIANGLE_FAN, 0, num_points);
}
