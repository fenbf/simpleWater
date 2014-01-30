/** @file ShaderProrgam.cpp
*  @brief useful class for shader program management and loading
*
*	@author Bartlomiej Filipek 

*/

#include "commonCode.h"
#include <algorithm>

#include "Init.h"
#include "Log.h"
#include "Shader.h"
#include "ShaderProgram.h"

///////////////////////////////////////////////////////////////////////////////
ShaderProgram::ShaderProgram() :
    mId(0)
{

}

///////////////////////////////////////////////////////////////////////////////
ShaderProgram::~ShaderProgram()
{
    destroy();
}

///////////////////////////////////////////////////////////////////////////////
void ShaderProgram::create()
{
    if (mId > 0)
        destroy();

    mId = glCreateProgram();
}

///////////////////////////////////////////////////////////////////////////////
void ShaderProgram::attachShader(Shader *shader)
{
    assert(shader != NULL);
    assert(mId > 0 && "create the program id first!");

    mShaders.push_back(shader);
    glAttachShader(mId, shader->getId());
}

///////////////////////////////////////////////////////////////////////////////
bool ShaderProgram::link()
{
    assert(mId > 0 && "create the program id first!");

    glLinkProgram(mId);

    int success = 0;
    glGetProgramiv(mId, GL_LINK_STATUS, &success);

    if (success == GL_FALSE) 
    {
        logProgramInfo();
        return false;
    }

    return true;
}

///////////////////////////////////////////////////////////////////////////////
bool ShaderProgram::validate()
{
    glValidateProgram(mId);
    GLint status;
    glGetProgramiv( mId, GL_VALIDATE_STATUS, &status );

    if( !status ) 
        logProgramInfo();

    return status == GL_TRUE;
}

///////////////////////////////////////////////////////////////////////////////
void ShaderProgram::destroy()
{
#if _MSC_VER >= 1600
    // lambda :)
    std::for_each(mShaders.begin(), mShaders.end(), [&](Shader *sh){
        glDetachShader(mId, sh->getId());
        delete sh;
    });
#else
    for (unsigned int i = 0; i < mShaders.size(); ++i)
    {
        glDetachShader(mId, mShaders[i]->getId());
        delete mShaders[i];
    }
#endif

    mShaders.clear();

    if (mId > 0) 
        glDeleteProgram(mId);

    mId = 0;
}

///////////////////////////////////////////////////////////////////////////////
void ShaderProgram::logProgramInfo()
{
    int infologLength = 0;
    int charsWritten  = 0;

    glGetProgramiv(mId, GL_INFO_LOG_LENGTH, &infologLength);
    if (infologLength > 0)
    {
        char *infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(mId, infologLength, &charsWritten, infoLog);

        LOG(infoLog);

        free(infoLog);
    }
}

/////////////////////////////////////////////////////////////////////////////////
//void testShaderProgramClass()
//{
//	std::unique_ptr<ShaderProgram> prog(new ShaderProgram());
//
//	// todo... test...
//}