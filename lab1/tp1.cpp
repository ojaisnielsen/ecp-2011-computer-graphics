
/* -------------------------------------------------------- */
/* -------------------------------------------------------- */
/*

TP OpenGL - Computer Graphics course

George Drettakis, Bruno Lévy, Sylvain Lefebvre

mailto: FirstName.LastName@inria.fr

REVES - ALICE / INRIA

History:
- Created  2006-04-11 (SL)
- Modified 2010-04-11 (SL)

*/
/* -------------------------------------------------------- */
/*

TP1 - Description

This TP introduces materials, Indexed Face Sets, and textures

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

Ex0)

  Launch the application, move the viewpoint with left/right mouse button.
  Read through the code.

Ex1) 

  - Change the object material when the following keys are pressed:
    '1' - gold (yellow + yellow specular)
    '2' - red plastic (red + white specular)

Ex2)

  - When pressing '6', use a green light on a red material. What is happening ?
    NOTE: Restore a white light color for other materials!

Ex3)

  - When pressing '7', enable three lights: one red, one green, one blue, each
    being a directional light along a major direction (respectively x,y, and z).
    Use a white material for the object.
    NOTE: Keep only one light for other keys ('0' to '6')

Ex4) 

  The 'loadIFS' function loads an Indexed Face Set in the following variables:

    unsigned int    g_NumVerticies  = 0;      // number of verticies
    t_point        *g_Verticies     = NULL;   // verticies
    unsigned int    g_NumIndices    = 0;      // number of indices (= 3 * number of triangles)
    unsigned short *g_Indices       = NULL;   // indices, 3 per triangles

  - Uncomment line [4A]
  - Uncomment code to draw the IFS (marker [4B]).
    Make sure per-vertex normals and texture coordinates are sent to OpenGL.
    NOTE: Currently only the vertex positions are sent!
  - Use spacebar to switch between torus and IFS

Ex5) 

  - Compute mesh min/max coordinates on x,y,z.
  - Center the object and make sure it is correctly oriented.

Ex6)

 Function 'createCheckerBoardTexture' generate a checkerboard texture (it returns the texture OpenGL id)

 - Uncomment block [7A], test the program, notice the quad at the bottom left corner
 - What is the purpose of glPushAttrib(GL_ENABLE_BIT) ? (marker [7B])
 - Apply a checkboard texture to the quad
 - Apply a checkboard texture to the mesh
 - Change 'createCheckerBoardTexture' to replace the black part by the i,j coordinates within the texture

Ex7)

 - Draw a mosaic of 2x2 times the texture on the on-screen quad by multiplying
   the texture coordinates by 2.

Ex8)

 - Use key 'l' to enable/disable texture filtering (bi-linear interpolation and MIP-mapping)
   WARNING: make sure the texture has valid MIP-map levels! Update the code where necessary.

Ex9)

 - Create a new function 'createCirclesTexture' generating a 256x256 texture with 16 
   concentric circles of alterning green/blue color (centered on the middle of the texture). 
 - Use key 's' to switch between this new texture and the checkerboard.

*/
/* -------------------------------------------------------- */

#include <windows.h>        // windows API header (required by gl.h)

#include <GL/gl.h>          // OpenGL header
#include <GL/glu.h>         // OpenGL Utilities header
#include <GL/glut.h>        // OpenGL Utility Toolkit header

#include <cstdio>
#include <cmath>
#include <iostream>

using namespace std;

/* -------------------------------------------------------- */

int          g_MainWindow; // glut Window Id
int          g_W=640;      // window width
int          g_H=480;      // window width

bool         g_WireframeMode      = false; // enable/disable wireframe
float        g_Zoom               = 1.0f;  // zoom factor
float        g_Rotate             = 0.0f;  // rotation
bool         g_LeftButtonPressed  = false; // is left button pressed ?
bool         g_RightButtonPressed = false; // is right button pressed ?
int          g_MouseX             = 0;     // previous mouse coord in x
int          g_MouseY             = 0;     // previous mouse coord in y

