/* -------------------------------------------------------- */
/* -------------------------------------------------------- */
/*

  TP OpenGL - Computer Graphics course

Sylvain Lefebvre - INRIA

mailto: FirstName.LastName@inria.fr

History:
- Created 2006-10-10 (SL)

*/
/* -------------------------------------------------------- */
/*

Helper functions for GLSL - header

*/
/* -------------------------------------------------------- */

#include <windows.h>        // windows API header (required by gl.h)

#include <GL/gl.h>          // OpenGL header
#include <GL/glu.h>         // OpenGL Utilities header
#include <GL/glut.h>        // OpenGL Utility Toolkit header

/* -------------------------------------------------------- */

/*
  loadGLSLShader

  Loads a GLSL shader from a string.

  * Inputs
    - prg  : null terminated string
	- type : program type (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)
  * Output
    - program GL id

*/
GLuint loadGLSLShader(const char *prg,GLuint type);

/*
  createGLSLProgram

  Create a GLSL program and load shaders.

  * Inputs
    - vp_code   : null terminated string for vertex program
    - fp_code   : null terminated string for fragment program
  * Output
    - program GL id

*/
GLuint createGLSLProgram(const char *vp_code,const char *fp_code);

/*
  Load a string from a file

  Returned string must be deleted with delete[]()
*/
const char *loadStringFromFile(const char *fname);
