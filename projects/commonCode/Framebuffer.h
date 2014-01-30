/** @file Framebuffer.h
*  @brief declarations of classes related to framebuffer objects and rendering to texture
*
*	@author Bartlomiej Filipek
*/

#pragma once


/** represents the FBO objects, that is used mostly in offscreen rendering */
class FrameBuffer 
{
private:
    struct Target 
    {
        Target() { mObject = 0; mType = 0; mActive = false; }
        GLuint mObject;		/**< id of object, texture or render buffer */
        GLenum mType;	    /**< type of object */
        bool mActive;	
    };

private:

    GLuint mFboId; //!< Framebuffer id

    std::vector<Target> mTargets;
    Target mDepthTarget;
    std::vector<GLenum> mDrawBuffers;

    bool mIsBounded; //!< true if given instance is currently bound as FBO

    int mWidth;
    int mHeight;

public:
    FrameBuffer();
    ~FrameBuffer();

    /** creates FBO */
    bool createAndBind();

    /** destroys the framebuffer */
    void destroy();

    /** attaches texture to specific color target target */
    void attachTextureAsColorTarget(GLuint destId, GLuint texId, int w, int h, GLenum texType);

    /** attaches texture layer to specified color target */
    void attachTextureLayerAsColorTarget(GLuint destId, GLuint layer, GLuint texId, int w, int h, GLenum texType);

    /** Detatches texture */
    void detachTextureFromColorTarget(GLuint destId);


    /** attaches texture as depth target */
    void attachTextureAsDepthTarget(GLuint texId, int w, int h, GLenum texType);
    void attachTextureLayerAsDepthTarget(GLuint layer, GLuint texId, int w, int h, GLenum texType);
    void detachTextureFromDepthTarget();

    /** creates and attaches renderbuffer that will be used for zbuffer */
    void createAndAttachDepthRenderbuffer(int w, int h);

    /** bind framebuffer and
    * @return false if completness is wrong
    */
    bool bind(bool setNewViewport);
    /** selects buffers to draw by the openGL */
    void setDrawBuffers();
    /** binds default frame buffer - system 
    * @param screenW width of the orginal window (before the fbo was bound), or -1 to not set it
    * @param screenH height of the orginal window (before the fbo was bound), or -1 to not set it
    */
    static void bindSystemFrameBuffer(int screenW = -1, int screenH = -1);

    /** checks its completness */
    bool check();

    /** simplifies process of binding texture from color target - binds as GL_TEXTURE_2D */
    void bindColorTargetAsTexture(GLuint colorTargetId);

    /** binds depth attachement as a texture */
    void bindDepthAsTexture();

    /** @returns tha name of depth buffer */
    GLuint getDepthTextureId() { assert(mDepthTarget.mObject != 0); return mDepthTarget.mObject; }

    /** @return width of the render target - common width of all the textures inside! */
    int getWidth() const { return mWidth; }
    /** @return height of the render target - common height of all the textures inside! */
    int getHeight() const { return mHeight; }

    GLuint getId() const { return mFboId; }

public:
    /** @return maximum of color targets */
    static GLuint getMaxColorTargets();
    /** @returns maximal renderbuffer size */
    static GLuint getMaxRenderbufferSize();

private:
    /** Should not be called from the outside of this class */
    void unbind();

private:
    /// calculated when the first Framebuffer is created using 'createAndBind'
    static GLuint sMaxColorTargets;
    /// calculated when the first Framebuffer is created using 'createAndBind'
    static GLuint sMaxRenderbufferSize;
    static FrameBuffer *sCurrentBinding;

    //
    // block copying:
    //
    FrameBuffer(const FrameBuffer &) { }
    FrameBuffer & operator=(const FrameBuffer &) { return *this; }
};