GLfloat g_material_ambient[]   = { 0, 0, 0, 1 }; 
GLfloat g_material_diffuse[]   = { 1, 1, 1, 1 }; 
GLfloat g_material_specular[]  = { 1, 1, 1, 1 }; 
GLfloat g_material_shininess[] = { 100 };
GLfloat g_light[]  = { 1, 1, 1, 0}; 
bool g_threeLightsOn = false;
GLfloat g_threeLightsPositions[][4]  = {{ 1, 0, 0, 0 }, { 0, 1, 0, 0 }, { 0, 0, 1, 0 }}; 
GLfloat g_threeLightsColors[][4]  = {{ 1, 0, 0, 0}, { 0, 1, 0, 0}, { 0, 0, 1, 0}};
int g_objectId = 0;
float g_objectMinCoords[][3] = {{0, 0, 0}, {0, 0, 0}};
float g_objectMaxCoords[][3] = {{0, 0, 0}, {0, 0, 0}};
float g_center[] = {0, 0, 0};
GLuint g_tex;


/* -------------------------------------------------------- */

typedef struct s_point
{
  float p[3];
  float n[3];
  float uv[2];
} t_point;

t_point        *g_Verticies     = NULL;
unsigned short *g_Indices       = NULL;
unsigned int    g_NumVerticies  = 0;
unsigned int    g_NumIndices    = 0;

/* -------------------------------------------------------- */

void getMinMaxCoords(t_point *verticies, int nVerticies, float *minCoord, float *maxCoord)
 {

	for (int j = 0; j < 3; j++)
	{
		 minCoord[j] = verticies[0].p[j];
	     maxCoord[j] = verticies[0].p[j];
		 for (int i = 0; i < nVerticies; i++)
		 {
				minCoord[j] = min(verticies[i].p[j], minCoord[j]);
				maxCoord[j] = max(verticies[i].p[j], maxCoord[j]);		 
		 }
	}
 }

