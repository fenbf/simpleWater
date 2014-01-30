/** @file texture.cpp
*  @brief useful functions for textures
*
*	Effects in Games, June 2012

*/

#include "commonCode.h"
#include "soil.h"

#include "Init.h"

namespace textureLoader
{
    GLuint createEmptyTexture2D(GLuint w, GLuint h, GLenum internalFormat, GLenum format, GLenum dataType,
        GLenum wrapType /*= GL_CLAMP_TO_EDGE*/, GLenum minFiletr /*= GL_LINEAR*/, GLenum magFilter /*= GL_LINEAR*/)
    {
        GLuint texId;
        glGenTextures(1, &texId);

        if (texId == 0)
            return 0;

        glBindTexture(GL_TEXTURE_2D, texId);
        CHECK_OPENGL_ERRORS();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapType);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapType);
        CHECK_OPENGL_ERRORS();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFiletr);
        CHECK_OPENGL_ERRORS();

        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, dataType, NULL); 

        return texId;
    }

    GLuint createEmptyCubeMap(GLuint w, GLuint h, GLenum internalFormat, GLenum format, GLenum dataType,
        GLenum wrapType /*= GL_CLAMP_TO_EDGE*/, GLenum minFiletr /*= GL_LINEAR*/, GLenum magFilter /*= GL_LINEAR*/)
    {
        GLuint texId;
        glGenTextures(1, &texId);

        if (texId == 0)
            return 0;

        glBindTexture(GL_TEXTURE_CUBE_MAP, texId);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, wrapType);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, wrapType);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, wrapType);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, magFilter);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minFiletr); 

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, internalFormat, w, h, 0, format, dataType, NULL);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, internalFormat, w, h, 0, format, dataType, NULL);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, internalFormat, w, h, 0, format, dataType, NULL);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, internalFormat, w, h, 0, format, dataType, NULL);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, internalFormat, w, h, 0, format, dataType, NULL);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, internalFormat, w, h, 0, format, dataType, NULL);

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        return texId;
    }

    GLuint loadTexture(const char *fileName, bool genMipMaps, bool invertY)
    {
        return SOIL_load_OGL_texture(fileName, SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID,
            (genMipMaps ? SOIL_FLAG_GL_MIPMAPS : 0) | (invertY ? SOIL_FLAG_INVERT_Y : 0));
    }

} // namespce textureLoader
