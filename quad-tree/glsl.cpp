/* -------------------------------------------------------- */
/* -------------------------------------------------------- */
/*

  TP OpenGL - Computer Graphics course

Sylvain Lefebvre - INRIA

mailto: FirstName.LastName@inria.fr

History:
- Created 2006-10-10 (SL)
- Edited   2010-12-01 (SL)

*/
/* -------------------------------------------------------- */
/*

Helper functions for GLSL

*/
/* -------------------------------------------------------- */

#include <windows.h>        // windows API header (required by gl.h)

#include <GL/gl.h>          // OpenGL header
#include <GL/glu.h>         // OpenGL Utilities header
#include <GL/glut.h>        // OpenGL Utility Toolkit header
#include <glux.h>           // OpenGL eXtension loading

// load up OpenGL extensions for GLSL
#include "GL_ARB_shader_objects.h"
GLUX_REQUIRE(GL_ARB_shader_objects);
#include "GL_ARB_vertex_shader.h"
GLUX_REQUIRE(GL_ARB_vertex_shader);
#include "GL_ARB_fragment_shader.h"
GLUX_REQUIRE(GL_ARB_fragment_shader);
#include "GL_ARB_vertex_program.h"
GLUX_REQUIRE(GL_ARB_vertex_program);
#include "GL_ARB_multitexture.h"
GLUX_REQUIRE(GL_ARB_multitexture);

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
GLuint loadGLSLShader(const char *prg,GLuint type)
{
  GLuint id;
  // create shader object
  id = glCreateShaderObjectARB(type);
  // setup string as source
  glShaderSourceARB(id,1,&prg,NULL);
  // compile the shader
  glCompileShaderARB(id);
  // check for errors
  GLint compiled;
  glGetObjectParameterivARB(id,GL_OBJECT_COMPILE_STATUS_ARB, &compiled);
  if (!compiled) {
    // there was an error => display
    GLint maxLength;
    glGetObjectParameterivARB(id,
			     GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
    GLcharARB *infoLog=new GLcharARB[maxLength+1];
    glGetInfoLogARB(id, maxLength, NULL, infoLog);
    fprintf(stderr,"\n\n**** GLSL shader failed to compile ****\n%s",infoLog);
	delete [](infoLog);
	// exit on error
	exit (0);
  }
  // done, return shader id
  return (id);
}

/* -------------------------------------------------------- */

/*
  createGLSLProgram

  Create a GLSL program and load shaders.

  * Inputs
    - vp_code   : null terminated string for vertex program
    - fp_code   : null terminated string for fragment program
  * Output
    - program GL id

*/
GLuint createGLSLProgram(const char *vp_code,const char *fp_code)
{
  GLuint vp,fp;
  // create program
  GLuint id = glCreateProgramObjectARB();
  // if vertex program code is given, compile it
  if (vp_code) {
    vp=loadGLSLShader(vp_code,GL_VERTEX_SHADER);
    glAttachObjectARB(id,vp);
  }
  // if fragment program code is given, compile it
  if (fp_code) {
    fp=loadGLSLShader(fp_code,GL_FRAGMENT_SHADER);
    glAttachObjectARB(id,fp);
  }
  // link compiled shaders
  glLinkProgramARB(id);
  // check link success
  GLint linked;
  glGetObjectParameterivARB(id,GL_OBJECT_LINK_STATUS_ARB, &linked);
  if (!linked) {
	// there was an error => display
    GLint maxLength;
    glGetObjectParameterivARB(id,GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
    GLcharARB *infoLog=new GLcharARB[maxLength+1];
    glGetInfoLogARB(id, maxLength, NULL, infoLog);
    fprintf(stderr,"\n\n**** GLSL program failed to link ****\n%s",infoLog);
	delete [](infoLog);
	// exit on error
	exit (0);
  }
  // free compiled shaders (they are now embbeded into the program)
  if (vp_code) {
    glDeleteObjectARB(vp);
  }
  if (fp_code) {
    glDeleteObjectARB(fp);
  }
  // unset
  glUseProgramObjectARB(0);
  // done, return program id
  return (id);
}

/* -------------------------------------------------------- */

/*
  Load a string from a file

  Returned string must be deleted with delete[]()
*/
const char *loadStringFromFile(const char *fname)
{
  // open file
  FILE *f=fopen(fname,"rb");
  if (f == NULL) {
	  fprintf(stderr,"[ERROR] Cannot open file '%s'\n",fname);
	  exit (-1);
  }
  // get file size
  fseek(f,0,SEEK_END);    // goto end
  long fsize = ftell(f);  // get size
  fseek(f,0,SEEK_SET);    // goto begining
  // allocate string
  char *str=new char[fsize+1];
  // read
  fread(str,fsize,sizeof(char),f);
  str[fsize]='\0';
  // close file
  fclose(f);
  // return string
  return (str);
}

/* -------------------------------------------------------- */