void mainKeyboard(unsigned char key, int x, int y) 
{
  if (key == 'q') {
    exit (0);
  } 
  if (key == 'w') {
    g_WireframeMode=!g_WireframeMode;
  } 
  if(key == '1') {
	  g_material_diffuse[0] = g_material_diffuse[1] = 1;
	  g_material_diffuse[2] = 0;
	  g_material_specular[0] = g_material_specular[1] = 1;
	  g_material_specular[2] = 0;
  } 
  if (key == '2') {
	  g_material_diffuse[0] = 1;
	  g_material_diffuse[1] = g_material_diffuse[2] = 0;
	  g_material_specular[0] = g_material_specular[1] = g_material_specular[2] = 1;
  }
  if (key == '6') {

	  if (g_material_diffuse[0] == 1 && g_material_diffuse[1] == 0 && g_material_diffuse[2] == 0)
	  {
		  g_light[0] = g_light[2] = 0;
		  g_light[1] = 1;
	  }
	  else
	  {
		  g_light[0] = g_light[2] = g_light[1] = 1;
	  }
  }
  if (key == '7')
  {
	  g_threeLightsOn = true;
	  g_material_diffuse[0] = g_material_diffuse[1] = g_material_diffuse[2] = 1;
	  g_material_specular[0] = g_material_specular[1] = g_material_specular[2] = 1;
  }
  if (key == '0' || key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6')
  {
	   g_threeLightsOn = false;
  }
  if (key == ' ')
  {
	   g_objectId = 1 - g_objectId;
	   g_center[0] = (g_objectMinCoords[g_objectId][0] + g_objectMaxCoords[g_objectId][0]) / 2;
	   g_center[1] = (g_objectMinCoords[g_objectId][1] + g_objectMaxCoords[g_objectId][1]) / 2;
	   g_center[2] = (g_objectMinCoords[g_objectId][2] + g_objectMaxCoords[g_objectId][2]) / 2;
  }

  printf("key '%c' pressed\n",key);
}

/* -------------------------------------------------------- */

void mainMouse(int btn, int state, int x, int y) 
{
  if (state == GLUT_DOWN) {
    if (btn == GLUT_LEFT_BUTTON) {
      printf("Left mouse button pressed at coordinates %d,%d\n",x,y);
      g_LeftButtonPressed  = true;
      g_MouseX = x;
      g_MouseY = y;
    } else if (btn == GLUT_RIGHT_BUTTON) {
      printf("Right mouse button pressed at coordinates %d,%d\n",x,y);
      g_RightButtonPressed = true;
      g_MouseX = x;
      g_MouseY = y;
    }
  } else if (state == GLUT_UP) {
    if        (btn == GLUT_LEFT_BUTTON) {
      printf("Left mouse button release at coordinates %d,%d\n",x,y);
      g_LeftButtonPressed  = false;
    } else if (btn == GLUT_RIGHT_BUTTON) {
      printf("Right mouse button release at coordinates %d,%d\n",x,y);
      g_RightButtonPressed = false;
    }
  }
}

/* -------------------------------------------------------- */

void mainMotion(int x,int y)
{
  printf("Mouse is at %d,%d\n",x,y);
  
  if (g_LeftButtonPressed) {
  
    int deltax = x - g_MouseX;
    int deltay = y - g_MouseY;
    if (abs(deltax) > abs(deltay)) {
      g_Zoom += deltax * 0.01;
    } else {
      g_Zoom += deltay * 0.01;
    }
    g_MouseX   = x;
    g_MouseY   = y;

  } else if (g_RightButtonPressed) {

    int deltax = x - g_MouseX;
    int deltay = y - g_MouseY;
    if (abs(deltax) > abs(deltay)) {
      g_Rotate += deltax * 0.01;
    } else {
      g_Rotate += deltay * 0.01;
    }
    g_MouseX   = x;
    g_MouseY   = y;

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
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHT3);

	if (g_threeLightsOn)
	{
		glDisable(GL_LIGHT0);

		for (int i = 0; i < 3; i++)
		{
			glLightfv(GL_LIGHT1 + i, GL_SPECULAR, g_threeLightsColors[i]);
			glLightfv(GL_LIGHT1 + i, GL_DIFFUSE, g_threeLightsColors[i]);
			glLightfv(GL_LIGHT1 + i, GL_POSITION, g_threeLightsPositions[i]);
		}
	}
	else
	{	
		  glDisable(GL_LIGHT1);
		  glDisable(GL_LIGHT2);
		  glDisable(GL_LIGHT3);

		  glLightfv(GL_LIGHT0, GL_SPECULAR, g_light);
		  glLightfv(GL_LIGHT0, GL_DIFFUSE, g_light);
		  glLightfv(GL_LIGHT0, GL_AMBIENT, g_light);
	}

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   g_material_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   g_material_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  g_material_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, g_material_shininess); 
	glEnable(GL_NORMALIZE);

  /// clear screen
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /// setup projection
  // set matrix mode
  glMatrixMode(GL_PROJECTION);
  // load identity
  glLoadIdentity();
  // ask glu for a 2d orthogonal projection
  gluPerspective(45,g_W/float(g_H),0.01,100.0);

  /// setup modelview
  // set matrix mode
  glMatrixMode(GL_MODELVIEW);
  // load identity
  glLoadIdentity();
  // setup camera

  gluLookAt(g_Zoom*2.5,g_Zoom*2.5,0.5f, g_center[0],g_center[1],g_center[2], 0,0,1);
  // rotate object around z axis (up)
  glRotated(g_Rotate*20.0f,0,0,1);

  // wireframe mode
  if (g_WireframeMode) {
    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
  } else {
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
  }

  if (g_objectId == 0)
  {
	  // draw object
	  glutSolidTorus(0.2f,1.0f,30,30);
  }
  else if (g_objectId == 1)
  {

	  // [4B]
	  // Draw IFS
      glBindTexture (GL_TEXTURE_2D, g_tex);
	  glBegin(GL_TRIANGLES);
	  for (int t = 0 ; t < g_NumIndices/3 ; t ++ ) {
		for (int i = 0 ; i < 3 ; i ++ ) {
		  glVertex3fv( g_Verticies[ g_Indices[t*3 + i] ].p );
		  glTexCoord2fv(g_Verticies[ g_Indices[t*3 + i] ].uv);
		  glNormal3fv(g_Verticies[ g_Indices[t*3 + i] ].n);
		}
	  }
	  glEnd();
  }
  


  // [7A] draw an on-screen quad in the top left corner
 

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0,1,0,g_H/float(g_W));
  glMatrixMode(GL_MODELVIEW);
  glPushAttrib(GL_ENABLE_BIT); // [7B]
  glDisable(GL_LIGHTING);
  glDisable(GL_DEPTH_TEST);
  glColor3f(1,1,1);
  glLoadIdentity();
  glScaled(0.2,0.2,1);
  glBegin(GL_QUADS);
  glVertex2i(0,0);
  glVertex2i(1,0);
  glVertex2i(1,1);
  glVertex2i(0,1);
  glEnd();
  glPopAttrib();


  // swap
  glutSwapBuffers();
}

