#ifndef MAIN_CPP
#define MAIN_CPP
//project #3
//samuel lee
//draws a teapot using opengl pipeline
//data of the teapot is in files pat and col
//this program read the two files and calculates vertices and instructs
//opengl to draw polygons and triangle_loop (mesh outline)...

//IMPORTANT FUNCTIONS:
//makeBezierPolygons takes two file names to read,
//an int resolution (how many times a patch should be divided into).
//resolution is square root of # of squares a patch should have.
//Ex, resolution of 2 means there will be 2^2 = 4 square polygons/
//or 4*2 = 8 triangle polygons.

//to compile and run, open visual c++ file and click execute button
//or just try double clicking exe file under Debug directory
#include <cmath>
#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <GL/glut.h>// Graphics libraries
#include <vector>
#include <map>
#include <utility>
#include <string>
#include <sstream>
#include "graphicUtils.cpp" //has core data structures used in the program
using slee17::xyz;
using slee17::rgba;
using slee17::point;
using slee17::triangle;
using slee17::perspectiveViewVolume;
using slee17::orthographicViewVolume;
using slee17::matrix;

#include "font.cpp"//from the redbook. to draw fonts in the window
using slee17::DrawText;
using namespace std;

string intToString( int i ) {
	ostringstream oss;
	oss<<i;
	return oss.str();
}//intToString

//void constToCstring( const char* c )

template <typename T>
struct tuple {//tuple is a collection of same 3 data type
    T t1, t2, t3;
};//tuple

//pointer to a triangle polygon
typedef vector<triangle>::iterator polygon_ptr;

bool isOrtho = true;//is orthographic projection??
bool isRawFile = false;//should I read a raw file??
bool isMesh = false;//display polygon mesh??
bool isColor = false;//display colored object??
//enum Shading { GOURAUD = 0, FLAT, FLAT2, MESH };//Shading
enum Shading { GOURAUD = 0, FLAT, FLAT2, MESH };//Shading
//GOURAUD uses each vertex's defined normal.
//FLAT uses calculated normal for a triangle
//FLAT2 uses average of 3 vertices' normal.
Shading& operator++(Shading& s) {
    s = static_cast<Shading>((s+1)%4);
    return s;
}//++
Shading shadingMode = GOURAUD;//which shading mode is being used?

//possible resolutions for a patch
int RESOLUTION[] = {2, 4, 6, 8, 10, 12};
int RES_index = 1, RES_size = 6;

double rotation_x = 0.0, rotation_y = 0.0;
GLfloat objDiffuse[] = {0.0, 0.75, 0.25, 1.0};//Object material properties
GLfloat objAmb[] = {0.0, 0.6f, 0.2f, 1.0};
GLfloat objSpec[] = {0.5, 0.5, 0.5, 1.0};
GLfloat objSpecExp[] = {20.0};

//perspective view volume. 1.3333 = 4/3
perspectiveViewVolume pViewVol(90.0, 1.3333, 1.0, 25.0);

//orthographic view volume.
orthographicViewVolume oViewVol(-13.3, 13.3, -9.975, 9.975, 1.0, 25.0);
//orthographicViewVolume oViewVol(-10.7, 10.7, -8.025, 8.025, 1.0, 14.0);

void myDisplay(void);//main display function
void myMouse(int button, int state, int x, int y);//mouse function
void myKeyboard_s(int key, int x, int y);
void myKeyboard( unsigned char key, int x, int y );
void myReshape(GLsizei w, GLsizei h);//reshape function
void drawobj( multimap<string,triangle>::iterator b,
	multimap<string,triangle>::iterator e,
	GLfloat *diffuse,
	GLfloat *ambient,
	GLfloat *spec,
	GLfloat *specExponent,
	Shading s);
void makeBezierPolygons( const string& cor, const string& pat, int resolution,
	multimap<string, triangle>& polygons);

