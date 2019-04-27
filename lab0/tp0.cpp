/* -------------------------------------------------------- */
/* -------------------------------------------------------- */
/*

    TP OpenGL - Graphics and Sound Synthesis course

George Drettakis, Nicolas Tsingos, Sylvain Lefebvre

mailto: FirstName.LastName@inria.fr

REVES / INRIA

History:
- Created 2006-10-10 (SL)

*/
/* -------------------------------------------------------- */
/*

TP0 - Description

This first TP is dedicated to glut and basic OpenGL operations.

* Documentation:
---------------

You *will* need to access the documentation.
- OpenGL reference  http://www.mevis.de/opengl/opengl.html (more online => use google)
- glut   reference  http://www.opengl.org/documentation/specs/glut/spec3/spec3.html

* Reference:
------------
- OpenGL web site       http://www.opengl.org
- OpenGL specifications http://www.opengl.org/documentation/specs/


Exercises guidelines:
---------------------

=> For each exercise, markers have been placed into the file.
   For instance, markers [1A] and [1B] mark locations of interest
   for exercise 1.
=> As a generic rule, *do not delete anything*: use '/*' comments
   to deactivate parts of the code.
=> Anytime you wonder what the parameters to a function mean or should
   be: *use the documentation*.

Exercices:
----------

 Ex1) (markers [1A] and [1B])

   - Change the color background when pressing the following keys:
     '1' => red; '2' => green; '3' => blue

 Ex2) (marker [2A])
   - Invert two vertices of the triangle. What is happening and why ?
     * Answer: Nothing appears. The vertices are ordered in the reverse order therefore the face is considered facing back.
   - Correct the problem without changing the order of the vertices.

 Ex3) (marker [3A])
   - Display a triangle with red on topmost vertex, green on bottom left
     and blue on bottom right.

 Ex4) (markers [4A],[4B] and [4C])
   - Comment line [4A], uncomment lines [4B] and [4C]
   - Change line [4C] so that the entire triangle is visible on screen

 Ex5)
   - Comment out the triangle drawing
   - Uncomment block [5A]
   - test the program
   - Make sure the torus is entirely visible and approximatively centered on screen
   - Show a wireframe torus using glPolygonMode (see OpenGL documentation)
     (do *not* use glutWireTorus)
   - Use the 'w' key to switch the wireframe mode on/off
   - Make the torus smoother using the two latest parameters of glutSolidTorus 
     (see glut documentation!)

 Ex6)
   - Modify the code so that we can adjust the view distance from the torus 
     using the '+' and '-' keys.

 Ex7)
   - Make the torus automatically rotate around the y axis
   - Make the torus move back and forth on the x axis between [-1,1], 
     while still rotating on itself along the y axis.

 Ex8)
   - Modify the code so that we can adjust the view distance from the torus
     using the mouse left button + movement.

 Ex9)
   - Replace the torus by a teapot.

*/
/* -------------------------------------------------------- */

#include <windows.h>        // windows API header (required by gl.h)

#include <GL/gl.h>          // OpenGL header
#include <GL/glu.h>         // OpenGL Utilities header
#include <GL/glut.h>        // OpenGL Utility Toolkit header

#include <cstdio>
#include <cmath>

/* -------------------------------------------------------- */

int          g_MainWindow;          // glut Window Id
int          g_W=640;               // window width
int          g_H=480;               // window width

GLenum       polygonMode = GL_FILL; // polygon mode currently in use
GLdouble     z = 5.0;               // z-position of the eye 
GLdouble	 angle = 0.0;           // angular position of the object
GLdouble     zoomStartX = 0.0;      // x position of the cursor when current zoom starts
GLdouble     zoomStartY = 0.0;      // y position of the cursor when current zoom starts
GLdouble     zoomDirX = 0.0;        // x coordinate of current zoom initial direction
GLdouble     zoomDirY = 0.0;        // y coordinate of current zoom initial direction
GLdouble     zoomSpeed = 0.0;       // speed of current zoom
bool         zooming = false;       // zoom is currently occuring


/* -------------------------------------------------------- */

void mainKeyboard(unsigned char key, int x, int y) 
{
	if (key == 'q') {
		exit (0);
	} else if (key == ' ') {
		printf("spacebar pressed\n");
	} else if (key == '1') {
		glClearColor(1.0, 0.0, 0.0, 1.0);
	} else if (key == '2') {
		glClearColor(0, 1.0, 0.0, 1.0);
	} else if (key == '3') {
		glClearColor(0.0, 0.0, 1.0, 1.0);
	} else if (key == 'w') {
		polygonMode = polygonMode == GL_LINE ? GL_FILL : GL_LINE;
	} else if (key == '+') {
		z += 0.1;
	} else if (key == '-') {
		z -= 0.1;
	}
	printf("key '%c' pressed\n",key);
}

/* -------------------------------------------------------- */

void mainMouse(int btn, int state, int x, int y) 
{
	if (state == GLUT_DOWN) {
		if (btn == GLUT_LEFT_BUTTON) {
			printf("Left mouse button pressed at coordinates %d,%d\n",x,y);
		} else if (btn == GLUT_RIGHT_BUTTON) {
			printf("Right mouse button pressed at coordinates %d,%d\n",x,y);
		}

		// A zoom occurs when a button is pressed

		zooming = true;	 
		zoomStartX = x;  
		zoomStartY = y;
		
	} else if (state == GLUT_UP) {
		if        (btn == GLUT_LEFT_BUTTON) {
			printf("Left mouse button release at coordinates %d,%d\n",x,y);
		} else if (btn == GLUT_RIGHT_BUTTON) {
			printf("Right mouse button release at coordinates %d,%d\n",x,y);
		}

		// The current zoom ends when a button is released

		zooming = false;
		zoomDirX = 0.0;
		zoomDirY = 0.0;
		zoomSpeed = 0.0;

	}
}

