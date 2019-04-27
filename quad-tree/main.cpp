/*! \file */

#include "stdafx.h" 
#include <GL/gl.h>     
#include <GL/glu.h>   
#include <GL/glut.h>
#include <glux.h>
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

#include "glsl.h"
#include "QuadTree.h"

unsigned int g_task = 0;
int g_mainWindow;
int g_mainWindowWidth = 640;
int g_mainWindowHeight = 480;
QuadTree *g_tree;
GLuint g_texture;
unsigned int g_textureWidth = 0;
unsigned int g_textureHeight = 0;
unsigned int g_imageWidth = 0;
unsigned int g_imageHeight = 0;
GLuint g_indirectionPool;
GLuint g_glslProgram;
GLuint g_glslTexture;
GLuint g_glslIndirectionPool;

/// <summary>
/// Lit une image en niveaux de gris depuis un fichier PGM.
/// </summary>
/// <param name="filename">Nom du fichier</param>
/// <param name="width">Reference vers une variable contenant la largeur de l'image.</param>
/// <param name="height">Reference vers une variable contenant la hauteur de l'image.</param>
/// <returns>Pointeur vers un tableau contenant les données de l'image.</returns>
BYTE *readPgm(const char *filename, unsigned int &width, unsigned int &height)
{
	FILE *file = fopen(filename, "rb");
	char line[10];
	do
	{
		fgets(line, 10, file);
	}
	while (sscanf(line, "%d %d", &width, &height) != 2);	
	BYTE *data = new BYTE[width * height];
	fseek(file, -1 * width * height, SEEK_END);
	fread(data, 1, width * height, file);
	fclose(file);
	return data;
}

/// <summary>
/// Insert une chaîne de caractères au début d'une autre.
/// </summary>
/// <param name="source">Chaîne de départ.</param>
/// <param name="text">Chaîne à insérer.</param>
/// <param name="freeAfter">Spécifie si la chaîne de départ doit être désalouée.</param>
/// <returns>Pointeur vers la nouvelle chaîne.</returns>
const char *insertStringBefore(const char *&source, const char *text, bool freeAfter = true)
{
	unsigned int n = strlen(text) + strlen(source) + 1;
	char *res = new char[n];
	strcpy(res, text);
	strcat(res, source);
	if (freeAfter) delete[] source;
	return res;
}

/// <summary>
/// Insert une définition de constante entière de préprocesseur au début d'une chaîne de caractères.
/// </summary>
/// <param name="source">Chaîne de départ.</param>
/// <param name="varaible">Nom de la constante.</param>
/// <param name="value">Valeur de la constante.</param>
/// <param name="freeAfter">Spécifie si la chaîne de départ doit être désalouée.</param>
/// <returns>Pointeur vers la nouvelle chaîne.</returns>
const char *insertDefine(const char *source, const char *variable, int value, bool freeAfter = true)
{
	char line[100];
	sprintf(line, "%s %s %d\n", "#define", variable, value);
	return insertStringBefore(source, line, freeAfter);
}

/// <summary>
/// Première tâche : on affiche la texture générée entièrement à l'aide d'un <c>GL_QUADS</c>
/// </summary>
void task0(void)
{
	glutReshapeWindow(g_textureWidth, g_textureHeight);
	glutSetWindowTitle("Texture stockant les patches de l'image initiale (appuyer sur 't' pour continuer)");
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, g_texture);
	glBegin(GL_QUADS);

	glTexCoord2d(0., 0.);
	glVertex2d(0., 0.);

	glTexCoord2d(1., 0.);
	glVertex2d(1., 0.);

	glTexCoord2d(1., 1.);
	glVertex2d(1., 1.);

	glTexCoord2d(0., 1.);
	glVertex2d(0., 1.);

	glEnd();
}

/// <summary>
/// Affiche l'image de départ à partir de la texture générée à l'aide d'un <c>GL_QUADS</c> par patch, en surimposant éventuellement des couleurs représentant l'arbre.
/// </summary>
/// <param name="drawTree">Spécifie si l'arbre doit être représenté.</param>
void drawImage(bool drawTree = false)
{
	glutReshapeWindow(g_imageWidth, g_imageHeight);

	unsigned int color = 0;

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, g_texture);

	glBegin(GL_QUADS);

	for (unsigned int i = 0; i < g_tree->getNLeaves(); ++i)
	{

		const QuadTree *leaf = g_tree->getLeaf(i);
		if (drawTree) glColor3f((float)((color % 3) == 0), (float)((color % 3) == 1), (float)((++color % 3) == 2));

		glTexCoord2d(leaf->getU0d(),leaf->getV0d());
		glVertex2d(leaf->getX0d(), leaf->getY0d());

		glTexCoord2d(leaf->getU1d(), leaf->getV0d());
		glVertex2d(leaf->getX1d(), leaf->getY0d());

		glTexCoord2d(leaf->getU1d(), leaf->getV1d());
		glVertex2d(leaf->getX1d(), leaf->getY1d());

		glTexCoord2d(leaf->getU0d(), leaf->getV1d());
		glVertex2d(leaf->getX0d(), leaf->getY1d());
	}
	glEnd();
}

/// <summary>
/// Deuxième tâche : on affiche l'image de départ à partir de la texture générée à l'aide d'un <c>GL_QUADS</c> par patch.
/// </summary>
void task1(void)
{
	glutSetWindowTitle("Image reconstruite par le CPU avec un GL_QUAD par patch (appuyer sur 't' pour continuer)");
	drawImage();
}