//@@@@@@@@@@@@@@@@@@ data structures to store data file @@@@@@@@@@
//growable array of triangles. one triangle has three vertices
multimap<string, triangle> teapot;

int main(int argc, char** argv)
{
  glutInit(&argc, argv);
	//default resolution is 4. change the resolution by pressing r key.
	makeBezierPolygons("cor", "pat", RESOLUTION[RES_index], teapot);

    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH);   // RGB & Z buffers
    glutInitWindowSize(800, 600 );//4/3 aspect
    glutInitWindowPosition(1, 1);
    glutCreateWindow("arrow:rotates // space bar:restores // c:color // r:resolution");

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(oViewVol.left, oViewVol.right, oViewVol.bottom, oViewVol.top,
            oViewVol.close, oViewVol.distant);
    glEnable(GL_DEPTH_TEST);// Enable Z buffer
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);// Allow non-unit length surface normals

    glMatrixMode(GL_MODELVIEW);// Set matrix mode to object modeling
    glLoadIdentity();
    glTranslated(0, 0,-13);//to push objects into view volume

	GLfloat lightPosition[] = {20.0, 20.0, 20.0, 1.0};// Point light
	GLfloat amb[] = {0.1f, 0.1f, 0.1f, 1.0};//Ambient light for the entire scene
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);// Local viewer
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);// Define LIGHT0
    glEnable(GL_LIGHT0);// Enable LIGHT0
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);// Global ambient light

	/*
	GLfloat lightPosition2[] = {-20.0, -30.0, 30.0, 1.0};// Point light
	GLfloat amb2[] = {0.2f, 0.2f, 0.2f, 1.0};//Ambient light for the entire scene
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);// Local viewer
    glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);// Define LIGHT1
    glEnable(GL_LIGHT1);// Enable LIGHT0
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb2);// Global ambient light
	*/

	glutDisplayFunc(myDisplay);// Register redraw function
    glutMouseFunc(myMouse);// Register mouse button function
    glutReshapeFunc(myReshape);// Register resize function
	glutKeyboardFunc(myKeyboard);//Register keyboard function
	glutSpecialFunc(myKeyboard_s);//Register arrow keys for rotation
    glutMainLoop();// Begin event-processing loop
    return 0;
}//main

void myKeyboard( unsigned char key, int x, int y ) {
	//glFlush();
	switch(key) {
		case ' ':
			rotation_x = 0.0;
			rotation_y = 0.0;
			break;
		case 'c':
			isColor = !isColor;
			break;
		case 'r':
			RES_index++;
			RES_index = RES_index % RES_size;
			teapot.clear();
			makeBezierPolygons("cor", "pat", RESOLUTION[RES_index], teapot);
			//cout<<RESOLUTION[RES_index]<<endl;
		//	isMesh != isMesh;
			break;
	}//switch
	glPushMatrix();
	glRotated(rotation_x, 1, 0, 0);
	glRotated(rotation_y, 0, 1, 0);
	myDisplay();
	glPopMatrix();
}//myKeyboard

void myKeyboard_s(int key, int x, int y) {
	switch (key) {
		case GLUT_KEY_UP:
			rotation_x -= 10;
			break;
		case GLUT_KEY_DOWN:
			rotation_x += 10;
			break;
		case GLUT_KEY_LEFT:
			rotation_y += 10;
			break;
		case GLUT_KEY_RIGHT:
			rotation_y -= 10;
			break;
	}//switch
	if( rotation_x > 359 ) rotation_x = 0.0;
	if( rotation_y > 359 ) rotation_y = 0.0;
	glPushMatrix();
	glRotated(rotation_x, 1, 0, 0);
	glRotated(rotation_y, 0, 1, 0);
	myDisplay();
	glPopMatrix();
}//myKeyboard_s

