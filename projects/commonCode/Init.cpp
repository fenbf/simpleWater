/** @file init.cpp
*  @brief impleentation of useful utilities for opengl based apps
*
*	@author Bartlomiej Filipek 
*/

#include "commonCode.h"
#include "init.h"
#include "Log.h"


void APIENTRY DebugFunc(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
						const GLchar* message, const GLvoid* userParam)
{
	std::string srcName;
	switch(source)
	{
	case GL_DEBUG_SOURCE_API_ARB: srcName = "API"; break;
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB: srcName = "Window System"; break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB: srcName = "Shader Compiler"; break;
	case GL_DEBUG_SOURCE_THIRD_PARTY_ARB: srcName = "Third Party"; break;
	case GL_DEBUG_SOURCE_APPLICATION_ARB: srcName = "Application"; break;
	case GL_DEBUG_SOURCE_OTHER_ARB: srcName = "Other"; break;
	}

	std::string errorType;
	switch(type)
	{
	case GL_DEBUG_TYPE_ERROR_ARB: errorType = "Error"; break;
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB: errorType = "Deprecated Functionality"; break;
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB: errorType = "Undefined Behavior"; break;
	case GL_DEBUG_TYPE_PORTABILITY_ARB: errorType = "Portability"; break;
	case GL_DEBUG_TYPE_PERFORMANCE_ARB: errorType = "Performance"; break;
	case GL_DEBUG_TYPE_OTHER_ARB: errorType = "Other"; break;
	}

	std::string typeSeverity;
	switch(severity)
	{
	case GL_DEBUG_SEVERITY_HIGH_ARB: typeSeverity = "High"; break;
	case GL_DEBUG_SEVERITY_MEDIUM_ARB: typeSeverity = "Medium"; break;
	case GL_DEBUG_SEVERITY_LOW_ARB: typeSeverity = "Low"; break;
	}

	LOG("%s from %s,\t%s priority\nMessage: %s\n",
		errorType.c_str(), srcName.c_str(), typeSeverity.c_str(), message);
}

namespace utils
{
    bool initGL(bool vsync)
    {
        int glLoadStatus = ogl_LoadFunctions();
        if (glLoadStatus == ogl_LOAD_FAILED)
        {
            LOG_ERROR("Cannot load opengl 4.2");
            return false;
        }
        else if (glLoadStatus > ogl_LOAD_SUCCEEDED)
        {
            LOG("OpenGL Loaded, but without %d extensions!", glLoadStatus - ogl_LOAD_SUCCEEDED);
        }

        int wglLoadStatus = wgl_LoadFunctions(::wglGetCurrentDC());
        if (wglLoadStatus == ogl_LOAD_FAILED)
        {
            LOG_ERROR("Cannot load WGL extensions");
        }
        else if (wglLoadStatus > wgl_LOAD_SUCCEEDED)
        {
            LOG("WGL Loaded, but without %d extensions!", wglLoadStatus - wgl_LOAD_SUCCEEDED);
        }

        if (ogl_ext_ARB_debug_output)
	    {
    		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
	    	glDebugMessageCallbackARB((GLDEBUGPROCARB)DebugFunc, (void*)15);
            LOG("Debug Message Callback turned on!");
        }
        else
            LOG("NO Debug Message Callback ;(");			

        // print some GL info:
        int majorVersion = -1;
        int minorVersion = -1;
        
        glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
        glGetIntegerv(GL_MINOR_VERSION, &minorVersion);
        LOG("- - - - - - - - - - - - - - - - - - - - - - - ");
        LOG("OpenGL Version:  %d:%d", majorVersion, minorVersion);
        LOG("OpenGL Renderer: %s", (char *)glGetString(GL_RENDERER));
        LOG("OpenGL Vendor:   %s", (char *)glGetString(GL_VENDOR));
        LOG("GLSL Version:    %s", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
        LOG("- - - - - - - - - - - - - - - - - - - - - - - \n");

#ifdef WIN32
        if (wgl_ext_EXT_swap_control)
            wglSwapIntervalEXT(vsync ? 1 : 0);
#endif

        return true;
    }

    void calculateFps(float *fps)
    {
        static unsigned int frame = 0;
        static int timeBase = 0;

        frame++;

        int t = glutGet(GLUT_ELAPSED_TIME);
        if (t - timeBase > 1000) 
        {
            *fps = 0.5f*(*fps) + 0.5f*(frame*1000.0f/(float)(t - timeBase));
            timeBase = t;		
            frame = 0;
        }

    }

    void updateTimer(double *deltaTime, double *appTime, const double MAX_REFRESH_TIME)
    {
        static double lastDeltas[3] = { 0.0, 0.0, 0.0 };

        // in milisec
        int t = glutGet(GLUT_ELAPSED_TIME);
        double newTime = (double)t*0.001;

        *deltaTime = newTime - *appTime;
        if ((*deltaTime) < 0) *deltaTime = 0.0;
        if ((*deltaTime) > MAX_REFRESH_TIME) *deltaTime = MAX_REFRESH_TIME;

        // average:
        *deltaTime = ((*deltaTime) + lastDeltas[0] + lastDeltas[1] + lastDeltas[2]) * 0.25;

        *appTime = (*appTime) + (*deltaTime);

        // save delta:
        lastDeltas[0] = lastDeltas[1];
        lastDeltas[1] = lastDeltas[2];
        lastDeltas[2] = *deltaTime;
    }

    bool checkGLErrors(const char *func, const char * file, int line)
    {
        GLenum err;
        bool good = true;

        err = glGetError();
        while (err != GL_NO_ERROR)
        {
            const char * message = nullptr;
            switch( err )
            {
            case GL_INVALID_ENUM:
                message = "Invalid enum";
                break;
            case GL_INVALID_VALUE:
                message = "Invalid value";
                break;
            case GL_INVALID_OPERATION:
                message = "Invalid operation";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                message = "Invalid framebuffer operation";
                break;
            case GL_OUT_OF_MEMORY:
                message = "Out of memory";
                break;
            default:
                message = "Unknown error";
            }

            LOG("OpenGL ERROR in [%s] %s(%d): %s\n", func, logger::fileNameFromPath(file), line, message);
            good = false;
            err = glGetError();
        }
        return good;
    }
} // namespace utils