/// <summary>
/// Troisième tâche : on affiche l'image de départ à partir de la texture générée à l'aide d'un <c>GL_QUADS</c> par patch et en surimposant l'abre.
/// </summary>
void task2(void)
{
	glutSetWindowTitle("QuadTree décriavnt les patches (appuyer sur 't' pour continuer)");
	drawImage(true);
}

/// <summary>
/// Quatrième tâche : on affiche l'image de départ à partir de la texture générée à l'aide d'un unique <c>GL_QUADS</c> à l'aide du fragment shader et de l'indirection pool.
/// </summary>
void task3(void)
{
	glutSetWindowTitle("Image reconstruite via le fragment shader sur un unique GL_QUAD (appuyer sur 't' pour quitter)");
	glutReshapeWindow(g_imageWidth, g_imageHeight);

	glUseProgramObjectARB(g_glslProgram);
	glEnable(GL_TEXTURE_2D);

	glActiveTextureARB(GL_TEXTURE0_ARB);
	glBindTexture(GL_TEXTURE_2D, g_texture);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_2D, g_indirectionPool);

	glBegin(GL_QUADS);

	glTexCoord2d(0., 0.);
	glVertex2d(0., 0.);

	glTexCoord2d(1., 0.);
	glVertex2d(1., 0.);

	glTexCoord2d(1., 1.);
	glVertex2d(1., 1.);

	glTexCoord2d(0., 1.);
	glVertex2d(0., 1.);

	glEnd();
	glUseProgramObjectARB(0);

}

void mainKeyboard(unsigned char key, int x, int y) 
{
	switch (key)
	{
	case 't':
		++g_task;		
		break;
	case 'q':
		exit(0);
	}
}

void mainReshape(int w, int h)
{
	g_mainWindowWidth = w;
	g_mainWindowHeight = h;
	glViewport(0, 0, g_mainWindowWidth, g_mainWindowHeight);
}

void mainRender()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 1.0, 0.0, 1.0); 
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	switch (g_task % 5)
	{
	case 0:
		task0();
		break;
	case 1:
		task1();
		break;
	case 2:
		task2();
		break;
	case 3:
		task3();
		break;
	case 4:
		exit(0);
	}

	glutSwapBuffers();
}

void idle(void)
{
	glutPostRedisplay();
}

int main(int argc, char **argv)
{
	// On lit l'image et on crée le quad tree correspondant.
	BYTE *imageData = readPgm(argv[1], g_imageWidth, g_imageHeight);
	g_tree = new QuadTree(imageData, g_imageWidth, g_imageHeight);

	// On génère la texture contenant les patchs.
	BYTE *textureData = g_tree->generateTexture();
	delete[] imageData;
	g_textureHeight = g_tree->getTotalSizeV();
	g_textureWidth = g_tree->getTotalSizeU();

	// On génère l'indirection pool.
	float *indirectionPool = g_tree->generateIndirectionPool(true, 128);
	unsigned int indirectionPoolWidth = g_tree->getIndirectionPoolWidth();
	unsigned int indirectionPoolHeight = g_tree->getIndirectionPoolHeight();


	glutInit(&argc, argv);
	glutInitWindowSize(g_mainWindowWidth, g_mainWindowHeight); 
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	g_mainWindow = glutCreateWindow(NULL);
	glutSetWindow(g_mainWindow);
	glutDisplayFunc(mainRender);
	glutReshapeFunc(mainReshape);
	glutKeyboardFunc(mainKeyboard);
	glutIdleFunc(idle);
	gluxInit();

	glClearColor(0.0, 0.0, 0.0, 1.0);

	// On charge la texture contenant les patchs dans la mémoire vidéo.
	glGenTextures(1, &g_texture);
	glBindTexture(GL_TEXTURE_2D, g_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, g_textureWidth, g_textureHeight, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, textureData);
	delete[] textureData;

	// On charge l'indirection pool dans la mémoire vidéo.
	glGenTextures(1, &g_indirectionPool);
	glBindTexture(GL_TEXTURE_2D, g_indirectionPool);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, indirectionPoolWidth, indirectionPoolHeight, 0, GL_RGBA, GL_FLOAT, indirectionPool);
	delete[] indirectionPool;

	// On charge la source du fragment shader et on insère les définitions des différents paramètres.
	const char *fpCode = loadStringFromFile("quadTreeLookup.fp");
	fpCode = insertDefine(fpCode, "imageWidth", (int)g_imageWidth);
	fpCode = insertDefine(fpCode, "imageHeight", (int)g_imageHeight);
	fpCode = insertDefine(fpCode, "textureWidth", (int)g_textureWidth);
	fpCode = insertDefine(fpCode, "textureHeight", (int)g_textureHeight);
	fpCode = insertDefine(fpCode, "indirectionPoolWidth", (int)indirectionPoolWidth);
	fpCode = insertDefine(fpCode, "indirectionPoolHeight", (int)indirectionPoolHeight);
	g_glslProgram = createGLSLProgram(NULL, fpCode);
	delete[] fpCode;


	glUseProgramObjectARB(g_glslProgram);

	g_glslTexture = glGetUniformLocationARB(g_glslProgram, "u_texture");
	g_glslIndirectionPool = glGetUniformLocationARB(g_glslProgram, "u_indirectionPool");

	glUniform1iARB(g_glslTexture, 0);
	glUniform1iARB(g_glslIndirectionPool, 1); 
	glUseProgramObjectARB(0);

	glutMainLoop();

	glDeleteTextures(1, &g_texture);
	glDeleteTextures(1, &g_indirectionPool);

	delete g_tree;

	return 0;
}