void myDisplay(void)// Called by system to display/redisplay
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();//saves current matrix
	//shared teapot transformation
	glTranslated(0, -5, 0);
	glRotated(-40, 0, 1, 0);
	glRotated(-90, 1, 0, 0);
	glScaled(3,3,3);

    //=============================={ teapot }========================
	if( isColor ) {
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		GLfloat teapot_body_amb[] = {0.9, 0.9, 0.3, 1.0};
		GLfloat teapot_body_diff[] = {1.0, 1.0, 0.2, 1.0};
		GLfloat teapot_body_spec[] = {0.7, 0.7, 0.7, 1.0};
		GLfloat teapot_body_spec_exp[] = {30.0};
		glPushMatrix();
		drawobj(teapot.lower_bound("body"), teapot.upper_bound("body"),
			teapot_body_amb, teapot_body_diff, teapot_body_spec, teapot_body_spec_exp,
			shadingMode);
		glPopMatrix();

		GLfloat teapot_rim_amb[] = {0.9, 0.9, 0.9, 1.0};
		GLfloat teapot_rim_diff[] = {0.8, 0.8, 0.8, 1.0};
		GLfloat teapot_rim_spec[] = {0.3, 0.3, 0.3, 1.0};
		GLfloat teapot_rim_spec_exp[] = {50.0};
		glPushMatrix();
		drawobj(teapot.lower_bound("rim"), teapot.upper_bound("rim"),
			teapot_rim_amb, teapot_rim_diff, teapot_rim_spec, teapot_rim_spec_exp,
			shadingMode);
		glPopMatrix();

		GLfloat teapot_nose_amb[] = {0.2, 0.9, 0.2, 1.0};
		GLfloat teapot_nose_diff[] = {0.0, 1.0, 0.0, 1.0};
		GLfloat teapot_nose_spec[] = {0.7, 0.7, 0.7, 1.0};
		GLfloat teapot_nose_spec_exp[] = {100.0};
		glPushMatrix();
		drawobj(teapot.lower_bound("nose"), teapot.upper_bound("nose"),
			teapot_nose_amb, teapot_nose_diff, teapot_nose_spec, teapot_nose_spec_exp,
			shadingMode);
		glPopMatrix();

		GLfloat teapot_handle_amb[] = {0.2, 0.9, 0.2, 1.0};
		GLfloat teapot_handle_diff[] = {0.5, 1.0, 0.5, 1.0};
		GLfloat teapot_handle_spec[] = {0.3, 0.3, 0.3, 1.0};
		GLfloat teapot_handle_spec_exp[] = {30.0};
		glPushMatrix();
		drawobj(teapot.lower_bound("handle"), teapot.upper_bound("handle"),
			teapot_handle_amb, teapot_handle_diff, teapot_handle_spec, teapot_handle_spec_exp,
			shadingMode);
		glPopMatrix();

		GLfloat teapot_cover_amb[] = {0.9, 0.9, 0.3, 1.0};
		GLfloat teapot_cover_diff[] = {1.0, 1.0, 0.7, 1.0};
		GLfloat teapot_cover_spec[] = {0.5, 0.5, 0.5, 1.0};
		GLfloat teapot_cover_spec_exp[] = {70.0};
		glPushMatrix();
		drawobj(teapot.lower_bound("cover"), teapot.upper_bound("cover"),
			teapot_cover_amb, teapot_cover_diff, teapot_cover_spec, teapot_cover_spec_exp,
			shadingMode);
		glPopMatrix();

		GLfloat teapot_nip_amb[] = {0.9, 0.3, 0.3, 1.0};
		GLfloat teapot_nip_diff[] = {0.9, 0.2, 0.1, 1.0};
		GLfloat teapot_nip_spec[] = {0.7, 0.7, 0.7, 1.0};
		GLfloat teapot_nip_spec_exp[] = {70.0};
		glPushMatrix();
		drawobj(teapot.lower_bound("nip"), teapot.upper_bound("nip"),
			teapot_nip_amb, teapot_nip_diff, teapot_nip_spec, teapot_nip_spec_exp,
			shadingMode);
		glPopMatrix();

		GLfloat teapot_bottom_amb[] = {0.9, 0.9, 0.9, 1.0};
		GLfloat teapot_bottom_diff[] = {0.8, 0.8, 0.8, 1.0};
		GLfloat teapot_bottom_spec[] = {0.7, 0.7, 0.7, 1.0};
		GLfloat teapot_bottom_spec_exp[] = {170.0};
		glPushMatrix();
		drawobj(teapot.lower_bound("bottom"), teapot.upper_bound("bottom"),
			teapot_bottom_amb, teapot_bottom_diff, teapot_bottom_spec, teapot_bottom_spec_exp,
			shadingMode);
		glPopMatrix();

		glPopMatrix();//restores matrix saved
	}//if
	else {//mono
		//glDisable(GL_BLEND);
		GLfloat teapot_amb[] = {0.5, 0.5, 0.5, 1.0};
		GLfloat teapot_diff[] = {0.9, 0.9, 0.9, 1.0};
		GLfloat teapot_spec[] = {0.7, 0.7, 0.7, 1.0};
		GLfloat teapot_spec_exp[] = {20.0};

		glPushMatrix();
		drawobj(teapot.lower_bound("body"), teapot.upper_bound("body"),
			teapot_amb, teapot_diff, teapot_spec, teapot_spec_exp,
			shadingMode);
		glPopMatrix();

		glPushMatrix();
		drawobj(teapot.lower_bound("rim"), teapot.upper_bound("rim"),
			teapot_amb, teapot_diff, teapot_spec, teapot_spec_exp,
			shadingMode);
		glPopMatrix();

		glPushMatrix();
		drawobj(teapot.lower_bound("nose"), teapot.upper_bound("nose"),
			teapot_amb, teapot_diff, teapot_spec, teapot_spec_exp,
			shadingMode);
		glPopMatrix();

		glPushMatrix();
		drawobj(teapot.lower_bound("handle"), teapot.upper_bound("handle"),
			teapot_amb, teapot_diff, teapot_spec, teapot_spec_exp,
			shadingMode);
		glPopMatrix();

		glPushMatrix();
		drawobj(teapot.lower_bound("cover"), teapot.upper_bound("cover"),
			teapot_amb, teapot_diff, teapot_spec, teapot_spec_exp,
			shadingMode);
		glPopMatrix();

		glPushMatrix();
		drawobj(teapot.lower_bound("nip"), teapot.upper_bound("nip"),
			teapot_amb, teapot_diff, teapot_spec, teapot_spec_exp,
			shadingMode);
		glPopMatrix();

		glPushMatrix();
		drawobj(teapot.lower_bound("bottom"), teapot.upper_bound("bottom"),
			teapot_amb, teapot_diff, teapot_spec, teapot_spec_exp,
			shadingMode);
		glPopMatrix();

		glPopMatrix();//restores matrix saved
	}//else
	glPopMatrix();//restores matrix saved
	//========================={ end of teapot }=====================

    //text display
    switch( shadingMode ) {
		case GOURAUD:
			DrawText(10,20,"GOURAUD SHADING", 1.0, 1.0, 0.0);
			break;
		case FLAT:
			DrawText(10,20,"FLAT SHADING (crossproduct)", 1.0, 1.0, 0.0);
			break;
		case FLAT2:
			DrawText(10,20,"FLAT2 SHADING (mean of normals)", 1.0, 1.0, 0.0);
			break;
		case MESH:
			DrawText(10,20,"POLYGON MESH", 1.0, 1.0, 1.0);
			break;
    }//switch
    if( isOrtho ) {
        DrawText(10,40,"orthographic", 1.0, 1.0, 1.0);
    }//if
    else {
        DrawText(10,40,"perspective", 1.0, 1.0, 1.0);
    }//else
	if( isColor ) {
		DrawText( 10, 60, "color [c to toggle]", 1.0, 1.0, 1.0 );
	}//if
	else {
		DrawText( 10, 60, "mono [c to toggle]", 1.0, 1.0, 1.0 );
	}//else
	string resolution_note = intToString(RESOLUTION[RES_index]);
	resolution_note.append(" [r to change resolution]");
	char *message = (char*)resolution_note.c_str();
	DrawText( 10, 80, message, 1.0, 1.0, 1.0 );
	DrawText( 10, 100, "use arrow keys to rotate the teapot", 1.0, 1.0, 1.0 );
    glFlush();
	//glutSwapBuffers();
}//myDisplay
// Use glScaled(Sx, Sy, Sz) to scale