/* -------------------------------------------------------- */

void mainMotion(int x,int y)
{
	printf("Mouse is at %d,%d\n",x,y);

	if (zooming)
	{
		// If current zoom initial direction is to small, redefine it as the displacement of the mouse since the begining of current zoom
		if (((zoomDirX * zoomDirX) + (zoomDirY * zoomDirY)) < 0.1) 
		{
			zoomDirX = x - zoomStartX;
			zoomDirY = y - zoomStartY;
		}
		// Otherwise, the zoom speed is proportional to the projection of the displacement on the initial direction
		else
		{
			zoomSpeed = (((x - zoomStartX) * zoomDirX) + ((y - zoomStartY) * zoomDirY)) / sqrt(zoomDirX * zoomDirX + zoomDirY * zoomDirY);	
		}
	}
}

/* -------------------------------------------------------- */

void mainReshape(int w,int h)
{
	printf("Resizing window to %d,%d\n",w,h);
	g_W=w;
	g_H=h;
	// set viewport to the entire window
	glViewport(0,0,g_W,g_H);
}


/* -------------------------------------------------------- */

void mainRender()
{
	/// clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/// setup projection
	// set matrix mode
	glMatrixMode(GL_PROJECTION);
	// load identity
	glLoadIdentity();
	// ask glu for a 2d orthogonal projection
	//gluOrtho2D(0.0,1.0,0.0,1.0);                    // [4A]
	gluPerspective(45,g_W/float(g_H),0.01,100.0); // [4B]

	/// setup modelview
	// set matrix mode
	glMatrixMode(GL_MODELVIEW);
	// load identity
	glLoadIdentity();

	z = max(0, z + 0.001 * zoomSpeed); 
	gluLookAt(1.0,1.0, z, 0.0,0.0,0, 0,0,1);     // [4C]
	glRotated(angle, 0.0, 1.0, 0.0);
	glTranslated(cos(0.01 * angle), 0.0, 0.0);
	angle += 0.1;

	/// draw a white triangle
	// begin
	//glBegin(GL_TRIANGLES); // [2A] [3A]

	//glColor3f(0.0, 0.0, 1.0);

	//glVertex3f(1.0,0.0,0.0);

	//glColor3f(0.0, 1.0, 0.0);

	//glVertex3f(0.0,0.0,0.0);

	////glVertex3f(1.0,0.0,0.0);

	//glColor3f(1.0, 0.0, 0.0);

	//glVertex3f(0.5,1.0,0.0);

	//glEnd();


	 // block [5A] (For exercise 5)
	//
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
	/// draw a torus
	// enable lighting to see the surface curvature


	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	// ask glut to draw a torus - glut proposes several simple shapes (sphere, cones, ...)
	//int k = 3;
	//glutSolidTorus(0.25f,0.75f, k * 32, k * 16);
	glutSolidTeapot	(1);

	

	// swap - this call exchanges the back and front buffer
	// swap is synchronized on the screen vertical sync
	glutSwapBuffers();
}

/* -------------------------------------------------------- */

void idle( void )
{
	// whenever the application has free time, ask for a screen refresh
	glutPostRedisplay();
}

/* -------------------------------------------------------- */

void main(int argc, char **argv) 
{
	///
	///  glut
	///
	// main glut init
	glutInit(&argc, argv);
	// initial window size
	glutInitWindowSize(g_W,g_H); 
	// init display mode
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	// create main window
	g_MainWindow=glutCreateWindow("TP0");
	// set main window as current window
	glutSetWindow(g_MainWindow);
	/// setup glut callbacks
	// mouse (whenever a button is pressed)
	glutMouseFunc(mainMouse);
	// motion (whenever the mouse is moved *while* a button is down)
	glutMotionFunc(mainMotion);
	// keyboard (whenever a character key is pressed)
	glutKeyboardFunc(mainKeyboard);
	// display  (whenerver the screen needs to be painted)
	glutDisplayFunc(mainRender);
	// reshape (whenever the window size changes)
	glutReshapeFunc(mainReshape);
	// idle (whenever the application as some free time)
	glutIdleFunc(idle);

	///
	/// OpenGL
	///
	/// NOTE: OpenGL calls are not valid if an OpenGL
	///       context has not been created.
	///       Here the OpenGL context has been created
	///       by glut when calling glutCreateWindow
	// setup clear screen color (ie. background color)
	glClearColor(0.0, 0.0, 0.0, 1.0); // [1B]
	// enable z-buffer
	glEnable(GL_DEPTH_TEST);
	// disable lighting
	glDisable(GL_LIGHTING);
	// enable back face culling
	//glEnable(GL_CULL_FACE);
	// set default color to white
	glColor3f(1,1,1);

	// print a small documentation
	printf("[q]     - quit\n");

	// enter glut main loop - this *never* returns
	glutMainLoop();
}

/* -------------------------------------------------------- */

