/** @file waterSurface.cpp
*  @brief water surface class implementation
*
*	Effects in Games, June 2012

*/

#include "stdafx.h"

#include "Init.h"
#include "Log.h"
#include "DisplayUtils.h"
#include "shaderProgram.h"
#include "shaderLoader.h"
#include "texture.h"
#include "framebuffer.h"

#include "WaterSurface.h"

///////////////////////////////////////////////////////////////////////////////
WaterSurface::WaterSurface()
{
    mWidth  = 0;
    mHeight = 0;

    mfadeDY = 0.990;
    mgatherFactor = 1.0/4.0;
    mfadeY = 0.990;

    mNormalScale = 1.0;

    mEnabled = false;

    mCurrID = 0;

    // clear ids:
    mWaterDataTex[0] = 0;
    mWaterDataTex[1] = 0;
    mNormalsTex = 0;
    mQuadVBO = 0;
    mQuadVAO = 0;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
WaterSurface::~WaterSurface()
{
    glDeleteTextures(1, &mWaterDataTex[0]);
    glDeleteTextures(1, &mWaterDataTex[1]);
    glDeleteTextures(1, &mNormalsTex);

    glDeleteBuffers(1, &mQuadVBO);
    glDeleteVertexArrays(1, &mQuadVAO);
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
bool WaterSurface::init(GLuint width, GLuint height)
{
    mWidth = width;
    mHeight = height;

    mfadeDY = 0.9f;
    mgatherFactor = 1.0f/4.0f;
    mfadeY = 0.999999f;      

    mNormalScale = 1.0;
    mOffsetScale = 1.0;

    mEnabled = true;

    mCurrID = 0;

    CHECK_OPENGL_ERRORS();
    if (initBuffers() == false)
        return false;

    if (initShaders() == false)
        return false;

    CHECK_OPENGL_ERRORS();
    displayUtils::initQuadGeometry(&mQuadVAO, &mQuadVBO);
    CHECK_OPENGL_ERRORS();

    mBeginUpdateCalled = false;

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
void WaterSurface::beginUpdate()
{
    if (!mEnabled) 
        return;

    if (mBeginUpdateCalled)
        LOG_ERROR("beginUpdate called but was not finished with endUpdate probably!");

    // ping pong buffers
    GLuint nextID = 1 - mCurrID;

    glGetIntegerv(GL_VIEWPORT, mSavedViewport);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    float densities[3] = { (float)mfadeDY, (float)mgatherFactor, (float)mfadeY };

    //
    // 1. bind fbo for DY, set Y texture for shader
    //
    mFboForWater[nextID].bind(true);
    mComputeShader.use();
    mComputeShader.uniform3f("density", densities[0], densities[1], densities[2]);
    mComputeShader.uniform2f("texelSize", (float)mOffsetScale/(float)mWidth, (float)mOffsetScale/(float)mHeight);
    glActiveTexture(GL_TEXTURE0);
    mFboForWater[mCurrID].bindColorTargetAsTexture(0); 

    displayUtils::drawQuad(mQuadVAO); 
    mDrawShader.use();

    mBeginUpdateCalled = true;
}

void WaterSurface::endUpdate()
{
    if (!mEnabled) 
        return;

    if (!mBeginUpdateCalled)
        LOG_ERROR("endUpdate called but was not started with beginUpdate probably!");

    GLuint nextID = 1 - mCurrID;		

    //
    // 2. calculate normals
    //
    mFboForNormals.bind(false);		// this time we do not have to set new viepoer, its the same as before
    mComputeNormalsShader.use();
    mComputeNormalsShader.uniform1f("normalScale", (float)mNormalScale);
    mComputeNormalsShader.uniform2f("texelSize", (float)mOffsetScale/(float)mWidth, (float)mOffsetScale/(float)mHeight);
    mFboForWater[nextID].bindColorTargetAsTexture(0);
    displayUtils::drawQuad(mQuadVAO); 


    mComputeNormalsShader.disable();

    // this is called inside the bindSystemFrameBuffer() method
    //mFboForNormals.unbind();

    mCurrID = nextID;

    // restore:
    FrameBuffer::bindSystemFrameBuffer();
    glViewport(mSavedViewport[0], mSavedViewport[1], mSavedViewport[2], mSavedViewport[3]);

    mBeginUpdateCalled = false;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
bool WaterSurface::initBuffers()
{
    CHECK_OPENGL_ERRORS();
    //
    // clear if needed
    //
    if (mWaterDataTex[0] > 0) glDeleteTextures(1, &mWaterDataTex[0]);
    if (mWaterDataTex[1] > 0) glDeleteTextures(1, &mWaterDataTex[1]);
    if (mNormalsTex > 0)      glDeleteTextures(1, &mNormalsTex);

    // 
    // generate textures:
    //
    // todo: use some Texture class...
    mWaterDataTex[0] = textureLoader::createEmptyTexture2D(mWidth, mHeight, GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE);
    mWaterDataTex[1] = textureLoader::createEmptyTexture2D(mWidth, mHeight, GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE);
    mNormalsTex      = textureLoader::createEmptyTexture2D(mWidth, mHeight, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, GL_CLAMP_TO_EDGE);
    CHECK_OPENGL_ERRORS();
    // get current settings:
    int view[4];
    glGetIntegerv(GL_VIEWPORT, view);
    float col[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, col);

    // new viewport:
    glViewport(0, 0, mWidth, mHeight);
    //
    // fbo:
    //

    // 1:
    mFboForWater[0].createAndBind();
    mFboForWater[0].attachTextureAsColorTarget(0, mWaterDataTex[0], mWidth, mHeight, GL_TEXTURE_2D);
    mFboForWater[0].setDrawBuffers();
    mFboForWater[0].check();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    // 2:
    mFboForWater[1].createAndBind();
    mFboForWater[1].attachTextureAsColorTarget(0, mWaterDataTex[1], mWidth, mHeight, GL_TEXTURE_2D);
    mFboForWater[1].setDrawBuffers();

    mFboForWater[1].check();

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    // 3:
    mFboForNormals.createAndBind();
    mFboForNormals.attachTextureAsColorTarget(0, mNormalsTex, mWidth, mHeight, GL_TEXTURE_2D);
    mFboForNormals.setDrawBuffers();

    mFboForNormals.check();

    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // restore:
    FrameBuffer::bindSystemFrameBuffer();
    glViewport(view[0], view[1], view[2], view[3]);
    glClearColor(col[0], col[1], col[2], col[3]);

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
bool WaterSurface::initShaders()
{
    if (!shaderLoader::loadAndBuildShaderPairFromFile(&mDrawShader, "shaders/waterPassThrough.vs", "shaders/waterDraw.fs"))
    {
        return false;
    }

    if (!shaderLoader::loadAndBuildShaderPairFromFile(&mComputeShader, "shaders/waterPassThrough.vs", "shaders/waterUpdate.fs"))
    {
        return false;
    }

    mComputeShader.use();
    mComputeShader.uniform1i("texture0", 0);
    mComputeShader.uniform2f("texelSize", (float)mOffsetScale/(float)mWidth, (float)mOffsetScale/(float)mHeight);

    if (!shaderLoader::loadAndBuildShaderPairFromFile(&mComputeNormalsShader, "shaders/waterPassThrough.vs", "shaders/waterUpdateNormals.fs"))
    {
        return false;
    }

    mComputeNormalsShader.use();
    mComputeNormalsShader.uniform1i("texture0", 0);
    mComputeNormalsShader.uniform2f("texelSize", (float)mOffsetScale/(float)mWidth, (float)mOffsetScale/(float)mHeight);

#ifdef _DEBUG
    mDrawShader.validate();
    mComputeShader.validate();
    mComputeNormalsShader.validate();
#endif

    glUseProgram(0);

    return true;
}