/* -------------------------------------------------------- */

void idle( void )
{
  // whenever the application has free time, ask for a screen refresh
  glutPostRedisplay();
}

/* -------------------------------------------------------- */

// Load an IFS
void loadIFS(const char *filename)
{
  // open file
  FILE *f=fopen(filename,"rb");
  if (f == NULL) {
    // error?
    cerr << "[loadIFS] Cannot load " << filename << endl;
  }
  // read vertices
  fread(&g_NumVerticies,sizeof(unsigned int),1,f);
  g_Verticies = new t_point[g_NumVerticies];
  fread(g_Verticies,sizeof(t_point),g_NumVerticies,f);
  // read indices
  fread(&g_NumIndices,sizeof(unsigned int),1,f);
  g_Indices = new unsigned short[g_NumIndices];
  fread(g_Indices,sizeof(unsigned short),g_NumIndices,f);
  // close file
  fclose(f);
  // print mesh info
  cerr << g_NumVerticies << " points " << g_NumIndices/3 << " triangles" << endl;
  // done.

  getMinMaxCoords(g_Verticies, g_NumVerticies, g_objectMinCoords[1], g_objectMaxCoords[1]);
}

/* -------------------------------------------------------- */

GLuint createCheckerBoardTexture()
{
  static int res=16;
  GLuint id=0;
  glGenTextures(1,&id);
  glBindTexture(GL_TEXTURE_2D,id);
  unsigned char *data=new unsigned char[res*res*4];
  for (int j=0;j<res;j++) {
    for (int i=0;i<res;i++) {
      data[(i+j*res)*4+0] = ((i+j)&1) ? 255 : 0;
      data[(i+j*res)*4+1] = ((i+j)&1) ? 255 : 0;
      data[(i+j*res)*4+2] = ((i+j)&1) ? 255 : 0;
      data[(i+j*res)*4+3] = 255;
    }
  }
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,res,res,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
  delete [](data);
  return (id);
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
  g_MainWindow=glutCreateWindow("TP1");
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
  glClearColor(0.0, 0.0, 0.0, 1.0);
  // enable z-buffer
  glEnable(GL_DEPTH_TEST);
  // enable lighting
  glEnable(GL_LIGHTING);
  //glEnable(GL_LIGHT0);
  // enable back face culling
  glEnable(GL_CULL_FACE);
  // set default color to white
  glColor3f(1,1,1);
  // set default material

  //glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   material_ambient);
  //glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   material_diffuse);
  //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  material_specular);
  //glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, material_shininess); 
  //glEnable(GL_NORMALIZE);

  g_tex = createCheckerBoardTexture();
  
  /// Load IFS mesh
   loadIFS("test.mesh"); // [4A]

  // print a small documentation
  printf("[q]     - quit\n");

  // enter glut main loop - this *never* returns
  glutMainLoop();
}

/* -------------------------------------------------------- */

