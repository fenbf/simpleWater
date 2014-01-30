/** @file waterSurface.h
*  @brief water surface class
*
*	@author Bartlomiej Filipek 
*/

#pragma once

#include "FrameBuffer.h"

/** simple heght map based water surface simulation that is performed on the GPU
*
* result: two textures: one with height data (height, velocity) and the next one with normals
*
* drawing on the water can be done between beginUpdate and endUpdate methods
*
* in the next version of the class, normal map calcultions should be done outside
*/
class WaterSurface
{
protected:
    GLuint mWidth;
    GLuint mHeight;

    GLuint mCurrID;

    FrameBuffer mFboForWater[2];
    FrameBuffer mFboForNormals;

    GLuint mWaterDataTex[2];
    GLuint mNormalsTex;

    GLuint mQuadVBO;
    GLuint mQuadVAO;

    ShaderProgram mDrawShader;
    ShaderProgram mComputeShader;
    ShaderProgram mComputeNormalsShader;

    bool mEnabled;

    /// for beginUpdate/endUpdate matching...
    bool mBeginUpdateCalled;
    /// saved viewport so that it can be restored in the endUpdate
    int mSavedViewport[4];
public:
    /// fade DY factor: when 1 the water does not fade, default value is 0.99
    double mfadeDY; 
    /// max value is 1.8, default value is 1/4
    double mgatherFactor;
    /// fade Y factor: when 1 the water does not fade, default value is 0.99
    double mfadeY;
    /// strenght of the normalmap in the Z direction
    /// the higher the flatter normal map is
    double mNormalScale;
    /// distance to neighbour - 1.0 is the default value, 
    /// used in normal map update and water simulation update
    double mOffsetScale;
public:
    WaterSurface();
    virtual ~WaterSurface();

    /// initializes all the needed data
    bool init(GLuint width, GLuint height);

    void beginUpdate();
    void endUpdate();

    const GLuint dataTexName() const { return mWaterDataTex[mCurrID]; }
    const GLuint normalsTexName() const { return mNormalsTex; }

    GLuint width() const { return mWidth; }
    GLuint height() const { return mHeight; }
protected:
    bool initShaders();
    bool initBuffers();

    // block copying
    WaterSurface(const WaterSurface &) { }
    WaterSurface& operator=(const WaterSurface&) { }
};
