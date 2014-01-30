/** @file Framebuffer.h
*  @brief declarations of classes related to framebuffer objects and rendering to texture
*
*	@author Bartlomiej Filipek
*/

#include "commonCode.h"

#include "Init.h"
#include "Log.h"
#include "Framebuffer.h"

using namespace std;


FrameBuffer *FrameBuffer::sCurrentBinding = NULL;

GLuint FrameBuffer::sMaxColorTargets = 0;
GLuint FrameBuffer::sMaxRenderbufferSize = 0;

///////////////////////////////////////////////////////////////////////////////
FrameBuffer::FrameBuffer() : mHeight(0), mWidth(0), mIsBounded(false), mFboId(0)
{

}

///////////////////////////////////////////////////////////////////////////////
FrameBuffer::~FrameBuffer() 
{
    destroy();

    mTargets.clear();
    mDrawBuffers.clear();
}

///////////////////////////////////////////////////////////////////////////////
bool FrameBuffer::createAndBind() 
{
    // calculate max values if this is first FBO...
    if (sMaxColorTargets == 0 && sMaxRenderbufferSize == 0)
    {
        sMaxColorTargets = getMaxColorTargets();
        sMaxRenderbufferSize = getMaxRenderbufferSize();
    }

    // add mx num of draw buffers to targets array
    mTargets.clear();
    for (GLuint i = 0; i < sMaxColorTargets; ++i)
        mTargets.push_back(Target());

    if (sCurrentBinding != NULL && sCurrentBinding != this)
        sCurrentBinding->unbind();

    glGenFramebuffers(1, &mFboId);

    glBindFramebuffer(GL_FRAMEBUFFER, mFboId);
    mIsBounded = true;
    sCurrentBinding = this;

    LOG_SUCCESS("fbo created id: %d, ok", mFboId);
    return true;
}

///////////////////////////////////////////////////////////////////////////////
void FrameBuffer::destroy() 
{
    if (mTargets.size() > 0 && mFboId != 0)
    {
        if (mDepthTarget.mActive == true && mDepthTarget.mType == GL_RENDERBUFFER)
            glDeleteRenderbuffers(1, &mDepthTarget.mObject);

        glDeleteFramebuffers(1, &mFboId);
        mFboId = 0;
    }
    mTargets.clear();
}

///////////////////////////////////////////////////////////////////////////////
void FrameBuffer::attachTextureAsColorTarget(GLuint destId, GLuint texId, int w, int h, GLenum texType) 
{
    assert(texId != 0);
    assert(destId < sMaxColorTargets);
    assert(mIsBounded == true);

    mDrawBuffers.push_back(GL_COLOR_ATTACHMENT0 + destId);

    mTargets[destId].mActive = true;
    mTargets[destId].mObject = texId;
    mTargets[destId].mType = texType;

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+destId, texType, texId, 0);

    // get W & H:
    mWidth = w;
    mHeight = h;
}

///////////////////////////////////////////////////////////////////////////////
void FrameBuffer::attachTextureLayerAsColorTarget(GLuint destId, GLuint layer, GLuint texId, int w, int h, GLenum texType) 
{
    assert(texId != 0);
    assert(destId < sMaxColorTargets);
    assert(mIsBounded == true);

    mDrawBuffers.push_back(GL_COLOR_ATTACHMENT0 + destId);

    mTargets[destId].mActive = true;
    mTargets[destId].mObject = texId;
    mTargets[destId].mType = texType;

    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + destId, texId, 0, layer);

    // get W & H:
    mWidth = w;
    mHeight = h;	
}

///////////////////////////////////////////////////////////////////////////////
void FrameBuffer::detachTextureFromColorTarget(GLuint destId) 
{
    assert(destId < sMaxColorTargets);
    assert(mIsBounded == true);

    mTargets[destId].mActive = false;
    mTargets[destId].mObject = 0;

    vector<GLenum>::iterator it = mDrawBuffers.begin();
    while ((*it) != GL_COLOR_ATTACHMENT0 + destId && it != mDrawBuffers.end()) ++it;
    mDrawBuffers.erase(it);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + destId, mTargets[destId].mType, 0, 0);

    mTargets[destId].mType = 0;
}

///////////////////////////////////////////////////////////////////////////////
void FrameBuffer::attachTextureAsDepthTarget(GLuint texId, int w, int h, GLenum texType)
{
    assert(texId != 0);
    assert(mIsBounded == true);

    // destroy the old buffer if needed
    if (mDepthTarget.mObject != 0 && mDepthTarget.mType == GL_RENDERBUFFER) {
        // maybe it would be better to save it rather that destroy?
        glDeleteRenderbuffers(1, &mDepthTarget.mObject);
    }

    mDepthTarget.mActive = true;
    mDepthTarget.mObject = texId;
    mDepthTarget.mType = texType;

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texType, texId, 0);	

    mWidth = w;
    mHeight = h;
}

