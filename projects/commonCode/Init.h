/** @file init.h
*  @brief init functions for the application
*
*	@author Bartlomiej Filipek 
*/

#pragma once

#ifdef _DEBUG
#define CHECK_OPENGL_ERRORS() utils::checkGLErrors(__FUNCTION__, __FILE__, __LINE__);
#else
#define CHECK_OPENGL_ERRORS() 
#endif

namespace utils
{
    bool initGL(bool vsync);

    void calculateFps(float *fps);

    void updateTimer(double *deltaTime, double *appTime, const double MAX_REFRESH_TIME = 1.0/60.0);

    bool checkGLErrors(const char *func, const char * file, int line);

    inline float randFloatRange(float a, float b);
} // namespce utils	


//
// inline
//
inline float utils::randFloatRange(float a, float b)
{
    return a + (float)rand()/(float)RAND_MAX * (b-a);
}