void myMouse(int button, int state, int x, int y)// Called when there is a
{// mouse-button event
    switch (button) {// Switch based on event type
    case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN) {                // Button down
            if( isOrtho ) {//change to perspective projection
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                gluPerspective(pViewVol.angle, pViewVol.aspect,
                               pViewVol.close, pViewVol.distant);
                isOrtho = false;
            }//if
            else {//change to orthgraphic projection
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrtho(oViewVol.left, oViewVol.right, oViewVol.bottom,
                        oViewVol.top, oViewVol.close, oViewVol.distant);
                isOrtho = true;
            }//else
        }//if
        break;
    case GLUT_RIGHT_BUTTON:
        if (state == GLUT_DOWN) {
            ++shadingMode;
        }//if
        break;
    }//switch
    glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glRotated(rotation_x, 1, 0, 0);
	glRotated(rotation_y, 0, 1, 0);
    myDisplay();
	glPopMatrix();
}//myMouse

void myReshape(GLsizei w, GLsizei h) // Called by system when window is resized
{

    if ( (4/3)>(w/h) )                   // Keep aspect ratio = 4/3
        //glViewport(0, 0, 1.3333*h, h);
        glViewport(0, 0, w, (3/4)*w);
    //glViewport(0, 0, w, w);
    else
        //glViewport(0, 0, h, h);
        glViewport(0, 0, w, h);
}//myReshape

