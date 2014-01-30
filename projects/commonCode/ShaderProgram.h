/** @file ShaderProgram.h
*  @brief useful class for shader program management and loading
*
*	@author Bartlomiej Filipek 

*/

#pragma once

#include <vector>
#include "Log.h"

class Shader;

/// manages the creation and building process of the glsl shader program
class ShaderProgram
{
private:
    GLuint mId;
    std::vector<Shader *> mShaders;
public:
    /// create shader 
    ShaderProgram();
    ~ShaderProgram();

    /// generates new id for the shader program, if the shader program was used before
    /// the it will be destroyed and a new id will be generated
    void create();
    /// adds shader to the program, the pointer is destroyed in destructor of ShaderProgram
    void attachShader(Shader *shader);
    /// links all the program and checks the link status, outputs Error using utLOG_ERROR
    bool link();

    /// validated the shader program, note that this can degradate performance, 
    /// so use this in DEBUG mode for instance
    bool validate();

    inline void  use();
    inline void  disable();
    inline GLint getUniformLocation(const char *varName);

    //
    // some useful glUniform* methods, returns false when no 'varName' found in shader
    //

    inline bool uniform1i(const char *varName, int value);
    inline bool uniform1f(const char *varName, float value);
    inline bool uniform2f(const char *varName, float x, float y);
    inline bool uniform3f(const char *varName, float x, float y, float z);
    inline bool uniform4f(const char *varName, float x, float y, float z, float w);
    inline bool uniform1fv(const char *varName, GLuint count, float *values);
    inline bool uniform2fv(const char *varName, float *values);
    inline bool uniform3fv(const char *varName, float *values);
    inline bool uniform4fv(const char *varName, float *values);

    //
    // uniform matrix
    //

    /// sends single 3x3 matrix
    inline bool uniformMatrix3f(const char *varName, float *mat, bool transpose = false);
    /// sends single 4x4 matrix
    inline bool uniformMatrix4f(const char *varName, float *mat, bool transpose = false);


    GLuint getId() const { return mId; }	
private:
    void destroy();
    void logProgramInfo();

    //
    // block copying:
    //
    ShaderProgram(const ShaderProgram &) { }
    ShaderProgram & operator=(const ShaderProgram &) { return *this; }
};


///////////////////////////////////////////////////////////////////////////////
// inline methods

inline void ShaderProgram::use()
{
    glUseProgram(mId);
}

inline void ShaderProgram::disable()
{
    glUseProgram(0);
}

inline GLint ShaderProgram::getUniformLocation(const char *varName)
{
    assert(mId > 0 && "create the program id first!");
    GLint i = glGetUniformLocation(mId, varName);

    // log msg only in the DEBUG version
#ifdef _DEBUG
    if (i == -1) 
        LOG_ERROR("uniform %s does not exist!", varName);
#endif

    return i;
}


//
// useful uniform*
//

inline bool ShaderProgram::uniform1i(const char *varName, int value)
{
    GLint i = getUniformLocation(varName);
    if (i == -1) return false;

    glUniform1i(i, value);
    return true;
}

inline bool ShaderProgram::uniform1f(const char *varName, float value)
{
    GLint i = getUniformLocation(varName);
    if (i == -1) return false;

    glUniform1f(i, value);
    return true;
}

inline bool ShaderProgram::uniform2f(const char *varName, float x, float y)
{
    GLint i = getUniformLocation(varName);
    if (i == -1) return false;

    glUniform2f(i, x, y);
    return true;
}

inline bool ShaderProgram::uniform3f(const char *varName, float x, float y, float z)
{
    GLint i = getUniformLocation(varName);
    if (i == -1) return false;

    glUniform3f(i, x, y, z);
    return true;
}

inline bool ShaderProgram::uniform4f(const char *varName, float x, float y, float z, float w)
{
    GLint i = getUniformLocation(varName);
    if (i == -1) return false;

    glUniform4f(i, x, y, z, w);
    return true;
}

inline bool ShaderProgram::uniform1fv(const char *varName, GLuint count, float *values)
{
    GLint i = getUniformLocation(varName);
    if (i == -1) return false;

    glUniform1fv(i, count, values);
    return true;
}

inline bool ShaderProgram::uniform2fv(const char *varName, float *values)
{
    GLint i = getUniformLocation(varName);
    if (i == -1) return false;

    glUniform2fv(i, 1, values);
    return true;
}

inline bool ShaderProgram::uniform3fv(const char *varName, float *values)
{
    GLint i = getUniformLocation(varName);
    if (i == -1) return false;

    glUniform3fv(i, 1, values);
    return true;
}

inline bool ShaderProgram::uniform4fv(const char *varName, float *values)
{
    GLint i = getUniformLocation(varName);
    if (i == -1) return false;

    glUniform4fv(i, 1, values);
    return true;
}

inline bool ShaderProgram::uniformMatrix3f(const char *varName, float *mat, bool transpose)
{
    GLint i = getUniformLocation(varName);
    if (i == -1) return false;

    glUniformMatrix3fv(i, 1, transpose, mat);
    return true;
}

inline bool ShaderProgram::uniformMatrix4f(const char *varName, float *mat, bool transpose)
{
    GLint i = getUniformLocation(varName);
    if (i == -1) return false;

    glUniformMatrix4fv(i, 1, transpose, mat);
    return true;
}
