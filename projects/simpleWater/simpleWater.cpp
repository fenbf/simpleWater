/** @file simpleWater.cpp
*  @brief simple water example
*
*  this example uses
*   - ping pong fbo
*   - WaterSurface class for height map based water animation
*   - WaterSurface class for normal map calculation - find correct version!
*   - shaders for water update
*   - shaders for simple normal mapping light, on a quad with water texture
*   - water is displayed as a quad (only four vertices) + normalmapping + some simple texture
*   - there is no environment around
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
#include "TimeQuery.h"

#include "waterSurface.h"


using namespace std;

///////////////////////////////////////////////////////////////////////////////
//
// globals
//

#define MEASURE_GL_TIME

#ifdef MEASURE_GL_TIME
TimerQuery gTimeQuery;
float gWaterUpdateTime;
#endif

// is aimation enabled?
bool gAnimate;

// water:
struct SimpleWater
{
    static const int GRID_SIZE = 512;

    WaterSurface  mSurface;
    GLuint        mVaoSurface;
    GLuint        mVboSurface;
    GLuint        mVaoRain;
    GLuint        mVboRain;
    glm::vec4     mSurfaceColor;
    GLuint        mTexture;
    int           mRainProbability;
    float         mRainForce;
    float		  mRefractionFactor;
    ShaderProgram mSurfaceShader;
    ShaderProgram mDebugShader;
    bool          mRenderDebug;
} gSimpleWater;

//
// transformations & camera
//
glm::vec3 gCamPos;
glm::mat4 gModelViewMatrix;
glm::mat3 gNormalMatrix;
glm::mat4 gProjectionMatrix;

///////////////////////////////////////////////////////////////////////////////
bool initApp() 
{
    // 
    // bacis OpenGL init
    //
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2f, 0.3f, 0.5f, 0.0f);

    // random...
    srand((unsigned int)time(NULL));

    //
    // transformation & camera
    //
    gCamPos = glm::vec3(0.0f, 2.0f, 2.0f);

    //
    // shaders
    //
    if (!shaderLoader::loadAndBuildShaderPairFromFile(&gSimpleWater.mSurfaceShader, "shaders/renderSurface.vs", "shaders/renderSurface.fs"))
    {
        return false;
    }

    if (!shaderLoader::loadAndBuildShaderPairFromFile(&gSimpleWater.mDebugShader, "shaders/renderSurfaceDebug.vs", "shaders/renderSurfaceDebug.fs"))
    {
        return false;
    }

#ifdef _DEBUG
    gSimpleWater.mSurfaceShader.validate();
    gSimpleWater.mDebugShader.validate();
#endif

    //
    // texture
    //
    gSimpleWater.mTexture = SOIL_load_OGL_texture("data/checkerboard.jpg", SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_GL_MIPMAPS | SOIL_FLAG_INVERT_Y);
    if (gSimpleWater.mTexture == 0)
    {
        LOG_ERROR("cannot load water texture!");
        return false;
    }

    //
    // water simulation
    //
    if (gSimpleWater.mSurface.init(SimpleWater::GRID_SIZE, SimpleWater::GRID_SIZE) == false)
    {
        LOG_ERROR("Cannot init water surface simulation");
        return false;
    }
    //
    // water geometry
    //

    // triangle strip
    float quadData[] = {
        -1.0f, 0.0f, -1.0f, // pos
        0.0f, 1.0f, 0.0f,   // normal (up in Y direction)
        0.0f, 0.0f,         // tex

        -1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f,

        1.0f, 0.0f, -1.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 0.0f,

        1.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 1.0f,
    };

    const GLsizei STRIDE = sizeof(float)*8;

    //
    // VAO setup
    //
    glGenVertexArrays(1, &gSimpleWater.mVaoSurface); 
    glBindVertexArray(gSimpleWater.mVaoSurface);

    // vbo:
    glGenBuffers(1, &gSimpleWater.mVboSurface);
    glBindBuffer(GL_ARRAY_BUFFER, gSimpleWater.mVboSurface);
    glBufferData(GL_ARRAY_BUFFER, STRIDE * 4, quadData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STRIDE, (const void *)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, STRIDE, (const void *)(sizeof(float)*3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, STRIDE, (const void *)(sizeof(float)*6));

    //
    // VAO for Rain
    //
    glGenVertexArrays(1, &gSimpleWater.mVaoRain); 
    glBindVertexArray(gSimpleWater.mVaoRain);

    // vbo:
    glGenBuffers(1, &gSimpleWater.mVboRain);
    glBindBuffer(GL_ARRAY_BUFFER, gSimpleWater.mVboRain);
    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float), NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, STRIDE, (const void *)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //
    // GUI & HUD & Timer
    //

#ifdef MEASURE_GL_TIME
    gTimeQuery.init();
    TwAddVarRO(Globals::sMainTweakBar, "water update (ms)", TW_TYPE_FLOAT, &gWaterUpdateTime, NULL);
#endif

    TwAddSeparator(Globals::sMainTweakBar, "", "");		

    TwAddVarRW(Globals::sMainTweakBar, "camera", TW_TYPE_DIR3F, &gCamPos[0], "");

    TwAddSeparator(Globals::sMainTweakBar, "", "");		

    gAnimate = true;
    TwAddVarRW(Globals::sMainTweakBar, "animate", TW_TYPE_BOOLCPP, &gAnimate, NULL);		

    gSimpleWater.mRenderDebug = false;
    TwAddVarRW(Globals::sMainTweakBar, "draw debug", TW_TYPE_BOOLCPP, &gSimpleWater.mRenderDebug, NULL);

    gSimpleWater.mSurfaceColor = glm::vec4(0.2f, 0.5f, 0.99f, 1.0f);
    TwAddVarRW(Globals::sMainTweakBar, "water color", TW_TYPE_COLOR4F, glm::value_ptr(gSimpleWater.mSurfaceColor), NULL);

    gSimpleWater.mRainProbability = 50;
    TwAddVarRW(Globals::sMainTweakBar, "rain probability", TW_TYPE_INT32, &gSimpleWater.mRainProbability, "min=0 max=100");

    gSimpleWater.mRainForce = 0.5f;
    TwAddVarRW(Globals::sMainTweakBar, "rain force", TW_TYPE_FLOAT, &gSimpleWater.mRainForce, "min=0.0 max=2.0 step=0.01");

    gSimpleWater.mRefractionFactor = 0.05f;
    TwAddVarRW(Globals::sMainTweakBar, "refraction", TW_TYPE_FLOAT, &gSimpleWater.mRefractionFactor, "min=0.0 max=1.0 step=0.005");

    TwAddVarRW(Globals::sMainTweakBar, "normal scale", TW_TYPE_DOUBLE, &gSimpleWater.mSurface.mNormalScale, "min=0.05 max=5.0 step=0.05");
    TwAddVarRW(Globals::sMainTweakBar, "offset scale", TW_TYPE_DOUBLE, &gSimpleWater.mSurface.mOffsetScale, "min=0.5 max=5.0 step=0.05");

    return true;
}

///////////////////////////////////////////////////////////////////////////////
void cleanUp()
{
#ifdef MEASURE_GL_TIME    
    LOG("Water average GPU time spent on update: %f ms", gTimeQuery.getAverageTime());
#endif

    glDeleteBuffers(1, &gSimpleWater.mVboSurface);
    glDeleteVertexArrays(1, &gSimpleWater.mVaoSurface);
    glDeleteBuffers(1, &gSimpleWater.mVboRain);
    glDeleteVertexArrays(1, &gSimpleWater.mVaoRain);
}

///////////////////////////////////////////////////////////////////////////////
void processNormalKeys(unsigned char key, int x, int y) 
{
    if (key == 27) 
        exit(0);
}

///////////////////////////////////////////////////////////////////////////////
void pressSpecialKey(int key, int x, int y) 
{
    if (key == GLUT_KEY_UP)
        gCamPos[2] += 0.1f;
    else if (key == GLUT_KEY_DOWN)
        gCamPos[2] -= 0.1f;
}


///////////////////////////////////////////////////////////////////////////////
void releaseSpecialKey(int key, int x, int y) 
{
    // todo: add some custom code...		
}

///////////////////////////////////////////////////////////////////////////////
void processMouse(int button, int state, int x, int y)
{
    // todo: add some custom code...
}

///////////////////////////////////////////////////////////////////////////////
void processMouseMotion(int x, int y)
{
    // todo: add some custom code...	
}

///////////////////////////////////////////////////////////////////////////////
void processMousePassiveMotion(int x, int y)
{
    // todo: add some custom code...	
}

///////////////////////////////////////////////////////////////////////////////
void changeSize(int w, int h)
{
    float aspect = (float)w/(float)h;

    // Set the viewport to be the entire window
    glViewport(0, 0, w, h);

    // setup projection matrix
    gProjectionMatrix = glm::perspective(45.0f, aspect, 0.1f, 100.0f);
}

///////////////////////////////////////////////////////////////////////////////
void updateScene(double deltaTime) 
{
    if (gAnimate == false) return;

    static float objAngle = 0.0f;
    objAngle += (float)deltaTime * 0.178f;

    //
    // update the water surface
    //
    float px = sinf(objAngle);
    float py = cosf(objAngle); 

#ifdef MEASURE_GL_TIME    
    gTimeQuery.begin();
#endif

    gSimpleWater.mSurface.beginUpdate(); 
    {
        if (gSimpleWater.mRainForce > 0.01f && gSimpleWater.mRainProbability > rand()%100)
        {
            glPointSize(1.5f);
            float rainDrop[3] = { utils::randFloatRange(-1.0f, 1.0f),			                         // pos X (from -1 to 1)
                utils::randFloatRange(-1.0f, 1.0f),                                    // pos Y (from -1 to 1)
                utils::randFloatRange(0.5f, 1.0f) * gSimpleWater.mRainForce * 5.0f };  // pressure

            glBindVertexArray(gSimpleWater.mVaoRain);
            glBindBuffer(GL_ARRAY_BUFFER, gSimpleWater.mVboRain);
            glBufferSubData(GL_ARRAY_BUFFER, 0, 3*sizeof(float), rainDrop);
            glDrawArrays(GL_POINTS, 0, 1);
            glBindVertexArray(0);
        }      
    }    
    gSimpleWater.mSurface.endUpdate();

#ifdef MEASURE_GL_TIME    
    gTimeQuery.end();
    gTimeQuery.updateResults(TimerQuery::WaitOption::WaitForResults);
    gWaterUpdateTime = (float)gTimeQuery.getTime();
#endif
}

///////////////////////////////////////////////////////////////////////////////
void renderScene() 
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //
    // camera
    //
    gModelViewMatrix = glm::lookAt(gCamPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    gNormalMatrix    = glm::transpose(glm::inverse(glm::mat3(gModelViewMatrix)));

    // render something
    if (gSimpleWater.mRenderDebug == false)
    {
        gSimpleWater.mSurfaceShader.use();

        gSimpleWater.mSurfaceShader.uniformMatrix4f("projectionMatrix", glm::value_ptr(gProjectionMatrix));
        gSimpleWater.mSurfaceShader.uniformMatrix3f("normalMatrix",     glm::value_ptr(gNormalMatrix));
        gSimpleWater.mSurfaceShader.uniformMatrix4f("modelviewMatrix",  glm::value_ptr(gModelViewMatrix));
        gSimpleWater.mSurfaceShader.uniform1i("texture0", 0);
        gSimpleWater.mSurfaceShader.uniform1i("normalMap", 1);
        glm::vec3 lightTemp = gNormalMatrix * glm::vec3(0.0f);
        gSimpleWater.mSurfaceShader.uniform3fv("lightPos", glm::value_ptr(lightTemp));
        gSimpleWater.mSurfaceShader.uniform4fv("waterColor", glm::value_ptr(gSimpleWater.mSurfaceColor));
        gSimpleWater.mSurfaceShader.uniform1f("refractionFactor", gSimpleWater.mRefractionFactor);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gSimpleWater.mSurface.normalsTexName()); 
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gSimpleWater.mTexture); 

        glBindVertexArray(gSimpleWater.mVaoSurface);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

        gSimpleWater.mSurfaceShader.disable();
    }
    else
    {
        gSimpleWater.mDebugShader.use();

        gSimpleWater.mDebugShader.uniformMatrix4f("projectionMatrix", glm::value_ptr(gProjectionMatrix));
        gSimpleWater.mDebugShader.uniformMatrix4f("modelviewMatrix",  glm::value_ptr(gModelViewMatrix));
        gSimpleWater.mDebugShader.uniform1i("texture0", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gSimpleWater.mSurface.normalsTexName()); 

        glBindVertexArray(gSimpleWater.mVaoSurface);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);

        gSimpleWater.mDebugShader.disable();
    }

    CHECK_OPENGL_ERRORS();
}