void drawobj( multimap<string,triangle>::iterator beg,
	multimap<string,triangle>::iterator end,
	GLfloat *diffuse = objDiffuse,
	GLfloat *ambient = objAmb,
	GLfloat *spec = objSpec,
	GLfloat *specExponent = objSpecExp,
	Shading s = GOURAUD )
{//by default, it uses GOURAUD shading
    //Front face properties
    glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
    glMaterialfv(GL_FRONT, GL_SHININESS, specExponent);
	//glEnable (GL_BLEND);
	//glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	multimap<string, triangle>::iterator curr;
    switch( s ) {
    case GOURAUD:
        for( curr = beg; curr != end; ++curr ) {
            // Draw all triangles/poly stored one by one
            glBegin(GL_POLYGON);// Begin polygon (triangle)
            //glBegin(GL_LINE_LOOP);// Begin polygon (triangle)

            //first vertex of the triangle
            glNormal3d( (curr->second).a.normal.x,
                        (curr->second).a.normal.y, (curr->second).a.normal.z );


            glVertex3d( (curr->second).a.where.x,
                        (curr->second).a.where.y, (curr->second).a.where.z);

            //second vertex of the triangle
            glNormal3d( (curr->second).b.normal.x,
                        (curr->second).b.normal.y, (curr->second).b.normal.z );
            glVertex3d( (curr->second).b.where.x,
                        (curr->second).b.where.y, (curr->second).b.where.z);

            //thrid and the last vertex of the triangle
            glNormal3d( (curr->second).c.normal.x,
                        (curr->second).c.normal.y, (curr->second).c.normal.z );
            glVertex3d( (curr->second).c.where.x,
                        (curr->second).c.where.y, (curr->second).c.where.z);

            glEnd();// End polygon
			/*
			vertex_counter = (char*)intToString(count).c_str();
			//glProject(
			//	(curr->second).a.where.x,
			//	(curr->second).a.where.y,
			//	(curr->second).a.where.z,
			DrawText( (int)((curr->second).a.where.x), (int)((curr->second).a.where.y),
				vertex_counter, 1, 1, 1 );
			count++;
			vertex_counter = (char*)intToString(count).c_str();
			DrawText( (int)((curr->second).b.where.x), (int)((curr->second).b.where.y),
				vertex_counter, 1, 1, 1 );
			count++;
			vertex_counter = (char*)intToString(count).c_str();
			DrawText( (int)((curr->second).c.where.x), (int)((curr->second).c.where.y),
				vertex_counter, 1, 1, 1 );
			count++;
			*/
        }//for
        break;
    case FLAT:
        for( curr = beg; curr != end; ++curr ) {
            // Draw all triangles/poly stored one by one
            glBegin(GL_POLYGON);// Begin polygon
            xyz ave_norm = (curr->second).triN();//normal of the triangle
            //this normal is used for all three vertices that form a triangle.
            glNormal3d(ave_norm.x, ave_norm.y, ave_norm.z);

            //first of the 3 vertices set.
            glVertex3d( (curr->second).a.where.x,
                        (curr->second).a.where.y, (curr->second).a.where.z);

            //second of the 3 vertices set.
            glVertex3d( (curr->second).b.where.x,
                        (curr->second).b.where.y, (curr->second).b.where.z);

            //thrid and the last of the 3 vertices set.
            glVertex3d( (curr->second).c.where.x,
                        (curr->second).c.where.y, (curr->second).c.where.z);

            glEnd();// End polygon
        }//for
        break;
    case FLAT2:
        for( curr = beg; curr != end; ++curr ) {
            // Draw all triangles/poly stored one by one
            glBegin(GL_POLYGON);// Begin polygon

            xyz ave_norm = curr->second.aveN();//normal of the triangle
            //this normal is used for all three vertices that form a triangle.
            glNormal3d(ave_norm.x, ave_norm.y, ave_norm.z);

            //first of the 3 vertices set.
            glVertex3d( (curr->second).a.where.x,
                        (curr->second).a.where.y, (curr->second).a.where.z);

            //second of the 3 vertices set.
            glVertex3d( (curr->second).b.where.x,
                        (curr->second).b.where.y, (curr->second).b.where.z);

            //thrid and the last of the 3 vertices set.
            glVertex3d( (curr->second).c.where.x,
                        (curr->second).c.where.y, (curr->second).c.where.z);
            glEnd();// End polygon
        }//for
        break;
    case MESH:
        for( curr = beg; curr != end; ++curr ) {
            // Draw all triangles/poly stored one by one
            glBegin(GL_LINE_LOOP);//draw mesh structure

            //first vertex of the triangle
            glNormal3d( (curr->second).a.normal.x,
                        (curr->second).a.normal.y, (curr->second).a.normal.z );
            glVertex3d( (curr->second).a.where.x,
                        (curr->second).a.where.y, (curr->second).a.where.z);

            //second vertex of the triangle
            glNormal3d( (curr->second).b.normal.x,
                        (curr->second).b.normal.y, (curr->second).b.normal.z );
            glVertex3d( (curr->second).b.where.x,
                        (curr->second).b.where.y, (curr->second).b.where.z);

            //thrid and the last vertex of the triangle
            glNormal3d( (curr->second).c.normal.x,
                        (curr->second).c.normal.y, (curr->second).c.normal.z );
            glVertex3d( (curr->second).c.where.x,
                        (curr->second).c.where.y, (curr->second).c.where.z );

            glEnd();// End polygon
        }//for
        break;

    }//switch
}//drawobj

