/** @file Texture.h
*  @brief useful functions for textures
*
*	@author Bartlomiej Filipek 
*	@date May 2012
*/


#pragma once

namespace textureLoader
{

    /// creates empty texture2D that can be ued to RenderToTextures for instance
    GLuint createEmptyTexture2D(GLuint w, GLuint h, GLenum internalFormat, GLenum format, GLenum dataType, 
        GLenum wrapType, GLenum minFiletr = GL_LINEAR, GLenum magFilter = GL_LINEAR);

    /// creates empty cube map texture
    GLuint createEmptyCubeMap(GLuint w, GLuint h, GLenum internalFormat, GLenum format, GLenum dataType,
        GLenum wrapType, GLenum minFiletr = GL_LINEAR, GLenum magFilter = GL_LINEAR);

    GLuint loadTextureFromFile(const char *fileName, bool genMipMaps);

} // namespace textureLoader
