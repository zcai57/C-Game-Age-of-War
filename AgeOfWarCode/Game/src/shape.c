#include <windows.h>											// Header File For Windows
#include <math.h>
#include <stdio.h>
#include <gl\gl.h>												// Header File For The OpenGL32 Library
#include <gl\glu.h>												// Header File For The GLu32 Library
#include "glut.h"

#include "baseTypes.h"

const float DEG2RAD = 3.14159f/180.0f;

/// @brief Draws a circle to the screen w/ the given properties
/// @param radius 
/// @param x center X
/// @param y center Y
/// @param r red
/// @param g green
/// @param b blue
/// @param filled solid circle, if true, outline otherwise
void shapeDrawCircle(float radius, float x, float y, uint8_t r, uint8_t g, uint8_t b, bool filled)
{	
	glEnable(GL_POINT_SMOOTH);
	glDisable(GL_TEXTURE_2D);
	if(!filled)
	{
		glColor4ub(r, g, b, 0xFF);
		// Set the point size
		glPointSize(1.0);
		glBegin(GL_POINTS);
		float radiusMax = radius + 2.0f;
		float radiusMin = radius - 2.0f;
		for (int i=0; i < 360; i+=3)
		{
			float degInRad = i*DEG2RAD;
			glVertex2f(x + (cos(degInRad)*radius),y + (sin(degInRad)*radius));
			glVertex2f(x + (cos(degInRad)*radiusMax),y + (sin(degInRad)*radiusMax));
			glVertex2f(x + (cos(degInRad)*radiusMin),y + (sin(degInRad)*radiusMin));
		}
		glEnd();
	}
	else
	{
	    glColor4ub(r, g, b, 0xFF);
		glEnable(GL_BLEND); 
	    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
		glPointSize(radius * 2);
		glBegin(GL_POINTS);
		glVertex2f(x, y);
		glEnd();
	}
}

/// @brief Draws a line to the screen with the given properties
/// @param startX 
/// @param startY 
/// @param endX 
/// @param endY 
/// @param r red
/// @param g green
/// @param b blue
void shapeDrawLine(float startX, float startY, float endX, float endY, uint8_t r, uint8_t g, uint8_t b)
{
	glColor3ub(r, g, b);
	// Draw filtered lines
	glEnable(GL_LINE_SMOOTH);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBegin(GL_LINE_STRIP);
		glVertex2f(startX, startY);
		glVertex2f(endX, endY);
	glEnd();

}
/// @brief Draws a rectange to the screen with the given properties
/// @param startX 
/// @param startY 
/// @param r red
/// @param g green
/// @param b blue
/// @param length
/// @param height
void shapeDrawRectangle(float startX, float startY, uint8_t r, uint8_t g, uint8_t b, float length, float height) {
	glColor4ub(r, g, b, 0xFF);
	/*glColor3ub(r, g, b);*/
	glEnable(GL_POINT_SMOOTH);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	const float BG_DEPTH = -0.99f;

	// TL
	glVertex2f(startX, startY);
	// BL
	glVertex2f(startX, startY + height);
	// BR
	glVertex2f(startX + length, startY + height);
	// TR
	glVertex2f(startX + length, startY);
	
	glEnd();

}
