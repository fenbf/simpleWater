/** @file main.cpp
 *  @brief core code for the sample, init, freeGlut setup, etc, etc
 *
 *	@author Bartlomiej Filipek 
 */

#include "stdafx.h"
#include "main.h"
#include "Init.h"
#include "Log.h"

#include "Shader.h"
#include "ShaderProgram.h"
#include "shaderLoader.h"


#ifdef DO_NOT_SHOW_CONSOLE
	#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#endif

using namespace std;

///////////////////////////////////////////////////////////////////////////////

// time:
double Globals::sAppTime = 0.0;	// global app time in seconds
float Globals::sFps = 0.0f;

unsigned int Globals::sMainWindowWidth = WND_START_WIDTH;
unsigned int Globals::sMainWindowHeight = WND_START_HEIGHT;

TwBar *Globals::sMainTweakBar = NULL;

///////////////////////////////////////////////////////////////////////////////
bool initApp();
void cleanUp();

// window:
void mainChangeSize(int w, int h);

// mouse:
void mainProcessMouse(int button, int state, int x, int y);
void mainProcessMouseMotion(int x, int y);
void mainProcessMousePassiveMotion(int x, int y);

// update & rendering
void mainIdle();

///////////////////////////////////////////////////////////////////////////////
// entry point
int main(int argc, char **argv)
{
	//
	// init GLUT
	//
	glutInit(&argc, argv);

	glutInitContextVersion(4, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitContextFlags(GLUT_DEBUG);

	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200,200);
	glutInitWindowSize(WND_START_WIDTH, WND_START_HEIGHT);
	// glutMainLoop returns and we can release our resources
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	if (glutCreateWindow(WND_TITLE) < 1)
	{
		LOG_ERROR("Cannot create the main GLUT window!");
		return 1;
	}

	// set callbacks:
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(pressSpecialKey);
	glutSpecialUpFunc(releaseSpecialKey);
	glutMouseFunc(mainProcessMouse);
	glutMotionFunc(mainProcessMouseMotion);
	glutPassiveMotionFunc(mainProcessMousePassiveMotion);
	glutDisplayFunc(renderScene);
	glutIdleFunc(mainIdle);
	glutReshapeFunc(mainChangeSize);

	//
	// 'init' OpenGL: extension, version and print some basic info
	//
	if (utils::initGL(true) == false) 
		return 1;	

	// Initialize AntTweakBar
    TwInit(TW_OPENGL_CORE, NULL);

	Globals::sMainTweakBar = TwNewBar("TweakBar");
	TwAddVarRO(Globals::sMainTweakBar, "FPS", TW_TYPE_FLOAT, &Globals::sFps, NULL);

	CHECK_OPENGL_ERRORS();

	//
	// init whole application:
	//
	if (initApp() == false)
	{
		LOG_ERROR("cannot init application...");
		return 1;
	}

	//
	// start rendering and animation:
	//
	glutMainLoop();

	//
	// finish:
	//
	cleanUp();
	TwTerminate();

	return(0);
}

///////////////////////////////////////////////////////////////////////////////
void mainChangeSize(int w, int h)
{
	Globals::sMainWindowWidth = w;
	Globals::sMainWindowHeight = h;		

    changeSize(w, h);

	// Send the new window size to AntTweakBar
    TwWindowSize(w, h);
}

///////////////////////////////////////////////////////////////////////////////
void mainProcessMouse(int button, int state, int x, int y)
{
	processMouse(button, state, x, y);
	
	// send message to antTweakBar
	TwEventMouseButtonGLUT(button, state, x, y);
}

///////////////////////////////////////////////////////////////////////////////
void mainProcessMouseMotion(int x, int y)
{
	processMouseMotion(x, y);
	
	// send message to antTweakBar
	TwEventMouseMotionGLUT(x, y);		
}

///////////////////////////////////////////////////////////////////////////////
void mainProcessMousePassiveMotion(int x, int y)
{
	processMousePassiveMotion(x, y);
	
	// send message to antTweakBar
	TwEventMouseMotionGLUT(x, y);		
}

///////////////////////////////////////////////////////////////////////////////
void mainIdle()
{
	double deltaTime;
	utils::updateTimer(&deltaTime, &Globals::sAppTime);
	
	utils::calculateFps(&Globals::sFps);

	// call Update:
	updateScene(deltaTime);

	// render frame:
	renderScene();

	TwDraw();

	glutSwapBuffers();
}
