/** @file main.h
*  @brief common includes, function prototypes for samples
*
*	@author <author>
*	@date <day-month> 2012
*/

#pragma once

#define WND_TITLE "OpenGL Sample"

#define WND_START_WIDTH  640
#define WND_START_HEIGHT 480

#define DO_NOT_SHOW_CONSOLE

struct Globals 
{
    static double sAppTime; // global app time in seconds
    static float sFps;

    static unsigned int sMainWindowWidth;
    static unsigned int sMainWindowHeight;

    static TwBar *sMainTweakBar;
};

///////////////////////////////////////////////////////////////////////////////
bool initApp();
void cleanUp();

// window:
void changeSize(int w, int h);

// keyboard:
void processNormalKeys(unsigned char key, int x, int y);
void pressSpecialKey(int key, int x, int y);
void releaseSpecialKey(int key, int x, int y);

// mouse:
void processMouse(int button, int state, int x, int y);
void processMouseMotion(int x, int y);
void processMousePassiveMotion(int x, int y);

// update & rendering
void updateScene(double deltaTime);
void renderScene();