///////////////////////////////////////////////////////////////////////////////
void FrameBuffer::attachTextureLayerAsDepthTarget(GLuint layer, GLuint texId, int w, int h, GLenum texType) 
{
    assert(texId != 0);
    assert(mIsBounded == true);

    // destroy the old buffer if needed
    if (mDepthTarget.mObject != 0 && mDepthTarget.mType == GL_RENDERBUFFER) {
        // maybe it would be better to save it rather that destroy?
        glDeleteRenderbuffers(1, &mDepthTarget.mObject);
    }

    mDepthTarget.mActive = true;
    mDepthTarget.mObject = texId;
    mDepthTarget.mType = texType;

    glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texId, 0, layer);	

    mWidth = w;
    mHeight = h;
}

///////////////////////////////////////////////////////////////////////////////
void FrameBuffer::detachTextureFromDepthTarget()
{
    assert(mIsBounded == true && false);

    // TODO: implement it
}

///////////////////////////////////////////////////////////////////////////////
void FrameBuffer::createAndAttachDepthRenderbuffer(int w, int h)
{
    assert(mIsBounded == true);
    assert(mDepthTarget.mType == 0);

    mDepthTarget.mType = GL_RENDERBUFFER;
    glGenRenderbuffers(1, &mDepthTarget.mObject);
    glBindRenderbuffer(GL_RENDERBUFFER, mDepthTarget.mObject);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_RENDERBUFFER, mDepthTarget.mObject);
}

///////////////////////////////////////////////////////////////////////////////
bool FrameBuffer::bind(bool setNewViewport) 
{
    if (mIsBounded) return true;

    if (sCurrentBinding != NULL && sCurrentBinding != this)
        sCurrentBinding->unbind();

    glBindFramebuffer(GL_FRAMEBUFFER, mFboId);

    // check completness
#ifdef _DEBUG
    if (check() == false) return false;
#endif


    mIsBounded = true;
    sCurrentBinding = this;

    if (setNewViewport)
        glViewport(0, 0, mWidth, mHeight);

    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool FrameBuffer::check()
{
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(status != GL_FRAMEBUFFER_COMPLETE) 
    {
        LOG_ERROR("status != GL_FRAMEBUFFER_COMPLETE");

        switch(status) 
        {
        case GL_FRAMEBUFFER_UNSUPPORTED:
            LOG_ERROR( "Unsupported framebuffer format");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
            LOG_ERROR("Framebuffer incomplete attachment");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            LOG_ERROR("Framebuffer incomplete, missing attachment");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            LOG_ERROR("Framebuffer incomplete, missing draw buffer");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            LOG_ERROR("Framebuffer incomplete, missing read buffer");
            break;
        default:
            LOG_ERROR("Unknown error %d\n", status);
        }

        return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
void FrameBuffer::setDrawBuffers() 
{
    //assert(GLEW_ARB_draw_buffers && "extensions must be checked in init phase!");

    glDrawBuffers((GLsizei)mDrawBuffers.size(), (const GLenum *)&mDrawBuffers[0]);
}

///////////////////////////////////////////////////////////////////////////////
void FrameBuffer::unbind() 
{
    if (sCurrentBinding == this)
    {
        mIsBounded = false;
    }
}

///////////////////////////////////////////////////////////////////////////////
void FrameBuffer::bindColorTargetAsTexture(GLuint colorTargetId)
{
    assert(colorTargetId < sMaxColorTargets);
    assert(mTargets[colorTargetId].mActive == true);
    assert(mTargets[colorTargetId].mType == GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, mTargets[colorTargetId].mObject);
}

///////////////////////////////////////////////////////////////////////////////
void FrameBuffer::bindDepthAsTexture() 
{
    assert(mDepthTarget.mActive == true);
    assert(mDepthTarget.mType == GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, mDepthTarget.mObject);
}

///////////////////////////////////////////////////////////////////////////////
GLuint FrameBuffer::getMaxColorTargets() 
{
    int temp = 0;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &temp);
    return static_cast<GLuint>(temp);
}

///////////////////////////////////////////////////////////////////////////////
GLuint FrameBuffer::getMaxRenderbufferSize() 
{
    int temp = 0;
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &temp);
    return static_cast<GLuint>(temp);
}

///////////////////////////////////////////////////////////////////////////////
void FrameBuffer::bindSystemFrameBuffer(int screenW, int screenH) 
{
    if (sCurrentBinding != NULL) 
        sCurrentBinding->unbind();

    sCurrentBinding = NULL;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);

    if (screenW > 0 && screenH > 0)
        glViewport(0, 0, screenW, screenH);
}