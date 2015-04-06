#ifndef FONT_CPP
#define FONT_CPP
#include <GL/glut.h>
#include <stdlib.h>
#include <string>
namespace slee17 {
//from the web opengl tutorial. lost the link so can't state the author.
//but it's not my function.
static void DrawText(GLint x, GLint y, char* s,
              GLfloat r, GLfloat g, GLfloat b)
{
    int lines;
    char* p;
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
	glOrtho(0.0, glutGet(GLUT_WINDOW_WIDTH),
			0.0, glutGet(GLUT_WINDOW_HEIGHT), -1.0, 14.0);
    //glOrtho(0.0, glutGet(GLUT_WINDOW_WIDTH),
    //        0.0, glutGet(GLUT_WINDOW_HEIGHT), -1.0, 14.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glColor3f(r,g,b);
    glRasterPos2i(x, y);
    for(p = s, lines = 0; *p; p++) {
        if (*p == '\n') {
            lines++;
            glRasterPos2i(x, y-(lines*18));
        }//if
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
    }//for
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}//DrawText
}//slee17
#endif