void makeBezierPolygons( const string& cor, const string& pat, int resolution,
	multimap<string, triangle>& polygons) {
	//reads coordinate file and patch file and
	//returns properly formed polygons via parameter

	//===================== reading coordinate file =================
	ifstream fin;
    fin.open(cor.c_str());
    if( !fin ) {
        cout<<"-- file "<<cor<<" cannot be opened"<<endl;
        exit(1);
    }//if

    int index = 1;
    map<int, xyz> coordinates;
    double temp, temp_x, temp_y, temp_z;
    while( fin>>temp_x ) {
        fin>>temp_y;
        if( !fin ) {
            cout<<"-- file "<<cor<<" is not valid."<<endl;
            exit(1);
        }//if
        fin>>temp_z;
        if( !fin ) {
            cout<<"-- file "<<cor<<" is not valid."<<endl;
            exit(1);
        }//if
        coordinates.insert( pair<int, xyz>(index, xyz(temp_x, temp_y, temp_z)) );
        ++index;
    }//while
    //map<int, xyz>::iterator curr;

	fin.close();
	fin.clear();

	//=================== reading patch file ========================
    fin.open(pat.c_str());
    if( !fin ) {
        cout<<"-- file "<<pat<<" cannot be opened"<<endl;
        exit(1);
    }//if

	const int PATCH_SIZE = 16;
	int indices[PATCH_SIZE];
	unsigned matrix_size = (unsigned)floor( sqrt(PATCH_SIZE) );
	//vector< matrix<xyz> > p;//patches
	multimap< string, matrix<xyz> > p;//patches
	matrix<xyz> tmp(matrix_size, matrix_size);
	int row, col;
	string key;//key starts a group of patches
	while( fin>>key ) {//while there's a group of patches to read
		while( fin>>index ) {//while the group hasn't ended yet.
			//resetting the row and column for a matrix
			row = 0;
			col = 0;

			while( fin.peek() != '\n' ) {//one patch ends with \n
				tmp(row, col) = coordinates[index];//a cell is assigned
				col++;//move to next column of the matrix
				if( col == matrix_size ) {//should move to next row
					col = 0;
					++row;
				}//if
				fin>>index;//since it's not \n, read another index
				if( !fin ) {//critical error
					cout<<"-- file "<<pat<<" is not valid."<<endl;
					exit(1);
				}//if
			}//while
			tmp(row, col) = coordinates[index];//lastly fined index added

			//one patch is read.
			p.insert( pair< string, matrix<xyz> >(key,tmp) );
		}//while
		fin.clear();
	}//while
	fin.close();
	fin.clear();

	//============= make vertices =======================

	matrix<xyz> b(4,4);
	b(0,0)=xyz(-1,-1,-1); b(0,1)=xyz(3,3,3); b(0,2)=xyz(-3,-3,-3); b(0,3)=xyz(1,1,1);
	b(1,0)=xyz(3,3,3); b(1,1)=xyz(-6,-6,-6); b(1,2)=xyz(3,3,3); b(1,3)=xyz(0,0,0);
	b(2,0)=xyz(-3,-3,-3); b(2,1)=xyz(3,3,3); b(2,2)=xyz(0,0,0); b(2,3)=xyz(0,0,0);
	b(3,0)=xyz(1,1,1); b(3,1)=xyz(0,0,0); b(3,2)=xyz(0,0,0); b(3,3)=xyz(0,0,0);

	matrix<xyz> u(1,4);

	matrix<xyz> w(4,1);

	point temp_cor;
	map<int, point> vertices;//stores vertices
	multimap< string, matrix<xyz> >::iterator curr;
	int vert_count = 1;//key to vertices map.
	double unit = 1.0/resolution;//so that loop iterates resolution times

	for( curr = p.begin(); curr != p.end(); ++curr ) {//for each patch
		for( double t_u = 0.0; t_u <= 1.0; t_u += unit ) {
			for( double t_w = 0.0; t_w <= 1.0; t_w += unit ) {
				double u3, u2;
				u3 = t_u*t_u*t_u;
				u2 = t_u*t_u;
				u(0,0) = xyz(u3, u3, u3);
				u(0,1) = xyz(u2, u2, u2);
				u(0,2) = xyz(t_u, t_u, t_u);
				u(0,3) = xyz(1.0, 1.0, 1.0);

				u3 = t_w*t_w*t_w;
				u2 = t_w*t_w;
				w(0,0) = xyz(u3, u3, u3);
				w(1,0) = xyz(u2, u2, u2);
				w(2,0) = xyz(t_w, t_w, t_w);
				w(3,0) = xyz(1.0, 1.0, 1.0);

				xyz loc = (u*b*(curr->second)*b*w)(0,0);

				u3 = 3*t_u*t_u;
				u2 = 2*t_u;
				u(0,0) = xyz(u3, u3, u3);
				u(0,1) = xyz(u2, u2, u2);
				u(0,2) = xyz(1.0, 1.0, 1.0);
				u(0,3) = xyz(0.0, 0.0, 0.0);
				xyz u_tan = (u*b*(curr->second)*b*w)(0,0);

				u3 = t_u*t_u*t_u;
				u2 = t_u*t_u;
				u(0,0) = xyz(u3, u3, u3);
				u(0,1) = xyz(u2, u2, u2);
				u(0,2) = xyz(t_u, t_u, t_u);
				u(0,3) = xyz(1.0, 1.0, 1.0);

				u3 = 3*t_w*t_w;
				u2 = 2*t_w;
				w(0,0) = xyz(u3, u3, u3);
				w(1,0) = xyz(u2, u2, u2);
				w(2,0) = xyz(1.0, 1.0, 1.0);
				w(3,0) = xyz(0.0, 0.0, 0.0);
				xyz w_tan = (u*b*(curr->second)*b*w)(0,0);

				xyz norm = xyz::crossProduct(w_tan, u_tan);
				vertices.insert( pair<int,point>(vert_count, point(loc, norm)) );
				++vert_count;
			}//for
		}//for

		//make triangles
		for(unsigned i=1; i<(vertices.size()-resolution-1); ++i) {
			//outer vertices _| ignored
			if(i%(resolution+1) != 0) {
				//valid vertice for triangle formulation

				if( (vertices[i]).normal == xyz(0,0,0) || vertices[i].where == xyz(0,0,3.15) ) {
					//cout<<vertices[i].where<<" "<<vertices[i].normal<<endl;
					(vertices[i]).normal = -xyz::crossProduct(
						(vertices[i+resolution+1]).where - (vertices[i]).where,
						(vertices[i+resolution+2]).where - (vertices[i]).where );
				}//if

				if( !(vertices[i].where==vertices[i+2].where
					|| vertices[i+resolution+1].where ==
						vertices[i+resolution+2].where) ) {
					//bottom vertex is not same as its adjacent vertex
					//(to the right)
					//And, this vertex is not same as adjacent vertex
					//(to the right)
					//two triangles pushed.

					//triangle: this, east, southeast vertices.
					polygons.insert(
						pair<string, triangle>(curr->first,
							triangle(vertices[i], vertices[i+1],
								vertices[i+resolution+2])) );

					//triangle: this, southeast, south vertices.
					polygons.insert(
						pair<string, triangle>(curr->first,
							triangle(vertices[i], vertices[i+resolution+2],
								vertices[i+resolution+1])) );
				}//if
				else {
					//bottom vertex and adjacent to the bottom vertex
					//(to the right) are the same. Or,
					//this vertex and adjacent to this vertex
					//(to the right) are the same.
					//only one triangle pushed.
					/*
cout<<"2nd : "<<vertices[i].where<<" "<<vertices[i].normal<<endl;
					(vertices[i]).normal = -xyz::crossProduct(
						(vertices[i+resolution+1]).where - (vertices[i]).where,
						(vertices[i+resolution+2]).where - (vertices[i]).where );
*/
					polygons.insert(
						pair<string,triangle>(curr->first,
							triangle(vertices[i], vertices[i+resolution+2],
								vertices[i+resolution+1])) );
				}//else
			}//if
		}//for
		vertices.clear();

		vert_count = 1;//another patch
	}//for

}//makeBezierPolygons

#endif

